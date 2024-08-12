
bool compass_live_mode=false;

#include "MPU9250.h"

#include "NvMemory.h"

// old values

#if 0
// on antenna boom
float AccBiasX=-68.10;		float AccBiasY=186.83;		float AccBiasZ=-80.80;
float GyroBiasX=-0.15;		float GyroBiasY=2.17;		float GyroBiasZ=-0.23;
float MagBiasX=437.31;		float MagBiasY=324.71;		float MagBiasZ=560.13;
float MagScaleX=1.03;		float MagScaleY=1.28;		float MagScaleZ=0.80;
#endif

// on antenna boom using 3d printed mount, with cable attached but this'll have to do for now
float AccBiasX=-68.17;		float AccBiasY=193.17;		float AccBiasZ=-71.03;
float GyroBiasX=-0.33;		float GyroBiasY=2.38;		float GyroBiasZ=-0.09;
float MagBiasX=501.83;		float MagBiasY=371.61;		float MagBiasZ=633.19;
float MagScaleX=1.04;		float MagScaleY=1.21;		float MagScaleZ=0.82;

float Declination=-1-1/60;

MPU9250 mpu9250;

bool use_compass=true;

byte CompassAddresses[]={0x68,0x69,0x68,0x69,0x68,0x69,0x68,0x69,0x68,0x69,0x00};

int SetupCompass(void)
{
	Serial.print("SetupCompass()\r\n");
	
	int fail=0;
	int cnt=0;
	
	for(cnt=0;cnt<(sizeof(CompassAddresses)/sizeof(byte));cnt++)
	{
		if(CompassAddresses[cnt]==0x00)
		{
			fail=1;
		}
		else
		{
			Serial.printf("\tTrying 0x%02x ... ",CompassAddresses[cnt]);
			
			if(mpu9250.setup(CompassAddresses[cnt]))	{	Serial.println("OK");	fail=0;		break;	}
			else									{	Serial.println("fail");	fail=1;				}
			
			delay(250);
		}
	}
	
	if(fail)
	{
		Serial.println("MPU connection failed");
		use_compass=false;
		return(1);
	}
	
#if 0
	CalibrateCompass();
	while(1);
#endif
	
	Serial.print("\tSetting sensor bias values\r\n");
	SetSensorBiasValues();
	Serial.print("SetupCompass() complete ...\r\n");
	
	return(0);
}

void PollCompass(void)
{
	if(!use_compass)
		return;

	if(mpu9250.update())
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
			Serial.printf("Heading: %.1f, Pitch: %.1f, Roll: %.1f, Yaw: %.1f\r\n",get_compass_bearing(),mpu9250.getPitch(),mpu9250.getRoll(),mpu9250.getYaw());
			updateat=millis()+200;
		}
	}
}

float get_compass_bearing(void)
{
	float heading=0.0f;

	static float mag_x_dampened=0.0f;
	static float mag_y_dampened=0.0f;

	// might need to remap these because it only seems to really work
	// properly when the LoRa antenna is pointing down or up

	float mag_pitch=mpu9250.getPitch();
	float mag_roll=mpu9250.getRoll();
	
	float mag_x=0.0;
	float mag_y=0.0;
	float mag_z=0.0;

#if 0
	if(			(mag_roll<-135.0)||(mag_roll>=135.0)	)
	{
	
	}
	else if(	(mag_roll>=-135.0)&&(mag_roll<-45.0)	)
	{
		mag_pitch=-mpu9250.getRoll()*DEG_TO_RAD;
		mag_roll=mpu9250.getPitch()*DEG_TO_RAD;
		mag_x=mpu9250.getMagX();
		mag_y=mpu9250.getMagY();
		mag_z=mpu9250.getMagZ();	
	}
	else if(	(mag_roll>=-45.0)&&(mag_roll<45.0)		)
	{
	
	}
	else if(	(mag_roll>=45.0)&&(mag_roll<135.0)		)
	{
		mag_pitch=mpu9250.getRoll()*DEG_TO_RAD;
		mag_roll=mpu9250.getPitch()*DEG_TO_RAD;
		mag_x=mpu9250.getMagX();
		mag_y=mpu9250.getMagY();
		mag_z=mpu9250.getMagZ();
	}
	else
	{
		// should never happen
		
		Serial.printf("Bad mag_roll reading in get_compass_bearing() - %f\r\n",mag_roll);
	}



	mag_roll*=DEG_TO_RAD;
	mag_pitch*=DEG_TO_RAD;



#else
	mag_pitch=-mpu9250.getRoll()*DEG_TO_RAD;
	mag_roll=mpu9250.getPitch()*DEG_TO_RAD;
	
	mag_x=mpu9250.getMagX();
	mag_y=mpu9250.getMagY();
	mag_z=mpu9250.getMagZ();
#endif
		
	// ----- Apply the standard tilt formulas
	float mag_x_hor=mag_x*cos(mag_pitch)+mag_y*sin(mag_roll)*sin(mag_pitch)-mag_z*cos(mag_roll)*sin(mag_pitch);
	float mag_y_hor=mag_y*cos(mag_roll)+mag_z*sin(mag_roll);
	
	// ----- Dampen any data fluctuations
	mag_x_dampened=mag_x_dampened*0.9+mag_x_hor*0.1;
	mag_y_dampened=mag_y_dampened*0.9+mag_y_hor*0.1;
	
	// ----- Calculate the heading
	heading=atan2(mag_x_dampened,mag_y_dampened)*RAD_TO_DEG;	// Magnetic North
	
	heading+=Declination;

#if 0
	heading=mpu9250.getYaw();
#endif
	
	// constrain to 0 to <360 degrees
	while(heading<0)    heading+=360;
	while(heading>360)  heading-=360;
	
	return(heading);
}

void print_calibration()
{
	Serial.println("< calibration parameters >");
	Serial.println("accel bias [g]: ");
	Serial.print(mpu9250.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu9250.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu9250.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
	Serial.println();
	Serial.println("gyro bias [deg/s]: ");
	Serial.print(mpu9250.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu9250.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.print(", ");
	Serial.print(mpu9250.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
	Serial.println();
	Serial.println("mag bias [mG]: ");
	Serial.print(mpu9250.getMagBiasX());
	Serial.print(", ");
	Serial.print(mpu9250.getMagBiasY());
	Serial.print(", ");
	Serial.print(mpu9250.getMagBiasZ());
	Serial.println();
	Serial.println("mag scale []: ");
	Serial.print(mpu9250.getMagScaleX());
	Serial.print(", ");
	Serial.print(mpu9250.getMagScaleY());
	Serial.print(", ");
	Serial.print(mpu9250.getMagScaleZ());
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
					
					mpu9250.verbose(true);
					delay(2000);
					mpu9250.calibrateAccelGyro();
					mpu9250.verbose(false);
					
					break;
		
		case 'm':	// calibrate the magnetometer
					Serial.println("Mag calibration will start in 2sec.");
					Serial.println("Please Wave device in a figure eight until done.");
					
					mpu9250.verbose(true);
					delay(2000);
					mpu9250.calibrateMag();
					mpu9250.verbose(false);
					
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
					Serial.printf("Pitch: %.1f\r\n",mpu9250.getPitch());
					break;
		
		case 'r':	// roll
					Serial.printf("Roll: %.1f\r\n",mpu9250.getRoll());
					break;
		
		case 'y':	// yaw
					Serial.printf("Yaw: %.1f\r\n",mpu9250.getYaw());
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

void SetSensorBiasValues(void)
{
	mpu9250.setAccBias(AccBiasX,AccBiasY,AccBiasZ);
	mpu9250.setGyroBias(GyroBiasX,GyroBiasY,GyroBiasZ);
	mpu9250.setMagBias(MagBiasX,MagBiasY,MagBiasZ);
	mpu9250.setMagScale(MagScaleX,MagScaleY,MagScaleZ);
	mpu9250.setMagneticDeclination(Declination);
}

void ResetCompassCalibration(void)
{
	// with panasonic 18650
	AccBiasX=-76.10;	AccBiasY=47.33;		AccBiasZ=-64.94;
	GyroBiasX=-0.29;	GyroBiasY=2.56;		GyroBiasZ=-0.45;
	MagBiasX=492.87;	MagBiasY=212.87;	MagBiasZ=581.00;
	MagScaleX=0.94;		MagScaleY=1.49;		MagScaleZ=0.79;
}

void CalibrateCompass(void)
{
	Serial.println("Starting accelerometer calibration");
	
	Serial.println("Mag calibration will start in 2sec.");
	Serial.println("Please Wave device in a figure eight until done.");

	mpu9250.verbose(true);
	delay(2000);
	mpu9250.calibrateMag();
	mpu9250.verbose(false);
	
	print_calibration();
	
	Serial.println("Magnetometer calibration complete");
}

void CalibrateAccelGyro(void)
{
	Serial.println("Starting accelerometer calibration");
	
	Serial.println("\tAccel Gyro calibration will start in 2sec.");
	Serial.println("\tPlease leave the device still on the flat plane.");

	mpu9250.verbose(true);
	delay(2000);
	mpu9250.calibrateAccelGyro();
	mpu9250.verbose(false);
	
	print_calibration();
	
	Serial.println("Accelerometer calibration complete");
}

