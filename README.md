# RocketTrackReceiver

This is a firmware load to run on a TTGO T-Beam which received and processes
packets sent from a similar device running RocketTrack.  It uses LoRa (not
LoRaWan) and supports two modes

- High Rate mode for pre-flight and inflight telemetry, and
- Long Range mode for post-landing telemetry 

RTR uses an OLED display for basic operation but the primary user interface
is over a WiFi connection to a smart phone in the field or a laptop.  The
two modes which are normally used are Telemetry mode before and during
flight, and Tracking mode for recovery of the rocket after landing.

## Telemetry Mode

This shows a tabular overview of the data bursts which can be simply glanced
at before and during the flight, showing only the most important
information.

## Tracking Mode

This shows a radar-style plot giving the range and bearing of the rocket
relative to the receiver.  This is optimised for wandering across a field,
potentially in the middle of crops, to find the landed rocket.

# Building the RocketTrackReceiver Hardware



# Compiling and Uploading

build and upload scripts are included.  I use arduino-cli.  You can do the
same with the Arduino GUI.  The application relies on a SPIFFS filesystem
for its configuration data and the web pages it serves.



# Copyright

RocketTrackReceiver is written by Chris Eilbeck, chris@hyperspace.org.uk. 
No warranty as to the fitness for purpose of this development is made or
implied.

Thanks are given to Damian Burrin for his help in developing this system.

