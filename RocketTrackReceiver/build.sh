#!/bin/bash

##VERBOSE=--verbose
BOARD=esp32:esp32:t-beam:Revision=Radio_SX1276

rm data/*~ 2>/dev/null

mkspiffs -c data -b 4096 -p 256 -s 0x160000 rockettrackreceiver.spiffs.bin

~/bin/arduino-cli compile --fqbn ${BOARD} ${VERBOSE}

