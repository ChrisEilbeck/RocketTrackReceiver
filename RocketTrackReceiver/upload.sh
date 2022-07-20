#!/bin/bash

~/arduino-cli/arduino-cli upload \
	--port /dev/ttyUSB0 \
	--verbose \
	--fqbn esp32:esp32:t-beam

python "/home/chris/.arduino15/packages/esp32/tools/esptool_py/3.0.0/esptool.py" \
	--chip esp32 \
	--port /dev/ttyUSB0 \
	--baud 921600 \
	write_flash -z 0x290000 rockettrackreceiver.spiffs.bin
