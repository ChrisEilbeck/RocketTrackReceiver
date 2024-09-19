
//
// this shoud automatically detect between a combination of an
// MPU6500/QMC5883/BME180, or a ICM20948 with a BME180, or a MPU9250 with a
// BME180
//

#include "IMU.h"
#include "NvMemory.h"

#include <MadgwickAHRS.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified hmc5883l=Adafruit_HMC5883_Unified(12345);

//#include "MPU9250.h"
//#include "ICM20948.h"
//#include "BME180.h"

#include "MPU9250_WE.h"
//#include <QMC5883LCompass.h>
#include <MechaQMC5883.h>

MechaQMC5883 qmc5883l;

float Declination=-1-1/60;

xyzFloat accelmin;
xyzFloat accelmax;
xyzFloat gyrooffset;
xyzFloat magoffset;
xyzFloat magscale;

// hardware devices

//MPU9250 mpu9250;

MPU6500_WE mpu6500;
//QMC5883LCompass qmc5883l;

//ICM20948 icm20948;

//BME180 bme180;

// for sensor fusion

Madgwick filter;

bool use_compass=true;
bool compass_live_mode=true;
int sensor_setup=NO_SENSORS;

float roll=0.0;
float pitch=0.0;
float yaw=0.0;

byte MPU9250Addresses[]={0x68,0x69,0x68,0x69,0x68,0x69,0x68,0x69,0x68,0x69,0x00};

float imu_rate=10.0;	// Hz

void CalibrateMPU6500Accelerometer(const char *orientation,xyzFloat *vals);
void CalibrateMPU6500Gyro(void);

void ResetCalibration(void);

int SetupIMU(void)
{
	Serial.print("SetupIMU()\r\n");
	
	int fail=1;
	int cnt=0;

	if(fail)	fail=DetectSeparateBoards();
	if(fail)	fail=DetectCombinedBoard();
	if(fail)	fail=DetectMPU9250();
	
	if(fail)
	{
		Serial.println("MPU connection failed");
		use_compass=false;
		return(1);
	}
	
	filter.begin(imu_rate);
	
	Serial.print("SetupIMU() complete ...\r\n");
	
	return(0);
}

int DetectMPU9250(void)
{
	int cnt;
	int fail=0;
	
	for(cnt=0;cnt<(sizeof(MPU9250Addresses)/sizeof(byte));cnt++)
	{
		if(MPU9250Addresses[cnt]==0x00)
		{
			fail=1;
		}
		else
		{
			Serial.printf("\tTrying 0x%02x ... ",MPU9250Addresses[cnt]);
			
//			if(mpu9250.setup(MPU9250Addresses[cnt]))	{	Serial.println("OK");	fail=0;		break;	}
//			else										{	Serial.println("fail");	fail=1;				}
			
			delay(250);
		}
	}
	
	return(1);
}

void displaySensorDetails(void)
{
	sensor_t sensor;
	hmc5883l.getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print("Sensor:\t\t"); Serial.println(sensor.name);
	Serial.print("Driver Ver:\t"); Serial.println(sensor.version);
	Serial.print("Unique ID:\t"); Serial.println(sensor.sensor_id);
	Serial.print("Max Value:\t"); Serial.print(sensor.max_value); Serial.println(" uT");
	Serial.print("Min Value:\t"); Serial.print(sensor.min_value); Serial.println(" uT");
	Serial.print("Resolution:\t"); Serial.print(sensor.resolution); Serial.println(" uT");
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
} 

int DetectSeparateBoards(void)
{
	int fail=0;
	
	Serial.println("Setting up individual motion sensor boards ...");

	if(!mpu6500.init())
	{
		Serial.println("MPU6500 setup failed");
		fail=1;
	}
	else
		Serial.println("\tMPU6500 gyro/mag initialised ...");

#if 1
	if(!hmc5883l.begin())
	{
		/* There was a problem detecting the HMC5883 ... check your connections */
		Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
		while(1);
	} 
	
	/* Display some basic information on this sensor */
	displaySensorDetails();
#endif
#if 0
	qmc5883l.init();
	qmc5883l.setMode(0x01,0x00,0x00,0xc0);
//	qmc5883l.setCalibrationOffsets(-7.00, 7.00, 0.00);
//	qmc5883l.setCalibrationScales(0.53, 2.21, 1.51);

	qmc5883l.setMode(Mode_Continuous,ODR_50Hz,RNG_2G,OSR_64);
#endif
#if 0
	// setup the qmc without using a library

	Wire.beginTransmission(0x0d);
	Wire.write(0x0a);
	Wire.write(0x80);
	Wire.endTransmission();

	delay(1);

	Wire.beginTransmission(0x0d);
	Wire.write(0x0a);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.beginTransmission(0x0d);
	Wire.write(0x0b);
	Wire.write(0x01);
	Wire.endTransmission();
	
	Wire.beginTransmission(0x0d);
	Wire.write(0x09);
	Wire.write(0x0d);	// 50Hz, 64x oversampling, +/- 2 Gauss continuous
	Wire.endTransmission();
#endif
	
	Serial.println("\tQMC5883L magnetometer initialised ...");

	return(fail);
}

int DetectCombinedBoard(void)
{


	return(1);
}

void PollIMU(void)
{
	if(!use_compass)
		return;

	static int update_filter_at=0;
	
	xyzFloat mag;
	xyzFloat accel;
	xyzFloat gyro;
	
	static xyzFloat magmin={1000.0,1000.0,1000.0};
	static xyzFloat magmax={-1000.0,-1000.0,-1000.0};
	
	if(millis()>update_filter_at)
	{
		update_filter_at=millis()+(int)(1000/imu_rate);
	
#if 1
	#if 1
		#if 1
			sensors_event_t event; 
			hmc5883l.getEvent(&event);

			mag.x=event.magnetic.x;
			mag.y=event.magnetic.y;
			mag.z=event.magnetic.z;
			
			if(mag.x<magmin.x)	magmin.x=mag.x;
			if(mag.x>magmax.x)	magmax.x=mag.x;
			if(mag.y<magmin.y)	magmin.y=mag.y;
			if(mag.y>magmax.y)	magmax.y=mag.y;
			if(mag.z<magmin.z)	magmin.z=mag.z;
			if(mag.z>magmax.z)	magmax.z=mag.z;

//			mag.x+=6.5;
//			mag.y+=16.0;
			
		#endif
		#if 0
			qmc5883l.read();
			// remap the axes to match the mpu6500 accel/gyro on the fake gy-91 board
			mag.x=(float)qmc5883l.getY();
			mag.y=(float)-qmc5883l.getX();
			mag.z=(float)qmc5883l.getZ();		
		#endif
		#if 0
			uint16_t x;
			uint16_t y;
			uint16_t z;
		
			qmc5883l.read(&x,&y,&z);
			
			mag.x=(float)x;		mag.y=(float)y;		mag.z=(float)z;
			
			if(mag.x>32768.0)	mag.x-=65536.0;
			if(mag.y>32768.0)	mag.y-=65536.0;
			if(mag.z>32768.0)	mag.z-=65536.0;
		#endif
	#else
		qmc5883l.read();
		mag.x=(float)qmc5883l.getX();
 		mag.y=(float)qmc5883l.getY();
		mag.z=(float)qmc5883l.getZ();
	#endif
#endif
		// these are values scaled to proper unit with offsets applied from
		// the calibration data
		accel=mpu6500.getGValues();
		gyro=mpu6500.getGyrValues();
		
		filter.update(
						gyro.x,gyro.y,gyro.z,
						accel.x,accel.y,accel.z,
						mag.x,mag.y,mag.z
					);
#if 0
		rx_heading=180.0-filter.getYaw();
#endif

		rx_heading=atan2(mag.y,mag.x);
		rx_heading*=180.0/PI;
		
//		rx_heading=90.0-rx_heading+120.0;
		
		if(rx_heading<0.0)		rx_heading+=360.0;
		if(rx_heading>360.0)	rx_heading-=360.0;
	}

	if(compass_live_mode)
	{
		static int update_ui_at=0;
		
		if(millis()>update_ui_at)
		{
			update_ui_at=millis()+200;

#if 0
			Serial.printf("AccX: % .2f, AccY: % .2f, AccZ: % .2f\t",accel.x,accel.y,accel.z);
			Serial.printf("GyroX: % .2f, GyroY: % .2f, GyroZ: % .2f\t",gyro.x,gyro.y,gyro.z);
#endif
#if 1
			Serial.printf("MagX: % .2f, MagY: % .2f, MagZ: % .2f, AccX: %.2f, AccY: %.2f, AccZ: %.2f, GyroX: %.2f, GyroY: %.2f, GyroZ: %.2f\r\n",
								mag.x,mag.y,mag.z,
								accel.x,accel.y,accel.z,
								gyro.x,gyro.y,gyro.z
						);
#endif
#if 0
			Serial.printf("MagX: % .2f (%.2f to %.2f), MagY: % .2f (%.2f to %.2f), MagZ: % .2f (%.2f to %.2f), Heading: %.2f\r\n",
				mag.x,magmin.x,magmax.x,
				mag.y,magmin.y,magmax.y,
				mag.z,magmin.z,magmax.z,
				rx_heading);
#endif
#if 0
			Serial.printf("MagX: % .2f, MagY: % .2f, MagZ: % .2f\t",qmc5883l.getX(),qmc5883l.getY(),qmc5883l.getZ());
#endif
#if 0
			Serial.print("MagX: ");
			Serial.print(qmc5883l.getX());
			Serial.print("\tMagY: ");
			Serial.print(qmc5883l.getY());
			Serial.print("\tMagZ: ");
			Serial.print(qmc5883l.getZ());
			Serial.print("\r\n");
#endif
#if 0			
			Serial.printf(
							"Roll: %.1f, Pitch: %.1f, Yaw: %.1f, Heading: %.1f\r\n",
							filter.getRoll(),
							filter.getPitch(),
							filter.getYaw(),
							rx_heading
						);
#endif
#if 0		
			Wire.beginTransmission(0x0d);
			Wire.write(0x00);
			Wire.requestFrom(0x0d,6);
			byte x_lsb=Wire.read();
			byte x_msb=Wire.read();
			byte y_lsb=Wire.read();
			byte y_msb=Wire.read();
			byte z_lsb=Wire.read();
			byte z_msb=Wire.read();
			Wire.endTransmission();

			int xmag=(int)(x_msb<<8|x_lsb);	if(xmag>32768)	xmag-=65536;
			int ymag=(int)(y_msb<<8|y_lsb);	if(ymag>32768)	ymag-=65536;
			int zmag=(int)(z_msb<<8|z_lsb);	if(zmag>32768)	zmag-=65536;

			Serial.printf("Xmag: % 05d, Ymag: % 05d, Zmag: % 05d\r\n",xmag,ymag,zmag);
#endif
		}
	}
}

float get_compass_bearing(void)
{
	float heading=0.0f;

#if 0
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
#endif
		
	return(heading);
}

int IMUCommandHandler(uint8_t *cmd,uint16_t cmdptr)
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
		case 'h':	// compass heading
					Serial.printf("Heading: %.1f\r\n",get_compass_bearing());
					break;
		
		case 'p':	// pitch
//					Serial.printf("Pitch: %.1f\r\n",mpu9250.getPitch());
					break;
		
		case 'r':	// roll
//					Serial.printf("Roll: %.1f\r\n",mpu9250.getRoll());
					break;
		
		case 'y':	// yaw
//					Serial.printf("Yaw: %.1f\r\n",mpu9250.getYaw());
					break;
		
		case 'l':	// live mode toggle
					compass_live_mode=!compass_live_mode;
					
					if(compass_live_mode)	Serial.print("Enabling live compass data mode\r\n");
					else					Serial.print("Disabling live compass data mode\r\n");
					
					break;
		
		case '?':	Serial.print("IMU Test Harness\r\n================\r\n\n");
					Serial.print("\th\t-\tDisplay the compass heading\r\n");
					Serial.print("\tl\t-\tEnable/disable live compass data display\r\n");
					Serial.print("\tp\t-\tRead current pitch value\r\n");
					Serial.print("\tr\t-\tRead current roll value\r\n");
					Serial.print("\ty\t-\tRead current yaw value\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

int SensorCalibrationCommandHandler(uint8_t *cmd,uint16_t cmdptr)
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
		case 'c':	// clear all calibration
//					qmc5883l.clearCalibration();
					ResetCalibration();
					StoreCalibrationData();					
					break;
		
		case 'g':	// calibrate the gyro
					CalibrateMPU6500Gyro();
					mpu6500.setGyrOffsets(gyrooffset);
					
					break;
		
		case 'm':	// calibrate the magnetometer
					CalibrateQMC5883LMagnetometer();
					break;
		
		case 'p':	// print valibration values
					PrintCalibrationValues();
					break;
		
		case 's':	// store the calibration values
					StoreCalibrationData();
					break;
		
		case 't':	// retrieve the calibration values
					RetrieveCalibrationData();
					break;					

		case 'x':	if(cmd[2]=='-')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Nose down",&vals);		accelmin.x=vals.x;	}
					if(cmd[2]=='+')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Nose up",&vals);			accelmax.x=vals.x;	}
							
					mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);

					break;
		
		case 'y':	if(cmd[2]=='-')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Left down",&vals);		accelmin.y=vals.y;	}
					if(cmd[2]=='+')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Right down",&vals);		accelmax.y=vals.y;	}
					
					mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
					
					break;
		
		case 'z':	if(cmd[2]=='+')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Flat and level",&vals);	accelmax.z=vals.z;	}
					if(cmd[2]=='-')	{	xyzFloat vals;	CalibrateMPU6500Accelerometer("Upside-down",&vals);		accelmin.z=vals.z;	}
					
					mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
					
					break;
		
		case '?':	Serial.print("IMU Test Harness\r\n================\r\n\n");
					Serial.print("g\t-\tCalibrate the Gyro\r\n");
					Serial.print("m\t-\tCalibrate the Magnetometer\r\n");
					Serial.print("p\t-\tPrint calibration values\r\n");
					Serial.print("s\t-\tStore the calibration values\r\n");
					Serial.print("t\t-\tRetrieve the calibration values\r\n");
					Serial.print("x-\t-\tCalibrate X- accelerometer reading\r\n");
					Serial.print("x+\t-\tCalibrate X+ accelerometer reading\r\n");
					Serial.print("y-\t-\tCalibrate Y- accelerometer reading\r\n");
					Serial.print("y+\t-\tCalibrate Y+ accelerometer reading\r\n");
					Serial.print("z-\t-\tCalibrate Z- accelerometer reading\r\n");
					Serial.print("z+\t-\tCalibrate Z+ accelerometer reading\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

void ResetCompassCalibration(void)
{
}

#if 0
void StoreCalibrationData(void)
{
	switch(sensor_setup)
	{
		case  NO_SENSORS:						
												
												break;
		
		case SENSOR_MPU6500_QMC5883L_BME180:	
												
												break;
		
		case SENSOR_ICM_20948_BME180:			
												
												break;
		
		case SENSOR_MPU9250_BME180:				
												
												break;
		
		default:								
												
												break;
	}
}

void RetrieveCalibrationData(void)
{
	switch(sensor_setup)
	{
		case NO_SENSORS:						
												
												break;
		
		case SENSOR_MPU6500_QMC5883L_BME180:	
												
												break;
		
		case SENSOR_ICM_20948_BME180:			
												
												break;
		
		case SENSOR_MPU9250_BME180:				
												
												break;
		
		default:								
												
												break;
	}
}
#endif

void CalibrateMPU6500Accelerometer(const char *orientation,xyzFloat *vals)
{
	Serial.println("CalibrateMPU6500Accelerometer() entry");
	
	Serial.print("\tPlace with ");
	Serial.print(orientation);
	Serial.print("\t");
	
	Serial.print("3 ...");	delay(1000);
	Serial.print("2 ...");	delay(1000);
	Serial.print("1 ...\r\n");	delay(1000);
	Serial.println("\tMeasuring ...");
	
	int now=millis();
	xyzFloat sum;
	int cnt=0;
		
	while((millis()-now)<1000)
	{
		sum+=mpu6500.getAccRawValues();
		cnt++;
		delay(10);
	}	
	
	sum/=(float)cnt;
	
	Serial.printf("\tMeasured X: %.1f, Y: %.1f, Z: %.1f\r\n",sum.x,sum.y,sum.z);	
	
	vals->x=sum.x;	vals->y=sum.y;	vals->z=sum.z;
	
	Serial.println("CalibrateMPU6500Accelerometer() exit");
}

void CalibrateMPU6500Gyro(void)
{
	Serial.println("CalibrateMPU6500Gyro() entry");
	Serial.println("\tPlace the unit on a flat surface and do not move it for 3 seconds ...");
	
	delay(1000);
	
	Serial.println("\tMeasuring ...");
	
	int now=millis();
	xyzFloat sum;
	int cnt=0;
	
	while((millis()-now)<3000)
	{
		sum+=mpu6500.getGyrRawValues();
		cnt++;
		delay(10);
	}
	
	sum/=(float)cnt;
	
	Serial.printf("\tMeasured X: %.1f, Y: %.1f, Z: %.1f\r\n",sum.x,sum.y,sum.z);	
	
	gyrooffset.x=sum.x;	gyrooffset.y=sum.y;	gyrooffset.z=sum.z;
		
	Serial.println("CalibrateMPU6500Gyro() exit");
}

void CalibrateQMC5883LMagnetometer(void)
{
	Serial.println("CalibrateQMC5883LMagnetometer() entry");

	Serial.println("\tPlease rotate about all axes for 10 seconds");
	
//	qmc5883l.calibrate();

//	magoffset.x=qmc5883l.getCalibrationOffset(0);	magoffset.y=qmc5883l.getCalibrationOffset(1);	magoffset.z=qmc5883l.getCalibrationOffset(2);
//	magscale.x=qmc5883l.getCalibrationOffset(0);	magscale.y=qmc5883l.getCalibrationOffset(1);	magscale.z=qmc5883l.getCalibrationOffset(2);
	
	Serial.println("\tMagnetometer calibration done ...");
	
	Serial.println("CalibrateQMC5883LMagnetometer() exit");
}

void PrintCalibrationValues(void)
{
	Serial.print("Accelerometer:\r\n");
	Serial.printf("\tX: %.1f\tto\t%.1f\r\n",accelmin.x,accelmax.x);
	Serial.printf("\tY: %.1f\tto\t %.1f\r\n",accelmin.y,accelmax.y);
	Serial.printf("\tZ: %.1f\tto\t%.1f\r\n\n",accelmin.z,accelmax.z);
	
	Serial.print("Gyroscope:\r\n");
	Serial.printf("\tX: %.1f\r\n",gyrooffset.x);
	Serial.printf("\tY: %.1f\r\n",gyrooffset.y);
	Serial.printf("\tZ: %.1f\r\n\n",gyrooffset.z);
	
	Serial.print("Magnetometer:\r\n");
	Serial.printf("\tX offset: %.1f\tX scale: %.1f\r\n",magoffset.x,magscale.x);
	Serial.printf("\tY offset: %.1f\tY scale: %.1f\r\n",magoffset.y,magscale.y);
	Serial.printf("\tZ offset: %.1f\tZ scale: %.1f\r\n\n",magoffset.z,magscale.z);
}

void ResetCalibration(void)
{
	magscale.x=1.0;		magscale.y=1.0;		magscale.z=1.0;
	magoffset.x=0.0;	magoffset.y=0.0;	magoffset.z=0.0;

	accelmin.x=-16384.0;	accelmax.x=16384.0;
	accelmin.y=-16384.0;	accelmax.y=16384.0;
	accelmin.z=-16384.0;	accelmax.z=16384.0;
	
	gyrooffset.x=0.0;	gyrooffset.y=0.0;	gyrooffset.z=0.0;					
}

