# modular-link
Ableton Link on Raspberry Pi

Inspired by the nice project "https://github.com/shaduzlabs/pink-0" I build my own module with some modifications in terms of used components. The software part has been completely rewritten.

The company that offers an Ableton Link device as a stand-alone product should not go unmentioned: Missing Link from http://circuithappy.com (By the way I have no affiliation with this company).

### Changes/Features
* Complete rewrite of methods for the trigger outputs
* Using MAX 7219 for driving 7 segment display
* Debouncing of Start/Stop Button (in Software)

### ToDo's
* Midi-Clock as Output over USB
* Possibility to log in to an external WLAN
* Raspberry Pi as WLAN Access Point

## Eurorack Module
![doc/modular-link-1.jpg]
![doc/modular-link-2.jpg]

## Hardware Wiring

### Eagle Schema
![doc/schema.pdf]

### Mappings between wiriingPi and physical Pins

`pi@raspberrypi:~ $ gpio readall`
```+-----+-----+---------+------+---+-Pi ZeroW-+---+------+---------+-----+-----+
| BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
+-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
|     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
|   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5v      |     |     |
|   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
|   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 0 | IN   | TxD     | 15  | 14  |
|     |     |      0v |      |   |  9 || 10 | 1 | IN   | RxD     | 16  | 15  |
|  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 1 | OUT  | GPIO. 1 | 1   | 18  |
|  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
|  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
|     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
|  10 |  12 |    MOSI |  OUT | 0 | 19 || 20 |   |      | 0v      |     |     |
|   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
|  11 |  14 |    SCLK |  OUT | 1 | 23 || 24 | 1 | OUT  | CE0     | 10  | 8   |
|     |     |      0v |      |   | 25 || 26 | 0 | OUT  | CE1     | 11  | 7   |
|   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
|   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
|   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
|  13 |  23 | GPIO.23 |  OUT | 1 | 33 || 34 |   |      | 0v      |     |     |
|  19 |  24 | GPIO.24 |   IN | 1 | 35 || 36 | 1 | IN   | GPIO.27 | 27  | 16  |
|  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
|     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
+-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
| BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
+-----+-----+---------+------+---+-Pi ZeroW-+---+------+---------+-----+-----+
```


## Installation on Raspberry Pi

### Create Boot-Disk

Copy Image from macOS to SD-Card
```
diskutil unmountDisk /dev/disk2
pv 2019-04-08-raspbian-stretch-lite.img | sudo dd bs=1m of=/dev/disk2
diskutil mountDisk /dev/disk2
```

Enable SSH
```
touch /Volumes/boot/ssh
```

Enable WLAN
```
echo -e 'country=DE\nctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\nupdate_config=1\nnetwork={\nssid="###WLAN-SSID###"\npsk="###PASSWORD###"\nkey_mgmt=WPA-PSK\n}' > /Volumes/boot/wpa_supplicant.conf
```
Replace ###WLAN-SSID### and ###PASSWORD## with your concrete values. 

Unmount disk
```
diskutil unmountDisk /dev/disk2
```

Now put SD-Card into Raspberry and boot...

### Raspi Config
`sudo raspi-config`

* Advanced Configuration ->  Expand Filesystem

### Packages
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git cmake wiringpi
sudo apt-get install librtmidi-dev
```

### Build
```
git clone https://github.com/vorwieger/modular-link.git
cd modular-link
git submodule update --init --recursive
```

```
    make
```

### Install
```
    sudo cp modlink.service /etc/systemd/system/
    sudo chmod u-w-x /etc/systemd/system/modlink.service
    sudo systemctl enable modlink
```

### Start /Stop Service
```
    sudo systemctl start modlink
```

```
    sudo systemctl stop modlink
```
