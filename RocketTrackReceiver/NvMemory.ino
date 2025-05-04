
#include <EEPROM.h>

#include "NvMemory.h"

int SetupNvMemory(void)
{
	EEPROM.begin(EEPROM_SIZE);
	return(0);
}

void PollNvMemory(void)
{
	// not used but retained as a common module structure
}

int NvMemoryCommandHandler(uint8_t *cmd,uint16_t cmdptr)
{
	// ignore a single key stroke
	if(cmdptr<=2)	return(0);

#if (DEBUG>0)
	Serial.println((char *)cmd);
#endif
	
	int retval=1;
	uint8_t cnt;
	
	switch(cmd[1]|0x20)
	{
		case 'x':	// hexdump of the eeprom for debugging
					{
						int cnt;
						for(cnt=0;cnt<EEPROM_SIZE;cnt++)
						{
							if((cnt%16)==0)
								Serial.print("\r\n");
							
							Serial.printf("%02x ",EEPROM.read(cnt));
						}
						
						Serial.print("\r\n");
					}
					
					break;
					
		case '?':	Serial.print("NvMemory Test Harness\r\n================\r\n\n");
					Serial.print("x\t-\tDump hex display of eeprom contents\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

void StoreCalibrationData(void)
{	
	int64_t CompassCalAddress=COMPASS_CAL_ADDRESS;
	
	EEPROM.writeFloat(CompassCalAddress,accelmin.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,accelmin.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,accelmin.x);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,accelmax.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,accelmax.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,accelmax.z);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,gyrooffset.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,gyrooffset.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,gyrooffset.z);		CompassCalAddress+=sizeof(float);
	
	EEPROM.commit();
	
	Serial.print("Calibration values stored to NvMemory\r\n");
}

int RetrieveCalibrationData(void)
{
	int64_t CompassCalAddress=COMPASS_CAL_ADDRESS;
	
	float accminx=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accminy=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accminz=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	
	float accmaxx=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accmaxy=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accmaxz=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	
	float gyrox=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float gyroy=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float gyroz=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);

	if(		isnan(accminx)||isnan(accminy)||isnan(accminz)
		||	isnan(accmaxx)||isnan(accmaxy)||isnan(accmaxz)
		||	isnan(gyrox)||isnan(gyroy)||isnan(gyroz)		)
	{
		return(1);
	}
	
	accelmin.x=accminx;		accelmin.y=accminy;		accelmin.z=accminz;
	accelmax.x=accmaxx;		accelmax.y=accmaxy;		accelmax.z=accmaxz;
	gyrooffset.x=gyrox;		gyrooffset.y=gyroy;		gyrooffset.z=gyroz;	
	
	Serial.print("Calibration values retrieved from NvMemory\r\n");

	return(0);
}


