
bool compass_live_mode=false;

#include "MPU9250.h"

#include "NvMemory.h"

#if 0
	// no battery
	float AccBiasX=-19.32;		float AccBiasY=39.22;		float AccBiasZ=-67.66;
	float GyroBiasX=-0.23;		float GyroBiasY=2.14;		float GyroBiasZ=-0.14;
	float MagBiasX=292.13;		float MagBiasY=238.12;		float MagBiasZ=-3.48;
	float MagScaleX=0.87;		float MagScaleY=1.52;		float MagScaleZ=0.84;
#endif
#if 0
	// with panasonic 18650
	float AccBiasX=-76.10;		float AccBiasY=47.33;		float AccBiasZ=-64.94;
	float GyroBiasX=-0.29;		float GyroBiasY=2.56;		float GyroBiasZ=-0.45;
	float MagBiasX=492.87;		float MagBiasY=212.87;		float MagBiasZ=581.00;
	float MagScaleX=0.94;		float MagScaleY=1.49;		float MagScaleZ=0.79;
#endif
#if 1
	// on antenna boom
	float AccBiasX=-68.10;		float AccBiasY=186.83;		float AccBiasZ=-80.80;
	float GyroBiasX=-0.15;		float GyroBiasY=2.17;		float GyroBiasZ=-0.23;
	float MagBiasX=437.31;		float MagBiasY=324.71;		float MagBiasZ=560.13;
	float MagScaleX=1.03;		float MagScaleY=1.28;		float MagScaleZ=0.80;
#endif

#if 0
	< calibration parameters >
	accel bias [g]: 
	-68.10, 186.83, -80.80
	gyro bias [deg/s]: 
	-0.15, 2.17, -0.23
	mag bias [mG]: 
	437.31, 324.71, 560.13
	mag scale []: 
	1.03, 1.28, 0.80
#endif

//float Declination=-1-1/60;
float Declination=0.0f;

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
	
#if 0
	CalibrateCompass();
	while(1);
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

void CalibrateCompass(void)
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

	if(compass_live_mode)
	{
		static int updateat=0;
		
		if(millis()>updateat)
		{
			Serial.printf("Heading: %.1f, Pitch: %.1f, Roll: %.1f, Yaw: %.1f\r\n",get_compass_bearing(),mpu.getPitch(),mpu.getRoll(),mpu.getYaw());
			updateat=millis()+200;
		}
	}
}

float get_compass_bearing(void)
{
	float heading=0.0f;

#if 0
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
#endif
#if 1
	static float mag_x_dampened=0.0f;
	static float mag_y_dampened=0.0f;

	float mag_pitch=-mpu.getRoll()*DEG_TO_RAD;
	float mag_roll=mpu.getPitch()*DEG_TO_RAD;
	
	float mag_x=mpu.getMagX();
	float mag_y=mpu.getMagY();
	float mag_z=mpu.getMagZ();
	
	// ----- Apply the standard tilt formulas
	float mag_x_hor=mag_x*cos(mag_pitch)+mag_y*sin(mag_roll)*sin(mag_pitch)-mag_z*cos(mag_roll)*sin(mag_pitch);
	float mag_y_hor=mag_y*cos(mag_roll)+mag_z*sin(mag_roll);
	
	// ----- Dampen any data fluctuations
	mag_x_dampened=mag_x_dampened*0.9+mag_x_hor*0.1;
	mag_y_dampened=mag_y_dampened*0.9+mag_y_hor*0.1;
	
	// ----- Calculate the heading
	heading=atan2(mag_x_dampened,mag_y_dampened)*RAD_TO_DEG;	// Magnetic North
	
	heading+=Declination;
#endif
#if 0
	heading=mpu.getYaw();
#endif
	
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

int CompassCommandHandler(uint8_t *cmd,uint16_t cmdptr)
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
		case 'a':	// calibrate the gyro and accelerometer
					Serial.println("Accel Gyro calibration will start in 2sec.");
					Serial.println("Please leave the device still on the flat plane.");
					
					mpu.verbose(true);
					delay(2000);
					mpu.calibrateAccelGyro();
					mpu.verbose(false);
					
					break;
		
		case 'm':	// calibrate the magnetometer
					Serial.println("Mag calibration will start in 2sec.");
					Serial.println("Please Wave device in a figure eight until done.");
					
					mpu.verbose(true);
					delay(2000);
					mpu.calibrateMag();
					mpu.verbose(false);
					
					print_calibration();
					
					break;
		
		case 's':	// store the calibration values
					StoreCompassCalibration();
					break;
		
		case 't':	// retrieve the calibration values
					RetrieveCompassCalibration();
					break;					
		
		case 'h':	// compass heading
					Serial.printf("Heading: %.1f\r\n",get_compass_bearing());
					break;
		
		case 'p':	// pitch
					Serial.printf("Pitch: %.1f\r\n",mpu.getPitch());
					break;
		
		case 'r':	// roll
					Serial.printf("Roll: %.1f\r\n",mpu.getRoll());
					break;
		
		case 'y':	// yaw
					Serial.printf("Yaw: %.1f\r\n",mpu.getYaw());
					break;
		
		case 'l':	// live mode toggle
					compass_live_mode=!compass_live_mode;
					
					if(compass_live_mode)	Serial.print("Enabling live compass data mode\r\n");
					else					Serial.print("Disabling live compass data mode\r\n");
					
					break;
		
		case '?':	Serial.print("Compass Test Harness\r\n================\r\n\n");
					Serial.print("\ta\t-\tCalibrate the accelerometer\r\n");
					Serial.print("\th\t-\tDisplay the compass heading\r\n");
					Serial.print("\tl\t-\tEnable/disable live compass data display\r\n");
					Serial.print("\tm\t-\tCalibrate the magnetometer\r\n");
					Serial.print("\tp\t-\tRead current pitch value\r\n");
					Serial.print("\tr\t-\tRead current roll value\r\n");
					Serial.print("\ts\t-\tStore the calibration values\r\n");
					Serial.print("\tt\t-\tRetrieve the calibration values\r\n");
					Serial.print("\ty\t-\tRead current yaw value\r\n");
					Serial.print("\th\t-\tRead current heading value\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}
