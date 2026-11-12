#!/bin/bash

set -vex

# Start from a clean slate (bash context on all platforms; on Windows this
# runs under git-bash, where `rm` is available)
rm -rf h3 build

# Clone the pinned H3 C library version
git clone --depth 1 --branch v4.5.0 https://github.com/uber/h3
cd h3

if command -v make; then
  # Unix-like: single-config build, position-independent code so the
  # static library can be linked into the shared .node addon
  cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-fPIC
else
  # Windows: let CMake pick the native (MSVC) generator
  cmake .
fi

cmake --build . --target h3 --config Release
cd ..

# Package a N-API prebuild. N-API is ABI-stable across the supported Node
# versions (22/24/26), so one prebuild per platform (os-arch-libc) is
# enough. On Linux the prebuild is tagged with its libc flavor so glibc
# and musl prebuilds can coexist in prebuilds/linux-<arch>/.
if [ "$(uname -s)" = "Linux" ]; then
  prebuildify --napi --tag-libc
else
  prebuildify --napi
fi

# Amazon Linux 2023 and other supported enterprise runners provide glibc 2.34.
# Fail the prebuild rather than publishing a binary that cannot dlopen there.
if [ "$(uname -s)" = "Linux" ]; then
  max_supported_glibc="${H3_NODE_MAX_GLIBC:-GLIBC_2.34}"

  for prebuild in prebuilds/linux-*/h3-node.glibc.node; do
    [ -e "$prebuild" ] || continue

    if ! command -v objdump >/dev/null 2>&1; then
      echo "objdump is required to verify the glibc baseline for $prebuild" >&2
      exit 1
    fi

    required_glibc=$(objdump -T "$prebuild" | grep -Eo 'GLIBC_[0-9]+(\.[0-9]+)*' | sort -uV | tail -n 1 || true)
    newest_glibc=$(printf '%s\n%s\n' "$max_supported_glibc" "$required_glibc" | sort -uV | tail -n 1)

    if [ "$newest_glibc" != "$max_supported_glibc" ]; then
      echo "$prebuild requires $required_glibc, but the maximum supported version is $max_supported_glibc" >&2
      exit 1
    fi

    echo "$prebuild requires at most $required_glibc"
  done
fi
