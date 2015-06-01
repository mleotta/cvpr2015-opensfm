#!/bin/bash

projectPath="/home/kitware/SfM/Data/Castle"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=$(pwd)"/SfM/features"
mkdir ${outputMatchesPath}
outputReconstructionPath=$(pwd)"/SfM/reconstruction"
mkdir ${outputReconstructionPath}

# 1 - Convert input JSON file to a PLY version

openMVG_main_ConvertSfM_DataFormat -i ./sfm_data.json -o ${outputReconstructionPath}/cameraPositions.ply

meshlab "${outputReconstructionPath}/cameraPositions.ply"

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 2 - Convert input JSON file to a PLY version
openMVG_main_ExportCameraFrustums -i ./sfm_data.json -o ${outputReconstructionPath}/frustum.ply
meshlab "${outputReconstructionPath}/frustum.ply"

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 3 - Compute some features (SIFT)
openMVG_main_ComputeFeatures -i ./sfm_data.json -o ${outputMatchesPath}/SIFT -m SIFT

# 4 - Compute from the known geometry a structure from SIFT keypoints
openMVG_main_ComputeStructureFromKnownPoses -i ./sfm_data.json -m ${outputMatchesPath}/SIFT -o ${outputReconstructionPath}/computed_SIFT_Structure.ply

meshlab "${outputReconstructionPath}/computed_SIFT_Structure.ply"

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

# 3 - Compute some features (AKAZE)
openMVG_main_ComputeFeatures -i ./sfm_data.json -o ${outputMatchesPath}/AKAZE -m AKAZE_FLOAT

# 4 - Compute from the known geometry a structure from AKAZE keypoints
openMVG_main_ComputeStructureFromKnownPoses -i ./sfm_data.json -m ${outputMatchesPath}/AKAZE -o ${outputReconstructionPath}/computed_AKAZE_Structure.ply

meshlab "${outputReconstructionPath}/computed_AKAZE_Structure.ply"

echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

pcmanfm ${outputReconstructionPath}

# Compute MORE AKAZE Keypoints and fit the structure:

# 3 - Compute some features (AKAZE)
openMVG_main_ComputeFeatures -i ./sfm_data.json -o ${outputMatchesPath}/AKAZE_HIGH -m AKAZE_FLOAT -p HIGH

# 4 - Compute from the known geometry a structure from AKAZE keypoints
openMVG_main_ComputeStructureFromKnownPoses -i ./sfm_data.json -m ${outputMatchesPath}/AKAZE_HIGH -o ${outputReconstructionPath}/computed_AKAZE_HIGH_Structure.json

openMVG_main_ComputeSfM_DataColor -i ${outputReconstructionPath}/computed_AKAZE_HIGH_Structure.json -o ${outputReconstructionPath}/computed_AKAZE_HIGH_Structure.ply

meshlab "${outputReconstructionPath}/computed_AKAZE_HIGH_Structure.ply"

