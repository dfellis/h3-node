#!/usr/bin/env bash
#
# Verify that Linux glibc prebuilds do not require a newer glibc than the
# supported baseline. Amazon Linux 2023 provides glibc 2.34.
#
# Usage:
#   scripts/check-glibc-prebuilds.sh [prebuild...]
#
# When no prebuild paths are supplied, every prebuilds/linux-*/h3-node.glibc.node
# file is checked.

set -euo pipefail
cd "$(dirname "$0")/.."

max_supported_glibc="${H3_NODE_MAX_GLIBC:-GLIBC_2.34}"

shopt -s nullglob
prebuilds=("$@")
if [ ${#prebuilds[@]} -eq 0 ]; then
  prebuilds=(prebuilds/linux-*/h3-node.glibc.node)
fi

if [ ${#prebuilds[@]} -eq 0 ]; then
  echo "No Linux glibc prebuilds found to check."
  exit 0
fi

if ! command -v readelf >/dev/null 2>&1; then
  echo "readelf is required to verify the glibc baseline." >&2
  exit 1
fi

for prebuild in "${prebuilds[@]}"; do
  if [ ! -f "$prebuild" ]; then
    continue
  fi

  required_glibc=$(readelf --dyn-syms --wide "$prebuild" \
    | grep -Eo 'GLIBC_[0-9]+(\.[0-9]+)*' \
    | sort -uV \
    | tail -n 1 || true)

  if [ -z "$required_glibc" ]; then
    echo "$prebuild does not declare GLIBC symbol versions."
    continue
  fi

  newest_glibc=$(printf '%s\n%s\n' "$max_supported_glibc" "$required_glibc" | sort -uV | tail -n 1)
  if [ "$newest_glibc" != "$max_supported_glibc" ]; then
    echo "$prebuild requires $required_glibc, but the maximum supported version is $max_supported_glibc." >&2
    exit 1
  fi

  echo "$prebuild requires at most $required_glibc."
done
