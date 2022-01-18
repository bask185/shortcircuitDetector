#!/usr/bin/env python
import os
os.system("python src/build.py")
print("UPLOADING")
os.system("arduino-cli upload -b arduino:avr:attiny45 -p COM1 -i ./build/arduino.avr.attiny45/currentDetector.ino.hex")
exit