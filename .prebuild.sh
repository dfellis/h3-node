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
