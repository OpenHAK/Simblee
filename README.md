# RFduino: An Arduino shrunk to the size of a finger-tip and made wireless!

# Simblee: IoT for connecting Everyone and Everything IoT4EE

## Installation

* Get the [Simblee hardware](https://Simblee.com).

* Install the [FTDI drivers](http://www.ftdichip.com/Drivers/VCP.htm).

* Download [Arduino 1.6.5](http://arduino.cc/en/Main/Software) on newer.
  * _Mac OS X_: Requires Java 6 to be installed. Available [here](https://support.apple.com/kb/DL1572?locale=en_US).

* Open Arduino
  * edit preferences, and add https://www.simblee.com/package_simblee_index.json to the Additional Board Manager URLs.
  * Open the Boards Manager under Tools->Board menu.
  * Select the Simblee package at the end of the list, and click Install to install it.
  * Your new board will now show up in the Tools->Board menu.

Select the board and port, and you're ready to go!

## Linux

```
// FTDI drivers are not needed (they are built into the kernel)
wget http://downloads.arduino.cc/arduino-1.6.5-linux64.tar.xz
tar -xf arduino-1.6.3-linux64.tar.xz

Ubuntu: sudo apt-get install default-jre
Fedora: sudo yum install jre
OpenSUSE: sudo zypper install jre

// otherwise the com port open will fail with permission denied (you must logout/login for change to take effect)
sudo usermod -aG dialout <username>
```

## Getting Started

* Attach the USB shield

* Select Simblee from the ```Tools/Board``` menu

* Select the port from the ```Tools/Port``` menu

* Select an example from the ```Files/Examples/SimbleeNonBLE``` or ```Files/Examples/SimbleeBLE``` directory

* Select ```Upload``` to compile, upload and execute the sketch

* Download the Simblee for Mobile iPhone apps from the iPhone App Store (search for ```Simblee```).

* The quick start guide is available [here](https://www.simblee.com/Simblee_Quickstart_Guide_v1.0.pdf).

## SimbleeApps

Check out the [SimbleeApps](http://github.com/Simblee/SimbleeApps) repository for iPhone App source code & Simblee to Raspberry Pi integration projects.

# Communicating with us

The best way to communiate with us is on the [RFduino Forum](http://forum.RFduino.com).
