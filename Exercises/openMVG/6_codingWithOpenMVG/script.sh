#!/bin/bash

mkdir build
cd build
cmake . ../
make
./main -i ../../4_FromCameraPosesToStructure/sfm_data.json
