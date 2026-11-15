#!/bin/bash
#
# Download the prebuild artifacts that CI uploaded to the GitHub release for
# the current package version and merge them into ./prebuilds, so the package
# can be published with prebuilds bundled in (see node-gyp-build).
#
# Usage:
#   npm run fetch-artifacts        # or: bash scripts/fetch-artifacts.sh
#
# Prerequisites:
#   - The version in package.json matches a pushed tag (e.g. 4.5.0 <-> v4.5.0)
#   - CI in .github/workflows/release.yml has finished uploading all assets
#   - GitHub CLI (https://cli.github.com) installed and authenticated
#     (`gh auth status`): the release lookup is an authenticated API call,
#     so it is not subject to the 60 requests/hour unauthenticated rate
#     limit that previously made `prebuildify-ci download` fail with a
#     misleading "No release found" error
#
# After this succeeds:
#   npm publish

set -euo pipefail
shopt -s nullglob
cd "$(dirname "$0")/.."

version=$(node -p "require('./package.json').version")
tag="v${version}"

command -v gh >/dev/null 2>&1 || {
  echo "ERROR: the GitHub CLI is required (https://cli.github.com)" >&2
  exit 1
}

if ! gh release view "${tag}" --json tagName > /dev/null; then
  echo "No release found for ${tag}." >&2
  echo "  - CI in .github/workflows/release.yml may still be uploading assets" >&2
  echo "  - check that the GitHub CLI is authenticated: gh auth status" >&2
  exit 1
fi

# Download the release assets into a staging dir, extract them into a
# *separate* subdirectory, and move only the extracted contents into
# ./prebuilds, so the downloaded archives themselves never end up in the
# published tarball.
staging=$(mktemp -d "$(pwd)/.prebuilds-staging.XXXXXX")
trap 'rm -rf "${staging}"' EXIT
mkdir "${staging}/extracted"

gh release download "${tag}" --dir "${staging}"

for asset in "${staging}"/*.tar.gz; do
  echo "Extracting $(basename "${asset}")"
  tar xzf "${asset}" -C "${staging}/extracted"
done
for asset in "${staging}"/*.zip; do
  command -v unzip >/dev/null 2>&1 || {
    echo "ERROR: unzip is required to extract $(basename "${asset}")" >&2
    exit 1
  }
  echo "Extracting $(basename "${asset}")"
  unzip -q "${asset}" -d "${staging}/extracted"
done

rm -rf prebuilds
mv "${staging}/extracted" prebuilds

# Guard: only .node files (inside platform dirs) may be published.
if find prebuilds -type f ! -name '*.node' | grep -q .; then
  echo "ERROR: unexpected non-prebuild files in ./prebuilds:" >&2
  find prebuilds -type f ! -name '*.node' >&2
  exit 1
fi

echo
echo "Prebuilds now in ./prebuilds:"
find prebuilds -type f | sort || true

echo
echo "Checking Linux glibc prebuilds against the supported baseline:"
bash scripts/check-glibc-prebuilds.sh

# Warn (don't fail) if a platform is missing, e.g. CI is still running
for expected in \
  prebuilds/linux-x64/h3-node.glibc.node \
  prebuilds/linux-x64/h3-node.musl.node \
  prebuilds/linux-arm64/h3-node.glibc.node \
  prebuilds/linux-arm64/h3-node.musl.node \
  prebuilds/darwin-arm64/h3-node.node \
  prebuilds/win32-x64/h3-node.node; do
  if [ ! -f "$expected" ]; then
    echo "WARNING: $expected not found (is CI finished?)" >&2
  fi
done

echo
echo "Done. Publish with: npm publish"
