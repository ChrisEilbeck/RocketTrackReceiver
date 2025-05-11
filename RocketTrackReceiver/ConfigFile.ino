
#define DEBUGCONFIG 0
  
#include <IniFile.h>

#include "Barometer.h"
#include "Crypto.h"
#include "FlightEvents.h"
#include "GPS.h"
#include "IMU.h"
#include "Logging.h"
#include "LoRaReceiver.h"
#include "Webserver.h"

#ifndef ADAFRUIT_FEATHER_M0
	#include <SPIFFSIniFile.h>
#endif

enum
{
	CFGSTRING=0,
	CFGINTEGER,
	CFGFLOAT,
	CFGDOUBLE,
	CFGIPADDRESS,
	CFGBOOL
};

typedef struct
{
	const char section[32];
	const char tag[32];
	void *variable;
	int type;
	const char defaultvalue[32];
} configvalue_t;

configvalue_t config[]={
//	{	"RocketTrack",		"Autostart",		(void *)&lora_constant_transmit,	CFGINTEGER,		"0"					},
//	{	"RocketTrack",		"Mode",				(void *)unit_mode,					CFGSTRING,		"RX"				},
//	{	"RocketTrack",		"SyncSensorsToGPS",	(void *)&sync_sampling,				CFGBOOL,		"1"					},

	{	"Crypto",			"Enable",			(void *)&crypto_enable,				CFGBOOL,		"1"					},
	{	"Crypto",			"Key",				(void *)crypto_key_hex,				CFGSTRING,		""					},

//	{	"WiFi",				"Enable",			(void *)&wifi_enable,				CFGBOOL,		"1"					},
	{	"WiFi",				"SSID",				(void *)ssid,						CFGSTRING,		"RocketRx"			},
	{	"WiFi",				"Password",			(void *)password,					CFGSTRING,		"marsflightcrew"	},

//	{	"LoRa",				"Frequency",		(void *)&lora_freq,					CFGFLOAT,		"434.150"			},
//	{	"LoRa",				"Mode",				(void *)&lora_mode,					CFGINTEGER,		"1"					},
//	{	"LoRa",				"EnableCRC",		(void *)&lora_crc,					CFGBOOL,		"1"					},

//	{	"High Rate",		"Bandwidth",		(void *)&hr_bw,						CFGINTEGER,		"125000"			},
//	{	"High Rate",		"SpreadingFactor",	(void *)&hr_sf,						CFGINTEGER,		"7"					},
//	{	"High Rate",		"CodingRate",		(void *)&hr_cr,						CFGINTEGER,		"8"					},
//	{	"High Rate",		"TransmitPeriodMS",	(void *)&hr_period,					CFGINTEGER,		"1000"				},

//	{	"Long Range",		"Bandwidth",		(void *)&lr_bw,						CFGINTEGER,		"31250"				},
//	{	"Long Range",		"SpreadingFactor",	(void *)&lr_sf,						CFGINTEGER,		"12"				},
//	{	"Long Range",		"CodingRate",		(void *)&lr_cr,						CFGINTEGER,		"8"					},
//	{	"Long Range",		"TransmitPeriodMS",	(void *)&lr_period,					CFGINTEGER,		"10000"				},

//	{	"Flight Events",	"DetectLaunch",		(void *)&detect_launch,				CFGBOOL,		"1"					},
//	{	"Flight Events",	"DetectApogee",		(void *)&detect_apogee,				CFGBOOL,		"1"					},
//	{	"Flight Events",	"DetectLowLevel",	(void *)&detect_lowlevel,			CFGBOOL,		"1"					},
//	{	"Flight Events",	"DetectLanding",	(void *)&detect_landing,			CFGBOOL,		"1"					},

//	{	"GPS",				"Type",				(void *)gps_type,					CFGSTRING,		"NMEA"				},
//	{	"GPS",				"InitialBaudRate",	(void *)&initial_baud,				CFGINTEGER,		"9600"				},
//	{	"GPS",				"FinalBaudRate",	(void *)&final_baud,				CFGINTEGER,		"9600"				},
//	{	"GPS",				"FixRate",			(void *)&fix_rate,					CFGINTEGER,		"1"					},

//	{	"Barometer",		"Enable",			(void *)&baro_enable,				CFGBOOL,		"1"					},
//	{	"Barometer",		"Type",				(void *)baro_type,					CFGSTRING,		"BME280"			},
//	{	"Barometer",		"MeasurementRate",	(void *)&baro_rate,					CFGINTEGER,		"10"				},

//	{	"Accelerometer",	"Enable",			(void *)&acc_enable,				CFGBOOL,		"1"					},
//	{	"Accelerometer",	"Type",				(void *)acc_type,					CFGSTRING,		"MPU6050"			},
//	{	"Accelerometer",	"MeasurementRate",	(void *)&accel_rate,				CFGINTEGER,		"10"				},

//	{	"Gyro",				"Enable",			(void *)&gyro_enable,				CFGBOOL,		"1"					},
//	{	"Gyro",				"Type",				(void *)gyro_type,					CFGSTRING,		"MPU6050"			},
//	{	"Gyro",				"MeasurementRate",	(void *)&gyro_rate,					CFGINTEGER,		"10"				},

//	{	"Magnetometer",		"Enable",			(void *)&mag_enable,				CFGBOOL,		"1"					},
//	{	"Magnetometer",		"Type",				(void *)mag_type,					CFGSTRING,		"None"				},
//	{	"Magnetometer",		"MeasurementRate",	(void *)&mag_rate,					CFGINTEGER,		"10"				},

	// soft iron calibration matrix
//	{	"Magnetometer",		"Mag_A11",			(void *)&Mag_A11,					CFGFLOAT,		"1.00"				},
//	{	"Magnetometer",		"Mag_A12",			(void *)&Mag_A12,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A13",			(void *)&Mag_A13,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A21",			(void *)&Mag_A21,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A22",			(void *)&Mag_A22,					CFGFLOAT,		"1.00"				},
//	{	"Magnetometer",		"Mag_A23",			(void *)&Mag_A23,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A31",			(void *)&Mag_A31,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A32",			(void *)&Mag_A32,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_A33",			(void *)&Mag_A33,					CFGFLOAT,		"1.00"				},
	
	// hard iron calibration vector
//	{	"Magnetometer",		"Mag_B1",			(void *)&Mag_B1,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_B2",			(void *)&Mag_B2,					CFGFLOAT,		"0.00"				},
//	{	"Magnetometer",		"Mag_B3",			(void *)&Mag_B3,					CFGFLOAT,		"0.00"				},

//	{	"Logging",			"Level",			(void *)&log_level,					CFGINTEGER,		"1"					},

	{	"",					"",					0,									0,				""					}
};

void printErrorMessage(uint8_t e,bool eol=true)
{
	switch(e)
	{
		case IniFile::errorNoError:			Serial.print("no error");				break;
		case IniFile::errorFileNotFound:	Serial.print("file not found");			break;
		case IniFile::errorFileNotOpen:		Serial.print("file not open");			break;
		case IniFile::errorBufferTooSmall:	Serial.print("buffer too small");		break;
		case IniFile::errorSeekError:		Serial.print("seek error");				break;
		case IniFile::errorSectionNotFound:	Serial.print("section not found");		break;
		case IniFile::errorKeyNotFound:		Serial.print("key not found");			break;
		case IniFile::errorEndOfFile:		Serial.print("end of file");			break;
		case IniFile::errorUnknownError:	Serial.print("unknown error");			break;

		default:							Serial.print("unknown error value");	break;
	}
  
	if(eol)
    	Serial.println();
}

int ReadConfigFile(const char *mode)
{
	Serial.println("ReadConfigFile() entry");
	
	char filename[32];

	if(strncmp(mode,"rx",2)==0)	strcpy(filename,"/receiver.ini");
	else						strcpy(filename,"/tracker.ini");
		
	Serial.print("\tLoading config from ");
	Serial.println(filename);

	const size_t bufferLen=128;
	char buffer[bufferLen];

	SPIFFSIniFile ini(filename);
		
	if(!ini.open())
	{
		Serial.print("\tIni file ");	Serial.print(filename);		Serial.println(" does not exist");
			
		// Cannot do anything else
		Serial.println("ReadConfigFile() abort");
		return(1);
	}
	else
		Serial.println("\tIni file exists");
	
	if(!ini.validate(buffer,bufferLen))
	{
		Serial.print("\tini file ");	Serial.print(ini.getFilename());	Serial.print(" not valid: ");	printErrorMessage(ini.getError());
		
		// Cannot do anything else
		Serial.println("ReadConfigFile() abort");
		return(1);
	}
	else
		Serial.print("\tini file is valid, starting to read the parameters\r\n");
	
	ini.setCaseSensitive(false);
	
	int cnt=0;
	int values_used=0;
	while(1)
	{
		if(		strlen(config[cnt].section)==0
			&&	strlen(config[cnt].tag)==0		)
		{
			break;
		}
		
#if DEBUGCONFIG
		Serial.print("\t\tLooking in Section \"");	Serial.print(config[cnt].section);	Serial.print("\" for Tag \"");	Serial.print(config[cnt].tag);		Serial.print("\"");
#endif
		
		if(ini.getValue(config[cnt].section,config[cnt].tag,buffer,sizeof(buffer)))
		{
#if DEBUGCONFIG
			Serial.print("\t-\tvalue = ");			Serial.println(buffer);
#endif
			if(config[cnt].variable!=NULL)
			{
				if(config[cnt].type==CFGSTRING)
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying string ...");
#endif
					strcpy((char *)config[cnt].variable,buffer);
				}
				else if(config[cnt].type==CFGIPADDRESS)	
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying IP address ...");
#endif
					IPAddress temp;
					temp.fromString(buffer);
					*((IPAddress *)config[cnt].variable)=temp;
				}
				else if(config[cnt].type==CFGBOOL)
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying boolean");
#endif														
					if(atoi(buffer)==0)	*((bool *)config[cnt].variable)=false;
					else				*((bool *)config[cnt].variable)=true;
				}
				else if(config[cnt].type==CFGINTEGER)	
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying integer ...");
#endif
					*((int *)config[cnt].variable)=atoi(buffer);
				}
				else if(config[cnt].type==CFGFLOAT)
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying float ...");
#endif
					*((float *)config[cnt].variable)=atof(buffer);
				}
				else if(config[cnt].type==CFGDOUBLE)
				{
#if DEBUGCONFIG
					Serial.println("\t\tCopying double ...");
#endif
					*((double *)config[cnt].variable)=atof(buffer);
				}
				else
				{
					Serial.println("\t\tUnknown data type requested ...");
				}
			}
			
#if 0
			Serial.println("\t\tCopied ...");
#endif
		}
		else
		{
			Serial.println("");
			
			error_t err=ini.getError();
			printErrorMessage(err,true);
		
//			Serial.print(" Tag \"");		Serial.print(config[cnt].tag);	Serial.print("\" in Section \"");	Serial.print(config[cnt].section);	Serial.print("\"\t-\tnot found, substituting the default value");
//			strcpy(buffer,config[cnt].defaultvalue);
		}
		
		cnt++;
	}

	ini.close();
	
	Serial.println("ReadConfigFile() exit");
	return(0);
}

int WriteConfigFile(void)
{

}

void SetDefaultConfigValues(void)
{
  
}

int ConfigCommandHandler(uint8_t *cmd,uint16_t cmdptr)
{
	// ignore a single key stroke
	if(cmdptr<=2)	return(0);

#if (DEBUG>0)
	Serial.println((char *)cmd);
#endif
	
	int retval=1;
	
	switch(cmd[1]|0x20)
	{
		case '?':	Serial.print("Config File Test Harness\r\n============================\r\n\n");
					Serial.print("d\t-\tDownload the config file\r\n");
					Serial.print("p\t-\tDisplay the config settings\r\n");
					Serial.print("r\t-\tRead the config file from SPIFFS\r\n");
					Serial.print("u\t-\tUpload the config file\r\n");
					Serial.print("w\t-\tWrite the config file to SPIFFS\r\n");
					Serial.print("x\t-\tRevert to default values\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	// ignore
					break;
	}
	
	return(retval);
}

int SettingsCommandHandler(uint8_t *cmd,uint16_t cmdptr)
{
	// ignore a single key stroke
	if(cmdptr<=2)	return(0);

#if (DEBUG>0)
	Serial.println((char *)cmd);
#endif
	
	int retval=1;
	
	switch(cmd[1]|0x20)
	{
		case 'd':	ResetAllSettings();
					break;
		
		case 's':	DisplayAllSettings();
					break;
		
		case '?':	Serial.print("Settings Test Harness\r\n============================\r\n\n");
					Serial.print("d\t-\tReset all settings to defaults\r\n");
					Serial.print("s\t-\tShow the current settings\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	// ignore
					break;
	}
	
	return(retval);
}

void ResetAllSettings(void)
{
	Serial.println("Resetting all Settings to defaults");

	lora_freq=LORA_CH2;
	if(lora_freq<1e6)	lora_freq*=1e6;
	
	lora_mode=LORA_HIGH_RATE_MODE;
	
	hr_bw=125000;
	hr_sf=7;
	hr_cr=8;
	
	lr_bw=31250;
	lr_sf=12;
	lr_cr=8;
	
	Mag_A11=1.0;	Mag_A12=0.0;	Mag_A13=0.0;
	Mag_A21=0.0;	Mag_A22=1.0;	Mag_A23=0.0;
	Mag_A31=0.0;	Mag_A32=0.0;	Mag_A33=1.0;
	
	Mag_B1=0.0;		Mag_B2=0.0;		Mag_B3=0.0;
	
	AccelOffset.x=0.0;	AccelOffset.y=0.0;	AccelOffset.z=0.0;
	AccelScale.x=1.0;	AccelScale.y=1.0;	AccelScale.z=1.0;
	
	GyroOffset.x=0.0;	GyroOffset.y=0.0;	GyroOffset.z=0.0;

	StoreCalibrationData();
	
	DisplayAllSettings();
}

void DisplayAllSettings(void)
{
#if LORA_BAND==434
	Serial.println("RocketTrackReceiver in 434MHz Mode\n");
#else
	Serial.println("RocketTrackReceiver in 868MHz Mode\n");
#endif

	Serial.printf("LoRa Freq %.3f MHz\r\n",lora_freq/1e6);

	if(lora_mode==LORA_HIGH_RATE_MODE)	Serial.println("High rate mode\n");
	else								Serial.println("Long range mode\n");

	Serial.printf("High Rate mode is BW=%d, SF=%d, CR=%d\r\n",hr_bw,hr_sf,hr_cr);
	Serial.printf("Long Range mode is BW=%d, SF=%d, CR=%d\r\n",lr_bw,lr_sf,lr_cr);

	PrintMagCalibration();
}

