#!/bin/bash

# Install all the Ubuntu packages to customize this VM
# with the desired software and library dependencies
echo "running /vagrant/VM/install_packages.sh"
/vagrant/VM/install_packages.sh

# Setup the user, home directory, and data
VM_USER=kitware
VM_PASS='$6$.7fXqR9L$WdmJCVuIOSt8JTgmyQPe1Mt8DObFPRmUXoyC9.qzw/US0hL5Uww1aHuDeKotAblyE1yEvM1Wxg7EvgAaA5Hkt1'

# Change the hostname
echo "updating the hostname"
echo "kitware-trusty-64" > /etc/hostname

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
