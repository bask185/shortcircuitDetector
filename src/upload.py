#!/usr/bin/env python
import os
os.system("python src/build.py")
print("UPLOADING")
os.system("arduino-cli upload -b attiny:avr:ATtinyX5 -p COM4 -i ./build/attiny.avr.attinyX5/currentDetector.ino.hex")
exit