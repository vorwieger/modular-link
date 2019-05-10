# modular-link
Ableton link on Raspberry Pi

# Create Boot-Disk for Raspberry Pi

## Copy Image from macOS to SC-Card
```
diskutil unmountDisk /dev/disk2
pv 2019-04-08-raspbian-stretch-lite.img | sudo dd bs=1m of=/dev/disk2
diskutil mountDisk /dev/disk2
```

## SSH
```
touch /Volumes/boot/ssh
```

## WLAN
```
echo -e 'country=DE\nctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\nupdate_config=1\nnetwork={\nssid="WLAN-NRQWKT"\npsk="###PASSWORD##"\nkey_mgmt=WPA-PSK\n}' > /Volumes/boot/wpa_supplicant.conf
```

```
diskutil unmountDisk /dev/disk2
```

# Prepare

## Raspi Config
`sudo raspi-config`

* Advanced Configuration ->  Expand Filesystem

## Packages
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git cmake wiringpi
sudo apt-get install librtmidi-dev
```

## Install AFP - Fileshare from macOD
### Raspberry Pi
```
sudo apt-get install netatalk
```

### macOS
```
open afp://pi@raspberrypi
```

# Build
```
git clone https://github.com/vorwieger/modular-link.git
cd modular-link
git submodule update --init --recursive
```

```
    make
```

## Install
```
    sudo cp modlink.service /etc/systemd/system/
    sudo chmod u-w-x /etc/systemd/system/modlink.service
    sudo systemctl enable modlink
```

## Start /Stop
```
    sudo systemctl start modlink
```

```
    sudo systemctl stop modlink
```
