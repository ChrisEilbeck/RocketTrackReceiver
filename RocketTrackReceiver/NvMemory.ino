
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
	
	EEPROM.writeFloat(CompassCalAddress,AccelOffset.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,AccelOffset.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,AccelOffset.x);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,AccelScale.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,AccelScale.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,AccelScale.z);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,GyroOffset.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,GyroOffset.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,GyroOffset.z);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,Mag_A11);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A12);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A13);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A21);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A22);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A23);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A31);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A32);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_A33);			CompassCalAddress+=sizeof(float);

	EEPROM.writeFloat(CompassCalAddress,Mag_B1);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_B2);			CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,Mag_B3);			CompassCalAddress+=sizeof(float);

	EEPROM.commit();
	
	Serial.print("Calibration values stored to NvMemory\r\n");
}

int RetrieveCalibrationData(void)
{
	int64_t CompassCalAddress=COMPASS_CAL_ADDRESS;
	
	float accoffx=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accoffy=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float accoffz=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	
	float accscalex=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float accscaley=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float accscalez=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	
	float gyrox=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float gyroy=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float gyroz=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);

	float a11=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a12=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a13=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a21=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a22=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a23=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a31=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a32=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float a33=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);

	float b1=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float b2=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float b3=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);

	if(		isnan(accoffx)||isnan(accoffy)||isnan(accoffz)
		||	isnan(accscalex)||isnan(accscaley)||isnan(accscalez)
		||	isnan(gyrox)||isnan(gyroy)||isnan(gyroz)
		||	isnan(a11)||isnan(a12)||isnan(a13)
		||	isnan(a21)||isnan(a22)||isnan(a23)
		||	isnan(a31)||isnan(a32)||isnan(a33)
		||	isnan(b1)||isnan(b2)||isnan(b3)					)
	{
		return(1);
	}
	
	AccelOffset.x=accoffx;		AccelOffset.y=accoffy;		AccelOffset.z=accoffz;
	AccelScale.x=accscalex;		AccelScale.y=accscaley;		AccelScale.z=accscalez;
	GyroOffset.x=gyrox;			GyroOffset.y=gyroy;			GyroOffset.z=gyroz;	
	
	Mag_A11=a11;				Mag_A12=a12;				Mag_A13=a13;
	Mag_A21=a21;				Mag_A22=a22;				Mag_A23=a23;
	Mag_A31=a31;				Mag_A32=a32;				Mag_A33=a33;
	
	Mag_B1=b1;					Mag_B2=b2;					Mag_B3=b3;
	
	Serial.print("Calibration values retrieved from NvMemory\r\n");

	return(0);
}


