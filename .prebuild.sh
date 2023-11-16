#!/bin/bash

set -vex;

COMMON_CMAKE_ARGS=-DCMAKE_BUILD_TYPE=Release

npm run clean &&
  git clone https://github.com/uber/h3 &&
  cd h3 &&
  git checkout v4.1.0 &&
  if command -v make; then
    if [[ "${ARCH}" == "ia32" ]]; then
      cmake . ${COMMON_CMAKE_ARGS} -DCMAKE_C_FLAGS="-fPIC -m32";
    else
      cmake . ${COMMON_CMAKE_ARGS} -DCMAKE_C_FLAGS=-fPIC;
    fi
  else
    if [[ "${Platform}" == "x64" ]]; then
      cmake . ${COMMON_CMAKE_ARGS} -DCMAKE_VS_PLATFORM_NAME=${Platform} -G "Visual Studio 14 Win64";
    else
      cmake . ${COMMON_CMAKE_ARGS} -DCMAKE_VS_PLATFORM_NAME=${Platform};
    fi
  fi &&
  cmake --build . --target h3 --config Release &&
  cd .. &&
  prebuildify --target=node@`node --version | sed s/v//`

