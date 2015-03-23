#!/bin/bash


sudo apt-get update
#sudo apt-get install -y --no-install-recommends ubuntu-desktop
sudo apt-get install -y lubuntu-desktop

# Install video and image tools
sudo apt-get install -y vlc

# Build tools
sudo apt-get install -y build-essential git cmake-qt-gui cmake-curses-gui

# Boost dependencies
sudo apt-get install -y libboost-timer1.55-dev libboost-program-options1.55-dev \
                        libboost-timer1.55-dev libboost-filesystem1.55-dev

sudo apt-get install -y libproj-dev


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

# Reboot for graphical login
reboot
