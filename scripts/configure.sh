# set startup.sh to launch on encor session open
mkdir -p ~/.config/autostart
cat << EOF > ~/.config/autostart/thehole.desktop
[Desktop Entry]
Name=The Hole
Type=Application
Exec=lxterminal -e "/home/encor/thehole/scripts/startup.sh"
Terminal=true
EOF

# Prevent auto-sleep
sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target

# Create user for calibration
sudo adduser --quiet --home /home/calib --shell /bin/bash calib << EOF
 
 





Y
EOF

sudo groupadd thehole
sudo adduser encor thehole
sudo adduser calib thehole
sudo adduser calib sudo

sudo chown -R encor:thehole ~encor/thehole

# Allow anyone in groupe thehole to kill each other's processes
echo "%thehole localhost=(%thehole) NOPASSWD:/bin/kill" | sudo EDITOR="tee -a" visudo

# install xRDP for remote control
# thanks to https://askubuntu.com/questions/580415/how-to-remote-desktop-from-windows-to-lubuntu
# for the black screen fix
apt install -y xrdp
# restore mouse and keyboard input for local session (https://c-nergy.be/blog/?p=13390)
apt install -y xserver-xorg-input-all
echo "lxsession -e LDXE -s Lubuntu" | sudo tee ~calib/.xsession > /dev/null


# Add desktop shortcut for calibration
sudo mkdir ~calib/Desktop
sudo cp scripts/Calibration\ tool ~calib/Desktop
sudo chown -R calib:calib ~calib/Desktop
sudo chmod a+x ~calib/Desktop/Calibration\ tool
