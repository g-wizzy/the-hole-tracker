#!/bin/bash

# install dependencies
apt -y install curl libglfw3{,-dev} libglew-dev libopencv-{core,imgproc,objdetect}-dev libfreeimage-dev libboost-filesystem-dev libpugixml-dev libopenni{,2}-dev
# remove automatically installed package -y
apt -y purge openni-utils -y

# install RealSense SDK 2.35
apt-key adv --keyserver keys.gnupg.net --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE || sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE
add-apt-repository "deb http://realsense-hw-public.s3.amazonaws.com/Debian/apt-repo bionic main" -u
apt-get -y install librealsense2-dev

# prepare to install Nuitrack
wget https://launchpad.net/~ubuntu-security/+archive/ubuntu/ppa/+build/15108504/+files/libpng12-0_1.2.54-1ubuntu1.1_amd64.deb -O libpng.deb
dpkg -i libpng.deb
rm libpng.deb

# fix logname error on Lubuntu 18.04
mv /usr/bin/logname{,bckp}
cat << EOF > /usr/bin/logname
#!/bin/bash
echo \$USER
EOF
chmod a+x /usr/bin/logname

# install Nuitrack
wget http://download.3divi.com/Nuitrack/platforms/nuitrack-ubuntu-amd64.deb -O nuitrack.deb
dpkg -i nuitrack.deb
rm nuitrack.deb

# restore actual logname program
mv /usr/bin/logname{bckp,}

# install xRDP for remote control
# thanks to https://askubuntu.com/questions/580415/how-to-remote-desktop-from-windows-to-lubuntu
# for the black screen fix
apt install -y xrdp
echo "lxsession -e LDXE -s Lubuntu" > ~/.xsession

# display the calibrator script on the desktop for ease of access
cp Calibration\ tool ~/Desktop

# set startup.sh to launch on session open
mkdir -p ~/.config/autostart
cat << EOF > ~./config/autostart/TheHole.desktop
[Desktop Entry]
Name=The Hole Tracker
Exec=lxterminal -e "home/encor/Desktop/thehole/scripts/startup.sh"
Terminal=true
EOF