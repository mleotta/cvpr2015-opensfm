#!/bin/bash

VM_USER=$1
WORK_DIR=/home/$VM_USER/SfM
SOFTWARE_DIR=$WORK_DIR/Software

# Create the Software Directory
mkdir -p $SOFTWARE_DIR
cd $SOFTWARE_DIR

git clone -b v0.5.0 https://github.com/Kitware/maptk.git maptk/src
mkdir maptk/bld



