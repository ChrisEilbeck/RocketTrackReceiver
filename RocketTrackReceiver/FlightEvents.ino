
#include "FlightEvents.h"

bool detect_launch=false;
bool detect_apogee=false;
bool detect_lowlevel=false;
bool detect_landing=false;

char flight_status[32]="Unknown";

double launch_altitude=0;
double apogee_altitude=0;

int FlightState=FlightState_PreLaunch;

char flight_state_text[65]="Arming ...";

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

											strcpy(flight_state_text,"Ready");

											Serial.println("FlightEvent system armed");
											Serial.print("Launch altitude: ");
											Serial.print(launch_altitude);
											Serial.println(" m");
										}
									}
									
									break;
		
		case FlightState_Armed:		{
										if(beaconalt>(launch_altitude+25))
										{
											FlightState=FlightState_Ascent;
											strcpy(flight_state_text,"Coast to<br>Apogee");
											Serial.println("Launch detected, coasting to apogee");
										}
									}
									
									break;
		
		case FlightState_Ascent:	{
										if(beaconalt>apogee_altitude)	
											apogee_altitude=beaconalt;
										else 
											if(beaconalt<(apogee_altitude-10))
											{
												FlightState=FlightState_Apogee;
												strcpy(flight_state_text,"Apogee");
												Serial.println("Apogee detected");
											}
									}
									
									break;
		
		case FlightState_Apogee:	{
										if(beaconalt>(apogee_altitude-100))
										{
											FlightState=FlightState_Descent;
											strcpy(flight_state_text,"Descent");
											Serial.println("Entered descent phase");
										}
									}
									
									break;
		
		case FlightState_Descent:	{
										if(beaconalt<(apogee_altitude-launch_altitude+200))
										{
											FlightState=FlightState_LowLevel;
											strcpy(flight_state_text,"Main chute<br>deployment");
											Serial.println("Main chute deployment at 200m AGL");
										}
									}
		
									break;
		
		case FlightState_LowLevel:	{
										// wait for the beacon altitude to stop changing
										
										static double last_altitude=-1000;
		
										if((beaconalt-last_altitude)<3)
										{
											FlightState=FlightState_Landed;
											strcpy(flight_state_text,"Landed");
											Serial.println("Landing detected");
										}
									}
									
									break;
		
		case FlightState_Landed:	{
										// hang in this state ...
									}
									
									break;
	
		default:					Serial.print("Invalid Flight State - ");
									Serial.println(FlightState);
									break;
	}
}

