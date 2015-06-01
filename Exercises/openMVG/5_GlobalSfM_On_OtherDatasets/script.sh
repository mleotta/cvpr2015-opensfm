#!/bin/bash

projectPath="/home/kitware/SfM/Data/MirebeauStHilaireStatue"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=${projectPath}"/SfM/matches"
outputReconstructionPath=$(pwd)"/SfM/reconstruction_PoitiersCathedralMainEntranceDoor"
mkdir ${outputReconstructionPath}

# 1 - Run Global SfM On precomputed features & matches data

openMVG_main_GlobalSfM -i ${outputMatchesPath}/sfm_data.json -m ${outputMatchesPath} -o ${outputReconstructionPath}

openMVG_main_ComputeSfM_DataColor -i ${outputReconstructionPath}/sfm_data.json -o ${outputReconstructionPath}/colorized.ply

meshlab "${outputReconstructionPath}/colorized.ply"


echo "\n"
read -p "Wait instructions and press a key to continue" key
echo "\n"

projectPath="/home/kitware/SfM/Data/PoitiersCathedralMainEntranceDoor"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=${projectPath}"/SfM/matches"
outputReconstructionPath=$(pwd)"/SfM/reconstruction_PoitiersCathedralMainEntranceDoor"
mkdir ${outputReconstructionPath}

# 1 - Run Global SfM On precomputed features & matches data

openMVG_main_GlobalSfM -i ${outputMatchesPath}/sfm_data.json -m ${outputMatchesPath} -o ${outputReconstructionPath}

openMVG_main_ComputeSfM_DataColor -i ${outputReconstructionPath}/sfm_data.json -o ${outputReconstructionPath}/colorized.ply

meshlab "${outputReconstructionPath}/colorized.ply"


