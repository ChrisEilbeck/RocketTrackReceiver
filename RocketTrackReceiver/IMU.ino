
//
// this shoud automatically detect between a combination of an
// MPU6500/QMC5883/BME180, or a ICM20948 with a BME180, or a MPU9250 with a
// BME180
//

#include "IMU.h"
#include "NvMemory.h"
#include "Webserver.h"

// for sensor fusion
#include <MadgwickAHRS.h>

// also supports MPU6500 on GY-91 boards these days
#include "MPU9250_WE.h"

// for the HMC magnetometer
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// for future use of a combined sensor board
//#include "ICM20948.h"

// IMU devices

MPU6500_WE mpu6500;
Adafruit_HMC5883_Unified hmc5883l=Adafruit_HMC5883_Unified(12345);

// typical declination value for the south of the UK

float Declination=-1-1/60;

// for calibration of the sensors

xyzFloat accelmin;
xyzFloat accelmax;
xyzFloat gyrooffset;
//xyzFloat magoffset;
//xyzFloat magscale;

// soft iron correction matrix for magnetometer
float Mag_A11,Mag_A12,Mag_A13;
float Mag_A21,Mag_A22,Mag_A23;
float Mag_A31,Mag_A32,Mag_A33;

// hard iron correction vector for magnetometer
float Mag_B1,Mag_B2,Mag_B3;

Madgwick filter;
float imu_rate=10.0;	// Hz

bool use_compass=true;
bool compass_live_mode=false;
int sensor_setup=NO_SENSORS;

// Magdwick filter outputs for global use
float roll=0.0;
float pitch=0.0;
float yaw=0.0;
float heading=0.0;

void CalibrateMPU6500Accelerometer(const char *orientation,xyzFloat *vals);
void CalibrateMPU6500Gyro(void);

void ResetCalibration(void);

int SetupIMU(void)
{
	Serial.print("SetupIMU()\r\n");
	
	bool fail=true;
	int cnt=0;

	if(fail)	fail=DetectSeparateBoards();
	if(fail)	fail=DetectCombinedBoard();

#if 0
	if(fail)	fail=DetectMPU9250();
#endif
	
	if(fail)
	{
		Serial.println("MPU connection failed");
		use_compass=false;
		track_compass=false;
		return(1);
	}
	
	filter.begin(imu_rate);
	
#if 0
	// identity matrix i.e. no correction
	Mag_A11=1.0;			Mag_A12=0.0;			Mag_A13=0.0;
	Mag_A21=0.0;			Mag_A22=1.0;			Mag_A23=0.0;
	Mag_A31=0.0;			Mag_A32=0.0;			Mag_A33=1.0;

	mag_B1=0.0;				Mag_B2=0.0;				Mag_B3=0.0;
#endif
#if 0
	Serial.println("\tApply magnetometer calibration for 4 element Yagi");

	// soft iron calibration matrix for 4 element Yagi antenna
	Mag_A11=9.7436e-01;		Mag_A12=-2.2516e-03;	Mag_A13=-7.3596e-02;
	Mag_A21=-2.2516e-03;	Mag_A22=1.1269e+00;		Mag_A23-3.0113e-03;
	Mag_A31=-7.3596e-02;	Mag_A32=-3.0113e-03;	Mag_A33=9.9677e-01;
	
	// hard iron calibration vector for 4 element Yagi antenna
	Mag_B1=-10.6583;		Mag_B2=8.0037;			Mag_B3=34.7096;
#endif
#if 1
	PrintMagCalibration();
#endif

	Serial.print("SetupIMU() complete ...\r\n");
	
	return(0);
}

void PrintMagCalibration(void)
{
	Serial.println("\tSoft iron calibration matrix");
	Serial.printf("\t\tA11=%.6f,\tA12=%.6f,\tA13=%.6f\r\n",Mag_A11,Mag_A12,Mag_A13);
	Serial.printf("\t\tA21=%.6f,\tA22=%.6f,\tA23=%.6f\r\n",Mag_A21,Mag_A22,Mag_A23);
	Serial.printf("\t\tA31=%.6f,\tA32=%.6f,\tA33=%.6f\r\n\n",Mag_A31,Mag_A32,Mag_A33);
	Serial.println("\tHard iron calibration vector");
	Serial.printf("\t\tB1=%.6f,\tB2=%.6f,\tB3=%.6f\r\n",Mag_B1,Mag_B2,Mag_B3);

}

#if 0
bool DetectMPU9250(void)
{
	int cnt;
	bool fail=false;
	
	for(cnt=0;cnt<(sizeof(MPU9250Addresses)/sizeof(byte));cnt++)
	{
		if(MPU9250Addresses[cnt]==0x00)
		{
			fail=true;
		}
		else
		{
			Serial.printf("\tTrying 0x%02x ... ",MPU9250Addresses[cnt]);
			
//			if(mpu9250.setup(MPU9250Addresses[cnt]))	{	Serial.println("OK");	fail=false;		break;	}
//			else										{	Serial.println("fail");	fail=true;				}
			
			delay(250);
		}
	}
	
	return(1);
}
#endif

void displaySensorDetails(void)
{
	sensor_t sensor;
	hmc5883l.getSensor(&sensor);
	
	Serial.println("------------------------------------");
	Serial.print("Sensor:\t\t");	Serial.println(sensor.name);
	Serial.print("Driver Ver:\t");	Serial.println(sensor.version);
	Serial.print("Unique ID:\t");	Serial.println(sensor.sensor_id);
	Serial.print("Max Value:\t");	Serial.print(sensor.max_value);		Serial.println(" uT");
	Serial.print("Min Value:\t");	Serial.print(sensor.min_value);		Serial.println(" uT");
	Serial.print("Resolution:\t");	Serial.print(sensor.resolution);	Serial.println(" uT");
	Serial.println("------------------------------------");
	Serial.println("");
	
	delay(500);
} 

bool DetectSeparateBoards(void)
{
	bool fail=false;
	
	Serial.println("Setting up individual motion sensor boards ...");

	if(!mpu6500.init())
	{
		Serial.println("MPU6500 setup failed");
		fail=true;
	}
	else
	{
		Serial.println("\tMPU6500 gyro/accelerometer initialised ...");
	}

	if(!hmc5883l.begin())
	{
		Serial.println("HMC5883 setup failed");
		fail=true;
	} 
	else
	{
		Serial.println("\tHMC5883L Magnetometer initialised ...");
	}
	
	return(fail);
}

bool DetectCombinedBoard(void)
{


	return(true);
}

void PollIMU(void)
{
	if(!use_compass)
		return;

	static int update_filter_at=0;
	
	xyzFloat accel;
	xyzFloat gyro;
	
	xyzFloat uncalibrated_mag;
	xyzFloat calibrated_mag;
	
	static xyzFloat magmin={	 1000.0,	 1000.0,	 1000.0		};
	static xyzFloat magmax={	-1000.0,	-1000.0,	-1000.0		};
	
	if(millis()>update_filter_at)
	{
		update_filter_at=millis()+(int)(1000/imu_rate);
	
		sensors_event_t event; 
		hmc5883l.getEvent(&event);
		
		uncalibrated_mag.x=event.magnetic.x;
		uncalibrated_mag.y=event.magnetic.y;
		uncalibrated_mag.z=event.magnetic.z;
		
		// see https://teslabs.com/articles/magnetometer-calibration/ and 
		// https://sailboatinstruments.blogspot.com/2011/08/improved-magnetometer-calibration.html
		// for a better explanation than I have of all of this 
		
		xyzFloat mag_temp;
		
		// apply the mag offsets i.e. remove hard iron distortion
		mag_temp.x=uncalibrated_mag.x-Mag_B1;
		mag_temp.y=uncalibrated_mag.y-Mag_B2;
		mag_temp.z=uncalibrated_mag.z-Mag_B3;
		
		// apply the soft iron distortion matrix
		calibrated_mag.x=mag_temp.x*Mag_A11+mag_temp.y*Mag_A12+mag_temp.z*Mag_A13;
		calibrated_mag.y=mag_temp.x*Mag_A21+mag_temp.y*Mag_A22+mag_temp.z*Mag_A23;
		calibrated_mag.z=mag_temp.x*Mag_A31+mag_temp.y*Mag_A32+mag_temp.z*Mag_A33;
		
		// remap the axes to be the same as the gyro and accelerometer
		mag_temp.x=calibrated_mag.x;	mag_temp.y=calibrated_mag.y;	mag_temp.z=calibrated_mag.z;		
		calibrated_mag.x=mag_temp.y;	calibrated_mag.y=-mag_temp.x;	calibrated_mag.z=mag_temp.z;
		
		// these are values scaled to proper units with offsets applied from
		// the calibration data
		accel=mpu6500.getGValues();
		gyro=mpu6500.getGyrValues();

#if 1
		filter.update(
						gyro.x,gyro.y,gyro.z,
						accel.x,accel.y,accel.z,
						calibrated_mag.x,calibrated_mag.y,calibrated_mag.z
					);
#else
		filter.updateIMU(
						gyro.x,gyro.y,gyro.z,
						accel.x,accel.y,accel.z
					);
#endif
		
		heading=180.0-filter.getYaw();
		if(heading<0.0)		heading+=360.0;
		if(heading>360.0)	heading-=360.0;
	}

	if(compass_live_mode)
	{
		static int update_ui_at=0;
		
		if(millis()>update_ui_at)
		{
			update_ui_at=millis()+100;

#if 0
			Serial.printf("AccX: % .2f, AccY: % .2f, AccZ: % .2f\t",accel.x,accel.y,accel.z);
			Serial.printf("GyroX: % .2f, GyroY: % .2f, GyroZ: % .2f\t",gyro.x,gyro.y,gyro.z);
			Serial.printf("MagX: % .2f, MagY: % .2f, MagZ: % .2f, Heading: %.2f\r\n",uncalibrated_mag.x,uncalibrated_mag.y,uncalibrated_mag.z,heading);
#endif
#if 0
			Serial.printf("MagX: % .2f, MagY: % .2f, MagZ: % .2f, AccX: %.2f, AccY: %.2f, AccZ: %.2f, GyroX: %.2f, GyroY: %.2f, GyroZ: %.2f\r\n",
								uncalibrated_mag.x,uncalibrated_mag.y,uncalibrated_mag.z,
								accel.x,accel.y,accel.z,
								gyro.x,gyro.y,gyro.z
						);
#endif
#if 0
			Serial.printf("MagX: % .2f, MagY: % .2f, MagZ: % .2f, Cal_MagX: % .2f, Cal_MagY: % .2f, Cal_MagZ: % .2f\r\n",
								uncalibrated_mag.x,uncalibrated_mag.y,uncalibrated_mag.z,
								calibrated_mag.x,calibrated_mag.y,calibrated_mag.z
						);
#endif
#if 1
			Serial.printf("Cal_MagX: % .2f, Cal_MagY: % .2f, Cal_MagZ: % .2f, ",calibrated_mag.x,calibrated_mag.y,calibrated_mag.z);
			Serial.printf("Roll: %.1f, Pitch: %.1f, Yaw: %.1f, Heading: %.1f\r\n",filter.getRoll(),filter.getPitch(),filter.getYaw(),heading);
#endif
		}
	}
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
					Serial.printf("Heading: %.1f\r\n",heading);
					break;
		
		case 'p':	// pitch
					Serial.printf("Pitch: %.1f\r\n",pitch);
					break;
		
		case 'r':	// roll
					Serial.printf("Roll: %.1f\r\n",roll);
					break;
		
		case 'y':	// yaw
					Serial.printf("Yaw: %.1f\r\n",yaw);
					break;
		
		case 'l':	// live mode toggle
					compass_live_mode=!compass_live_mode;
					
					if(compass_live_mode)	Serial.print("Enabling live compass data mode\r\n");
					else					Serial.print("Disabling live compass data mode\r\n");
					
					break;
		
		case 'c':	// calibration mode toggle
					compass_live_mode=!compass_live_mode;
					
					if(compass_live_mode)	Serial.print("Enabling magnetometer calibration mode\r\n");
					else					Serial.print("Disabling magnetometer calibration mode\r\n");
					
					break;
		
		case '?':	Serial.print("IMU Test Harness\r\n================\r\n\n");
					Serial.print("c\t-\tEnable/disable magnetometer calibration mode\r\n");
					Serial.print("h\t-\tDisplay the compass heading\r\n");
					Serial.print("l\t-\tEnable/disable live compass data display\r\n");
					Serial.print("p\t-\tRead current pitch value\r\n");
					Serial.print("r\t-\tRead current roll value\r\n");
					Serial.print("y\t-\tRead current yaw value\r\n");
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
#if 0
		case 'c':	// clear all calibration
//					qmc5883l.clearCalibration();
					ResetCalibration();
					StoreCalibrationData();					
					break;
#endif		
		case 'g':	// calibrate the gyro
					CalibrateMPU6500Gyro();
					mpu6500.setGyrOffsets(gyrooffset);
					
					break;
#if 0
		case 'm':	// calibrate the magnetometer
					CalibrateQMC5883LMagnetometer();
					break;
#endif	
		case 'p':	// print valibration values
					PrintCalibrationValues();
					break;
#if 0		
		case 's':	// store the calibration values
					StoreCalibrationData();
					break;
		
		case 't':	// retrieve the calibration values
					RetrieveCalibrationData();
					break;
#endif
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

#if 0
	Serial.print("Magnetometer:\r\n");
	Serial.printf("\tX offset: %.1f\tX scale: %.1f\r\n",magoffset.x,magscale.x);
	Serial.printf("\tY offset: %.1f\tY scale: %.1f\r\n",magoffset.y,magscale.y);
	Serial.printf("\tZ offset: %.1f\tZ scale: %.1f\r\n\n",magoffset.z,magscale.z);
#endif
}

void ResetCalibration(void)
{
#if 0
	magscale.x=1.0;		magscale.y=1.0;		magscale.z=1.0;
	magoffset.x=0.0;	magoffset.y=0.0;	magoffset.z=0.0;
#endif

	accelmin.x=-16384.0;	accelmax.x=16384.0;
	accelmin.y=-16384.0;	accelmax.y=16384.0;
	accelmin.z=-16384.0;	accelmax.z=16384.0;
	
	gyrooffset.x=0.0;	gyrooffset.y=0.0;	gyrooffset.z=0.0;					
}

