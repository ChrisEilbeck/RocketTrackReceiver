#!/bin/bash

~/arduino-cli/arduino-cli compile \
	--verbose \
	--fqbn esp32:esp32:t-beam \
	RocketTrackReceiver.ino 
