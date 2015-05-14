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
sudo apt-get install -y libboost-timer1.55-dev libboost-program-options1.55-dev \
                        libboost-timer1.55-dev libboost-filesystem1.55-dev

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

# Setup the user, home directory, and data
VM_USER=kitware
VM_PASS='$6$.7fXqR9L$WdmJCVuIOSt8JTgmyQPe1Mt8DObFPRmUXoyC9.qzw/US0hL5Uww1aHuDeKotAblyE1yEvM1Wxg7EvgAaA5Hkt1'

# Change the hostname
echo "updating the hostname"
echo "kitware-trusty-32" > /etc/hostname

# Set the desktop wallpaper
echo "copy the KW wallpaper"
cp /vagrant/VM/Desktop_KW.jpg /usr/share/lubuntu/wallpapers/

echo "creating user $VM_USER"
useradd -m -s /bin/bash -p $VM_PASS $VM_USER

echo "running /vagrant/VM/user_setup.sh"
sudo -u $VM_USER /vagrant/VM/user_setup.sh $VM_USER

echo "running /vagrant/VM/software_setup.sh"
sudo -u $VM_USER /vagrant/VM/software_setup.sh $VM_USER

# Setup Auto-login
echo "setup auto login"
cat > /etc/lightdm/lightdm.conf.d/30-autologin.conf << DELIM
[SeatDefaults]
autologin-user=$VM_USER
DELIM

# Setup sudo permission
echo "add sudo permission"
cat > /etc/sudoers.d/$VM_USER << DELIM
$VM_USER ALL=(ALL) NOPASSWD:ALL
DELIM

# Disable the lock screen from auto starting
sudo mv /etc/xdg/autostart/light-locker.desktop /etc/xdg/autostart/light-locker.desktop.bak

# Reboot for graphical login
reboot
