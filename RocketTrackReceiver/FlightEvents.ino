
#include "FlightEvents.h"

bool detect_launch=false;
bool detect_apogee=false;
bool detect_lowlevel=false;
bool detect_landing=false;

char flight_status[32]="Unknown";

double launch_altitude=0;
int FlightState=FlightState_PreLaunch;

void UpdateFlightEvents(double beaconlat,double beaconlong,double beaconalt)
{
	switch(FlightState)
	{
		case FlightState_PreLaunch:	{
										static int fixcounter=0;
										
										launch_altitude+=beaconalt;
										fixcounter++;
										
										if(fixcounter>=10)
										{
											launch_altitude/=(double)fixcounter;
											FlightState=FlightState_Armed;
											Serial.println("FlightEvent system armed");
											Serial.print("Launch altitude: ");
											Serial.print(launch_altitude);
											Serial.println(" m");
										}
									}
									
									break;
		
		case FlightState_Armed:		
									break;
		
		case FlightState_Ascent:
									break;
		
		case FlightState_Apogee:
									break;
		
		case FlightState_Descent:
									break;
		
		case FlightState_LowLevel:
									break;
		
		case FlightState_Landed:
									break;
	
		default:					Serial.print("Invalid Flight State - ");
									Serial.println(FlightState);
									break;
	}
}

