# RocketTrackReceiver

This is a firmware load to run on a TTGO T-Beam which receives and processes
packets sent from a similar device running [RocketTrack](https://github.com/ChrisEilbeck/RocketTrack)

It uses LoRa (not LoRaWan) and supports two modes

- High Rate mode for pre-flight and inflight telemetry, and
- Long Range mode for post-landing telemetry 

RTR uses an OLED display for basic operation but the primary user interface
is over a WiFi connection to a smart phone in the field or a laptop.  The
two modes which are normally used are Telemetry mode before and during
flight, and Tracking mode for recovery of the rocket after landing.

It is intended that the receiver is mounted directly on the beam of a Yagi-Uda
antenna so optimum range.  It can be used with just a monopole or rubber
ducky antenna but the performance will be reduced.

## Telemetry Mode

This shows a tabular overview of the data bursts which can be simply glanced
at before and during the flight, showing only the most important
information.  A barometric sensor is used to give accurate altitude
difference between the beacon and the receiver.

## Tracking Mode

This shows a radar-style plot giving the range and bearing of the rocket
relative to the receiver.  This is optimised for wandering across a field,
potentially in the middle of crops, to find the landed rocket.

Tracking Mode can operate in two different ways, north-up where up on the
web display is locked to True North, and beacon mode where the radar plot
will rotate based on inertial and magnetometer readings so as to have up on
the web display being wherever the antenna is pointing.

This latter feature requires that the T-Beam is fitted with sensors to
support it.  These are currently an MPU6500 and a HMMC5883L.

Calibrating the magnetometer is covered [here](/MagnetometerCalibration)

# Building the RocketTrackReceiver Hardware

This is covered in a separate instruction.  See [here](/Construction)

# Compiling and Uploading

build and upload scripts are included.  I use arduino-cli.  You can do the
same with the Arduino GUI.  The application relies on a SPIFFS filesystem
for its configuration data and the web pages it serves.  The upload scripts
generates this filesystem and uploads it to the T-Beam too.  In the gui, I
think you need to do something like Upload Sketch Data.

# Copyright

RocketTrackReceiver is written by Chris Eilbeck, chris@hyperspace.org.uk. 
No warranty as to the fitness for purpose of this development is made or
implied.  

It was initially based on work done by Dave Akerman using his Flextrack for
high altitude ballooning but I'm pretty sure there is no common code between
the two systems any more.  All credit to Dave for the inspiration.

Thanks are given to Damian Burrin for his help in developing this system.

