#!/bin/bash

sudo apt-get update
#sudo apt-get install -y --no-install-recommends ubuntu-desktop
sudo apt-get install -y lubuntu-desktop

# Install video and image tools
sudo apt-get install -y vlc

# Install 3D viewer tools
sudo apt-get install -y meshlab paraview

# Build tools
sudo apt-get install -y build-essential git cmake-qt-gui cmake-curses-gui

# Boost dependencies (for MAP-Tk)
sudo apt-get install -y libboost-timer-dev libboost-program-options-dev \
                        libboost-filesystem-dev

# More Boost dependencies (for GTSAM)
sudo apt-get install -y libboost-chrono-dev libboost-date-time-dev \
                        libboost-regex-dev libboost-thread-dev

sudo apt-get install -y libproj-dev

# google-glog + gflags
sudo apt-get install -y libgoogle-glog-dev

# BLAS & LAPACK
sudo apt-get install -y libatlas-base-dev

# Eigen3
sudo apt-get install -y libeigen3-dev

# SuiteSparse and CXSparse (optional)
# - If you want to build Ceres as a *static* library (the default)
#   you can use the SuiteSparse package in the main Ubuntu package
#   repository:
sudo apt-get install -y libsuitesparse-dev

# Qt4
sudo apt-get install -y libqt4-dev

# ffmpeg/libav
sudo apt-get install -y libavcodec-dev libavformat-dev libswscale-dev

# Image I/O libraries
sudo apt-get install -y libjpeg-dev libpng12-dev libtiff4-dev

# Python support
sudo apt-get install -y python3-dev python3-numpy python-dev python-numpy

# Install VXL (used by MAP-Tk)
sudo apt-get install -y libvxl1-dev
# The Ubuntu VXL package has a bug in which it links to libgeotiff at
# the wrong location, so add a sym-link to work around.
sudo ln -s /usr/lib/libgeotiff.so.2 /usr/lib/libgeotiff.so

# install blender for visualization of MAP-Tk output
sudo apt-get install -y blender

#install text editors
sudo apt-get install -y emacs
