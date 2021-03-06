This directory contains scripts to build the virtual machine image.  Pre-requisites:

- Install VM software, e.g. [VirtualBox](https://www.virtualbox.org/wiki/Downloads ) or VMware
- Install [Vagrant](https://www.vagrantup.com)
- If you have Vmware Fusion, you need to buy the [Vagrant Vmware plugin](https://www.vagrantup.com/vmware)

Then, run “vagrant up” in the root of directory and it should do everything.  Right now this is

- downloading a stock Ubuntu 14.04 image
- applying all the software updates
- adding a light-weight window manager (LDXE)
- installing packages for other dependencies and tools (build tools, boost, etc.)
- creating a user with auto login and sudo privileges (default username is “kitware”)
- setting a desktop wallpaper image
- setting up desktop shortcuts to the course material
- checking out and building source code
