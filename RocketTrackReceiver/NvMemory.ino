
#include <EEPROM.h>
//#include "MPU9250.h"

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

void StoreCompassCalibration_MPU9250(void)
{
#if 0
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
#endif
	
	Serial.print("Compass calibration values stored to NvMemory\r\n");
}

int RetrieveCompassCalibration_MPU9250(void)
{
#if 0
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
#endif
	
	Serial.print("Compass calibration values retrieved from NvMemory\r\n");

	return(0);
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
	
	EEPROM.writeFloat(CompassCalAddress,magoffset.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,magoffset.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,magoffset.z);		CompassCalAddress+=sizeof(float);
	
	EEPROM.writeFloat(CompassCalAddress,magscale.x);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,magscale.y);		CompassCalAddress+=sizeof(float);
	EEPROM.writeFloat(CompassCalAddress,magscale.z);		CompassCalAddress+=sizeof(float);
	
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

	float magoffsetx=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float magoffsety=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float magoffsetz=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	
	float magscalex=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float magscaley=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	float magscalez=EEPROM.readFloat(CompassCalAddress);	CompassCalAddress+=sizeof(float);
	
	if(		isnan(accminx)||isnan(accminy)||isnan(accminz)
		||	isnan(accmaxx)||isnan(accmaxy)||isnan(accmaxz)
		||	isnan(gyrox)||isnan(gyroy)||isnan(gyroz)
		||	isnan(magoffsetx)||isnan(magoffsety)||isnan(magoffsetz)
		||	isnan(magscalex)||isnan(magscaley)||isnan(magscalez)		)
	{
		return(1);
	}
	
	accelmin.x=accminx;		accelmin.y=accminy;		accelmin.z=accminz;
	accelmax.x=accmaxx;		accelmax.y=accmaxy;		accelmax.z=accmaxz;
	gyrooffset.x=gyrox;		gyrooffset.y=gyroy;		gyrooffset.z=gyroz;	
	magoffset.x=magoffsetx;	magoffset.y=magoffsety;	magoffset.z=magoffsetz;
	magscale.x=magscalex;	magscale.y=magscaley;	magscale.z=magscalez;
	
	Serial.print("Calibration values retrieved from NvMemory\r\n");

	mpu6500.setGyrOffsets(gyrooffset);	
	mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);

//	qmc5883l.setCalibrationOffsets(magoffset.x,magoffset.y,magoffset.z);
//	qmc5883l.setCalibrationScales(magscale.x,magscale.y,magscale.z);
	
	Serial.print("Set sensor calibration values\r\n");
	
	return(0);
}

