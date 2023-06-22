

#include "MPU9250.h"

#if 0
	// no battery
	float AccBiasX=-19.32;		float AccBiasY=39.22;		float AccBiasZ=-67.66;
	float GyroBiasX=-0.23;		float GyroBiasY=2.14;		float GyroBiasZ=-0.14;
	float MagBiasX=292.13;		float MagBiasY=238.12;		float MagBiasZ=-3.48;
	float MagScaleX=0.87;		float MagScaleY=1.52;		float MagScaleZ=0.84;
#else
	// with panasonic 18650
	float AccBiasX=-38.17;		float AccBiasY=47.94;		float AccBiasZ=-65.92;
	float GyroBiasX=-0.27;		float GyroBiasY=2.21;		float GyroBiasZ=-0.20;
	float MagBiasX=474.94;		float MagBiasY=-120.86;		float MagBiasZ=627.97;
	float MagScaleX=1.08;		float MagScaleY=1.18;		float MagScaleZ=0.82;
#endif

float Declination=-1-1/60;

MPU9250 mpu;

int SetupCompass(void)
{
	Serial.print("SetupCompass()\r\n");
	
	int fail=0;
	
#if 0
	if(!mpu.setup(0x68))			{	Serial.println("MPU connection failed at 0x68");	fail=1;	}
	else	if(!mpu.setup(0x69))	{	Serial.println("MPU connection failed at 0x69");	fail=1;	}

	if(fail)
		return(1);
#else
	if(!mpu.setup(0x69))
	{
		Serial.println("MPU connection failed at 0x69");
		return(1);
	}
#endif

	
	Serial.print("\tSetting sensor bias values\r\n");
	
	mpu.setAccBias(AccBiasX,AccBiasY,AccBiasZ);
	mpu.setGyroBias(GyroBiasX,GyroBiasY,GyroBiasZ);
	mpu.setMagBias(MagBiasX,MagBiasY,MagBiasZ);
	mpu.setMagScale(MagScaleX,MagScaleY,MagScaleZ);
	mpu.setMagneticDeclination(Declination);
	
	Serial.print("SetupCompass() complete ...\r\n");
	
	return(0);
}

int CalibrateCompass(void)
{
	Serial.println("Accel Gyro calibration will start in 5sec.");
	Serial.println("Please leave the device still on the flat plane.");
	mpu.verbose(true);
	delay(5000);
	mpu.calibrateAccelGyro();
	
	Serial.println("Mag calibration will start in 5sec.");
	Serial.println("Please Wave device in a figure eight until done.");
	delay(5000);
	mpu.calibrateMag();
	
	print_calibration();
	mpu.verbose(false);
}

void PollCompass(void)
{
	if(mpu.update())
	{
		static uint32_t prev_ms=0;
		if(millis()>(prev_ms+200))
		{
			rx_heading=get_compass_bearing();
			prev_ms=millis();
		}
	}
}

float get_compass_bearing(void)
{
	float heading;
		
	if(mpu.getAccZ()>0.85)
	{
		// flat with the display pointing up
		heading=atan2(mpu.getMagY(),mpu.getMagX())*180/PI;;
		heading=-heading;
		heading+=Declination;
	}
	else if(mpu.getAccZ()<-0.85)
	{
		heading=atan2(mpu.getMagY(),mpu.getMagX())*180/PI;;
		heading+=Declination;
	}
	else if(mpu.getAccX()>0.85)
	{
		heading=atan2(mpu.getMagX(),mpu.getMagZ())*180/PI-90.0f;
		heading+=Declination;
	}
	else if(mpu.getAccX()<-0.85)
	{
		heading=atan2(mpu.getMagX(),mpu.getMagZ())*180/PI-90.0f;
		heading=-heading;
		heading+=Declination;
	}
	else
		heading=0.0f;
	
	while(heading<0)    heading+=360;
	while(heading>360)  heading-=360;
	
	return(heading);
}

void print_calibration()
{
	Serial.println("< calibration parameters >");
	Serial.println("accel bias [g]: ");
	Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.println();
	Serial.println("gyro bias [deg/s]: ");
	Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.println();
	Serial.println("mag bias [mG]: ");
	Serial.print(mpu.getMagBiasX());
	Serial.print(", ");
	Serial.print(mpu.getMagBiasY());
	Serial.print(", ");
	Serial.print(mpu.getMagBiasZ());
	Serial.println();
	Serial.println("mag scale []: ");
	Serial.print(mpu.getMagScaleX());
	Serial.print(", ");
	Serial.print(mpu.getMagScaleY());
	Serial.print(", ");
	Serial.print(mpu.getMagScaleZ());
	Serial.println();
}

