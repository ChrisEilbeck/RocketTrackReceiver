
#pragma once

extern bool detect_launch;
extern bool detect_apogee;
extern bool detect_lowlevel;
extern bool detect_landing;

void UpdateFlightEvents(double beaconlat,double beaconlong,double beaconalt);

enum
{
	FlightState_PreLaunch=0,
	FlightState_Armed,
	FlightState_Ascent,
	FlightState_Apogee,
	FlightState_Descent,
	FlightState_LowLevel,
	FlightState_Landed
};

