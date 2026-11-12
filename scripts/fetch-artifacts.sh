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
#
# After this succeeds:
#   npm publish

set -euo pipefail
cd "$(dirname "$0")/.."

# prebuildify-ci renames ./prebuilds out of the way, so it must exist
mkdir -p prebuilds

npx prebuildify-ci download

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
