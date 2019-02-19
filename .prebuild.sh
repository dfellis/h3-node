#!/bin/bash

set -vex;
npm run clean &&
  git clone https://github.com/uber/h3 &&
  cd h3 &&
  git checkout v3.4.0 &&
  if command -v make; then
    cmake . -DCMAKE_C_FLAGS=-fPIC && make;
  else
    cmake . -DCMAKE_VS_PLATFORM_NAME=${Platform} `if [[ "${Platform}" == "x64" ]]; then echo "-G \"Visual Studio 14 Win64\""; fi` && msbuild.exe h3.vcxproj;
  fi &&
  cd .. &&
  prebuildify --target=node@`node --version | sed s/v//`

