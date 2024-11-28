#!/usr/bin/bash

set -e

module load cmake gcc/12.3.0 eigen

git clone --recurse-submodules --single-branch --depth 1 https://github.com/yoann-dufresne/rHashGen.git

cd rHashGen
mkdir -p release
cd release

cmake -DCMAKE_BUILD_TYPE=Release -DEDO=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON ..
make

