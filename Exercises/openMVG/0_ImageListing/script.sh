#!/bin/bash

projectPath="/home/kitware/SfM/Data/Castle"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=$(pwd)"/SfM/matches"

# 1 - Image listing
# ( list image information - size + focal length )

openMVG_main_SfMInit_ImageListing -i "${inputImagePath}" -o "${outputMatchesPath}" -d /usr/local/share/openMVG/cameraGenerated.txt -g 0

gedit ${outputMatchesPath}/sfm_data.json

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 2 - Image listing (shared intrinsic camera parameters)

openMVG_main_SfMInit_ImageListing -i "${inputImagePath}" -o "${outputMatchesPath}" -d /usr/local/share/openMVG/cameraGenerated.txt

gedit ${outputMatchesPath}/sfm_data.json

