#!/bin/bash

VERBOSE=--verbose
BOARD=esp32:esp32:t-beam:Revision=Radio_SX1276

~/bin/arduino-cli compile --fqbn ${BOARD} ${VERBOSE}




