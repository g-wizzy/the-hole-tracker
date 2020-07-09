#!/bin/bash

cd ~/thehole
chmod a+x the-hole* realsense-model scripts/*.sh

scripts/install_deps.sh
scripts/configure.sh

echo "You should run nuitrack_license_tool to activate a D415 camera on this computer if you intend to use it."
echo "Setup done. Run ~/thehole/scripts/startup.sh to test the tracker."
echo "Calibrate this station from the Windows PC by connecting to the following address :"
echo
ip addr show | grep 192
echo
echo "On next restart, this station will run autonomously."
