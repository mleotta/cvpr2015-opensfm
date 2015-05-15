#!/bin/bash

# Install OpenCV 2.4 packages
# Note: This PPA provides the non-free compoenets like SIFT and SURF
#       that are not available through the stock Ubuntu packages
sudo add-apt-repository --yes ppa:xqms/opencv-nonfree
sudo apt-get update
sudo apt-get install -y libopencv-dev libopencv-nonfree-dev


VM_USER=$1
WORK_DIR=/home/$VM_USER/SfM
SOFTWARE_DIR=$WORK_DIR/Software

# Create the Software Directory
mkdir -p $SOFTWARE_DIR


# Download and build OpenCV 3.0
#cd $SOFTWARE_DIR
#mkdir -p opencv/bld
#cd opencv
#wget https://github.com/Itseez/opencv/archive/3.0.0-beta.zip -O opencv-3.0.0-beta.zip
#unzip opencv-3.0.0-beta.zip
#cd bld
#cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local \
#      -D WITH_TBB=ON -D WITH_IPP=OFF -D WITH_QT=ON -D WITH_OPENGL=ON ../opencv-3.0.0-beta
#make -j2
#sudo make install

# Checkout and build MAP-Tk
cd $SOFTWARE_DIR
git clone -b v0.5.0 https://github.com/Kitware/maptk.git maptk/src
mkdir maptk/bld
cd maptk/bld
cmake $SOFTWARE_DIR/maptk/src -DMAPTK_ENABLE_PROJ:BOOL=ON \
                              -DMAPTK_ENABLE_OPENCV:BOOL=ON \
                              -DMAPTK_ENABLE_VXL:BOOL=ON
make -j2
sudo make install

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
sudo make install

# Download and build GTSAM
cd $SOFTWARE_DIR
mkdir -p gtsam/bld
cd gtsam
wget https://research.cc.gatech.edu/borg/sites/edu.borg/files/downloads/gtsam-3.2.1.tgz
tar zxf gtsam-3.2.1.tgz
cd bld
cmake ../gtsam-3.2.1
make -j2
make check
sudo make install
