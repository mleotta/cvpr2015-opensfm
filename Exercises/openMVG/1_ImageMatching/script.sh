#!/bin/bash

projectPath="/home/kitware/SfM/Data/Castle"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=$(pwd)"/SfM/matches"

# 1 - Image listing
# ( list image information - size + focal length )

openMVG_main_SfMInit_ImageListing -i "${inputImagePath}" -o "${outputMatchesPath}" -d /usr/local/share/openMVG/cameraGenerated.txt

# 2 - Features computation
# ( Compute per image a list of features & descriptors )

openMVG_main_ComputeFeatures -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath}

# export the features positions as SVG files
openMVG_main_exportKeypoints -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -o ${outputMatchesPath}/Keypoints_svg

# open the file with Firefox for best viewing
pcmanfm ${outputMatchesPath}/Keypoints_svg

echo "\n"
echo "View extracted feature positions (open SVG files with firefox)"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 3 - Matches computation
# ( Compute reliable matches between the picture set )

openMVG_main_ComputeMatches -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath} -f 1

# export found putative matches as SVG files (raw distance ratio descriptor matches)
openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.putative.txt -o ${outputMatchesPath}/Matches_Putative_svg

# export found geometric matches as SVG files
openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.f.txt -o ${outputMatchesPath}/Matches_Geometric_svg

pcmanfm ${outputMatchesPath}

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 4 - Matches computation
# Use a distance ratio that filter less the repetitives matches) 
# ( Compute reliable matches between the picture set )

cp ${outputMatchesPath}/matches.f.txt ${outputMatchesPath}/matches.f_OLD.txt

openMVG_main_ComputeMatches -i "${outputMatchesPath}/sfm_data.json" -o ${outputMatchesPath} -f 1 -r .8

openMVG_main_exportMatches -i "${outputMatchesPath}/sfm_data.json" -d ${outputMatchesPath} -m ${outputMatchesPath}/matches.f.txt -o ${outputMatchesPath}/Matches_Geometric_08_svg

echo "Compare the size of the file matches.f and matches.f_OLD"

