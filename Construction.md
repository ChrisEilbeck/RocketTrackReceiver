
# Building a RocketTrackReceiver

This is currently designed to be based on a three button TTGO T-Beam.  

The code is written using Arduino so it could be ported to other platforms
in the same way that RocketTrack has been but I am currently sticking
exclusively to the T-Beam for the receive side of the leg

# Sensors

We currently support

-	MPU 6500 gyro and accelerometer
-	HMC5883L magnetometer
-	BMP280 pressure sensor

There are plans to support other things through automatic detection and
setup, but there are on hold until I get something solidly working with a
full set of sensors first.

All sensors are hooked up on I2C and run from 3.3v.  They are wired in
parallel to the 0.96" OLED screen, mounted on top of the T-Beam itself.

As we are going to calibrate the magnetometer so the receiver can tell which
direction it's point in, it's important that the sensors are in a fixed
orientation relative to each other, and also relative to the T-Beam itself. 
I've found it works quite well if you make a stack of the sensor board by
soldering them one above another using a set of pin headers, then sticking
the sensor stack on top of the T-Beam using hot melt glue.  This is
permanent enough for in-the-field use but not so permanent as to be
non-removeable later if necessary.

Point-to-point wiring using enamelled copper wire is sufficient for this. 
Nothing runs at high current and if everything is bonded down, there is no
need to flexible wires to be used.

It is also worth adding an Openlog to the T-Beam to record anything that is
output on the serial port.  This can be done regardless of whether a PC is
hooked up to the USB port or not, and allows the flight track through the
sky, compass bearings, the track to the recovery site etc. to be logged. 
The Openlog is also used to perform calibration of the magnetometer to allow
accurate determination of where the antenna is pointing.

