#!/bin/bash

projectPath="/home/kitware/SfM/Data/Castle"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=$(pwd)"/SfM/matches"

# 1 - Image listing
# ( list image information - size + focal length )

openMVG_main_SfMInit_ImageListing -i "${inputImagePath}" -o "${outputMatchesPath}" -d /usr/local/share/openMVG/cameraGenerated.txt

gedit ${outputMatchesPath}/sfm_data.json

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 2 - Features computation
# ( Compute per image a list of features & descriptors )

openMVG_main_ComputeFeatures -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath}

openMVG_main_exportKeypoints -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -o ${outputMatchesPath}/Keypoints_svg

pcmanfm ${outputMatchesPath}/Keypoints_svg

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 3 - Matches computation
# ( Compute reliable matches between the picture set )

openMVG_main_ComputeMatches -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath} -f 1

openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.putative.txt -o ${outputMatchesPath}/Matches_Putative_svg

openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.f.txt -o ${outputMatchesPath}/Matches_Geometric_svg

pcmanfm ${outputMatchesPath}

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 4 - Matches computation
# Use a distance ratio that filter less the repetitives matches) 
# ( Compute reliable matches between the picture set )

openMVG_main_ComputeMatches -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath} -f 1 -r .8

openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.f.txt -o ${outputMatchesPath}/Matches_Geometric_08_svg

