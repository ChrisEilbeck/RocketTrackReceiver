#!/bin/bash

rm data/*~ 2>/dev/null

mkspiffs -c data -b 4096 -p 256 -s 0x170000 rockettrackreceiver.spiffs.bin

arduino-cli compile \
	--fqbn esp32:esp32:t-beam \
	RocketTrackReceiver.ino 
