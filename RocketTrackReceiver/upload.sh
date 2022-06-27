#!/bin/bash

~/arduino-cli/arduino-cli upload \
	--port /dev/ttyUSB1 \
	--verbose \
	--fqbn esp32:esp32:t-beam

