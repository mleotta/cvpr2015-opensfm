#!/bin/bash

VM_USER=$1
WORK_DIR=/home/$VM_USER/SfM
SOFTWARE_DIR=$WORK_DIR/Software

# Create the Software Directory
mkdir -p $SOFTWARE_DIR


# Download and build OpenCV
cd $SOFTWARE_DIR
mkdir -p opencv/bld
cd opencv
wget https://github.com/Itseez/opencv/archive/3.0.0-beta.zip -O opencv-3.0.0-beta.zip
unzip opencv-3.0.0-alpha.zip
cd bld
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D WITH_TBB=ON -D WITH_IPP=OFF -D WITH_QT=ON -D WITH_OPENGL=ON ../opencv-3.0.0-beta
make -j2

# Checkout and build MAP-Tk
cd $SOFTWARE_DIR
git clone -b v0.5.0 https://github.com/Kitware/maptk.git maptk/src
mkdir maptk/bld
cd maptk/bld
cmake $SOFTWARE_DIR/maptk/src -DMAPTK_ENABLE_PROJ:BOOL=ON
make -j2

# Download and build Ceres Solver
cd $SOFTWARE_DIR
mkdir -p ceres/bld
cd ceres
wget http://ceres-solver.org/ceres-solver-1.10.0.tar.gz
tar zxf ceres-solver-1.10.0.tar.gz
cd bld
cmake ../ceres-solver-1.10.0
make -j2
make test
