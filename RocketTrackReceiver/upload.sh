#!/bin/bash

PORT=/dev/ttyUSB0

arduino-cli upload \
	--port ${PORT} \
	--verbose \
	--fqbn esp32:esp32:t-beam

#esptool \
python3 ~/.arduino15/packages/esp32/tools/esptool_py/4.5/esptool.py --chip esp32 \
	--port ${PORT} \
	--baud 921600 \
	write_flash -z 0x290000 rockettrackreceiver.spiffs.bin
