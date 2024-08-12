
#include <EEPROM.h>
#include "MPU9250.h"

#include "NvMemory.h"

int SetupNvMemory(void)
{
	EEPROM.begin(EEPROM_SIZE);
}

void PollNvMemory(void)
{

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
						for(cnt=0;cnt<32;cnt++)
						{
							if(cnt==16)	Serial.print("\r\n");
							
							Serial.printf("%02x ",EEPROM.read(cnt));
						}
						
						Serial.print("\r\n");
					}
					
					break;
					
		case '?':	Serial.print("NvMemory Test Harness\r\n================\r\n\n");
					Serial.print("\tx\t-\tDump hex display of eeprom contents\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

void StoreCompassCalibration(void)
{
	int64_t CompassCalAddress=COMPASS_CAL_ADDRESS;
	
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getAccBiasX());		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getAccBiasY());		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getAccBiasZ());		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getGyroBiasX());	CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getGyroBiasY());	CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getGyroBiasZ());	CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagBiasX());		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagBiasY());		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagBiasZ());		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagScaleX());	CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagScaleY());	CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,mpu9250.getMagScaleZ());	CompassCalAddress+=sizeof(float);
	
	Serial.print("Compass calibration values stored to NvMemory\r\n");
}

int RetrieveCompassCalibration(void)
{
	int64_t CompassCalAddress=COMPASS_CAL_ADDRESS;
	
	float AccBiasX=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float AccBiasY=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float AccBiasZ=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	
	float GyroBiasX=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float GyroBiasY=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float GyroBiasZ=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	
	float MagBiasX=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float MagBiasY=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	float MagBiasZ=EEPROM.readFloat(CompassCalAddress);			CompassCalAddress+=sizeof(float);
	
	float MagScaleX=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float MagScaleY=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	float MagScaleZ=EEPROM.readFloat(CompassCalAddress);		CompassCalAddress+=sizeof(float);
	
	if(		isnan(AccBiasX)||isnan(AccBiasY)||isnan(AccBiasZ)
		||	isnan(GyroBiasX)||isnan(GyroBiasY)||isnan(GyroBiasZ)
		||	isnan(MagBiasX)||isnan(MagBiasY)||isnan(MagBiasZ)
		||	isnan(MagScaleX)||isnan(MagScaleY)||isnan(MagScaleZ)		)
	{
		return(1);
	}

	mpu9250.setAccBias(AccBiasX,AccBiasY,AccBiasZ);
	mpu9250.setGyroBias(GyroBiasX,GyroBiasY,GyroBiasZ);
	mpu9250.setMagBias(MagBiasX,MagBiasY,MagBiasZ);
	mpu9250.setMagScale(MagScaleX,MagScaleY,MagScaleZ);
	
	Serial.print("Compass calibration values retrieved from NvMemory\r\n");

	return(0);
}

