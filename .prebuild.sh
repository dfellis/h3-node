#!/bin/bash

set -vex;
npm run clean &&
  git clone https://github.com/uber/h3 &&
  cd h3 &&
  git checkout v3.4.0 &&
  if command -v make; then
    if [[ "${ARCH}" == "ia32" ]]; then
      cmake . -DCMAKE_C_FLAGS="-fPIC -m32" && make;
    else
      cmake . -DCMAKE_C_FLAGS=-fPIC && make;
    fi
  else
    if [[ "${Platform}" == "x64" ]]; then
      cmake . -DCMAKE_VS_PLATFORM_NAME=${Platform} -G "Visual Studio 14 Win64" && msbuild.exe h3.vcxproj;
    else
      cmake . -DCMAKE_VS_PLATFORM_NAME=${Platform} && msbuild.exe h3.vcxproj;
    fi
  fi &&
  cd .. &&
  prebuildify --target=node@`node --version | sed s/v//`

