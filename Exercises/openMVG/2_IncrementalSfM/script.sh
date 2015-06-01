#!/bin/bash

projectPath="/home/kitware/SfM/Data/Castle"
inputImagePath=${projectPath}"/images"
mkdir $(pwd)"/SfM"
outputMatchesPath=$(pwd)"/SfM/matches"
outputReconstructionPath=$(pwd)"/SfM/reconstruction"
mkdir ${outputReconstructionPath}

# 1 - Image listing
# ( list image information - size + focal length )

openMVG_main_SfMInit_ImageListing -i ${inputImagePath} -o ${outputMatchesPath} -d /usr/local/share/openMVG/cameraGenerated.txt

# 2 - Features computation
# ( Compute per image a list of features & descriptors )

openMVG_main_ComputeFeatures -i ${outputMatchesPath}/sfm_data.json -o ${outputMatchesPath}

# 3 - Matches computation
# ( Compute reliable matches between the picture set )

openMVG_main_ComputeMatches -i ${outputMatchesPath}/sfm_data.json -o ${outputMatchesPath}

# 4 - Incremental reconstruction

openMVG_main_IncrementalSfM -i ${outputMatchesPath}/sfm_data.json -m ${outputMatchesPath} -o ${outputReconstructionPath}/incremental

# View the SfM statistic report
firefox ${outputReconstructionPath}/incremental/SfMReconstruction_Report.html

openMVG_main_ComputeSfM_DataColor -i ${outputReconstructionPath}/incremental/sfm_data.json -o ${outputReconstructionPath}/incremental/colorized.ply

# View the SfM result (camera poses and structure point cloud)
meshlab "${outputReconstructionPath}/incremental/colorized.ply"

pcmanfm ${outputReconstructionPath}/incremental
