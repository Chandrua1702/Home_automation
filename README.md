====================================================
ESP32-WROOM PROJECT SETUP GUIDE (FROM SCRATCH)
====================================================

This document explains how to set up the ESP32-WROOM
home automation project on a NEW system after the
code is already pushed to the repository.

----------------------------------------------------
1. PREREQUISITES
----------------------------------------------------

Hardware:
- ESP32-WROOM Dev Board
- USB cable (data cable)
- Relay module connected properly

Software:
- Linux (Ubuntu recommended) OR Windows 10/11
- Internet connection

----------------------------------------------------
2. INSTALL REQUIRED TOOLS
----------------------------------------------------

2.1 Install Git
----------------
Linux:
sudo apt update
sudo apt install git

Windows:
Download and install Git from:
https://git-scm.com

----------------------------------------------------
2.2 Install Python
------------------
Required version: Python 3.8 or above

Linux:
sudo apt install python3 python3-pip

Windows:
Download from:
https://www.python.org
(Select "Add Python to PATH")

Verify:
python3 --version

----------------------------------------------------
3. INSTALL ESP-IDF
----------------------------------------------------

3.1 Clone ESP-IDF
-----------------
git clone -b v5.1 https://github.com/espressif/esp-idf.git
cd esp-idf

3.2 Install ESP-IDF tools
-------------------------
Linux:
./install.sh esp32

Windows:
install.bat

3.3 Export ESP-IDF environment
-------------------------------
Linux:
. ./export.sh

Windows:
export.bat

NOTE:
This command must be run in EVERY new terminal.

----------------------------------------------------
4. CLONE PROJECT CODE
----------------------------------------------------

Go to your workspace directory and clone the project:

git clone <your-repository-url>
cd home_automation

Example structure after clone:
home_automation/
├── main/
├── components/
├── CMakeLists.txt
├── sdkconfig
├── README.txt
└── SETUP.txt

----------------------------------------------------
5. CONNECT ESP32 BOARD
----------------------------------------------------

- Connect ESP32 to PC using USB cable
- Check serial port

Linux:
ls /dev/ttyUSB*

Windows:
Check COM port in Device Manager

----------------------------------------------------
6. CONFIGURE PROJECT
----------------------------------------------------

Set ESP32 target:
idf.py set-target esp32

Open menu configuration (optional):
idf.py menuconfig

----------------------------------------------------
7. BUILD PROJECT
----------------------------------------------------

idf.py build

If build fails:
- Check ESP-IDF is exported
- Check Python version
- Check no missing components

----------------------------------------------------
8. FLASH AND MONITOR
----------------------------------------------------

Linux example:
idf.py -p /dev/ttyUSB0 flash monitor

Windows example:
idf.py -p COM3 flash monitor

----------------------------------------------------
9. EXPECTED SERIAL OUTPUT
----------------------------------------------------

Example logs:
WIFI_MANAGER: Wi-Fi AP started
WIFI_MANAGER: AP IP: 192.168.4.1
WEB_SERVER  : Web server started

----------------------------------------------------
10. ACCESS WEB INTERFACE
----------------------------------------------------

1. Connect mobile / laptop to ESP32 Wi-Fi AP
2. Open browser
3. Enter URL:

http://192.168.4.1

You should see the relay control page.

----------------------------------------------------
11. COMMON ISSUES & FIXES
----------------------------------------------------

Issue: idf.py not found
Fix : Run export.sh / export.bat

Issue: Permission denied on ttyUSB
Fix : sudo chmod 666 /dev/ttyUSB0

Issue: Board not detected
Fix : Try different USB cable / port

----------------------------------------------------
12. SAFETY WARNING
----------------------------------------------------

- Do NOT touch live AC wiring
- Use relay modules with isolation
- Enclose final hardware properly

----------------------------------------------------
13. SETUP COMPLETE
----------------------------------------------------

ESP32 home automation project is now ready
to use and develop further.

====================================================
END OF SETUP FILE
====================================================
