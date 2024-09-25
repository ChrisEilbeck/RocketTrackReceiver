
#define DEBUGCONFIG 1
  
//#include "Accelerometer.h"
#include "Barometer.h"
#include "Crypto.h"
#include "FlightEvents.h"
#include "GPS.h"
//#include "Gyro.h"
#include "Logging.h"
#include "LoRaReceiver.h"
//#include "Magnetometer.h"
#include "Webserver.h"

#include <IniFile.h>

#ifndef ADAFRUIT_FEATHER_M0
	#include <SPIFFSIniFile.h>
#endif

enum
{
	CFGSTRING=0,
	CFGINTEGER,
	CFGFLOAT,
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
	{	"LoRa",				"Frequency",		(void *)&lora_freq,					CFGFLOAT,		"434.150"			},
	{	"LoRa",				"Mode",				(void *)&lora_mode,					CFGINTEGER,		"1"					},
	{	"LoRa",				"EnableCRC",		(void *)&lora_crc,					CFGBOOL,		"1"					},
	{	"High Rate",		"Bandwidth",		(void *)&hr_bw,						CFGINTEGER,		"125000"			},
	{	"High Rate",		"SpreadingFactor",	(void *)&hr_sf,						CFGINTEGER,		"7"					},
	{	"High Rate",		"CodingRate",		(void *)&hr_cr,						CFGINTEGER,		"8"					},
	{	"High Rate",		"TransmitPeriodMS",	(void *)&hr_period,					CFGINTEGER,		"1000"				},
	{	"Long Range",		"Bandwidth",		(void *)&lr_bw,						CFGINTEGER,		"31250"				},
	{	"Long Range",		"SpreadingFactor",	(void *)&lr_sf,						CFGINTEGER,		"12"				},
	{	"Long Range",		"CodingRate",		(void *)&lr_cr,						CFGINTEGER,		"8"					},
	{	"Long Range",		"TransmitPeriodMS",	(void *)&lr_period,					CFGINTEGER,		"10000"				},
	{	"Flight Events",	"DetectLaunch",		(void *)&detect_launch,				CFGBOOL,		"1"					},
	{	"Flight Events",	"DetectApogee",		(void *)&detect_apogee,				CFGBOOL,		"1"					},
	{	"Flight Events",	"DetectLowLevel",	(void *)&detect_lowlevel,			CFGBOOL,		"1"					},
	{	"Flight Events",	"DetectLanding",	(void *)&detect_landing,			CFGBOOL,		"1"					},
//	{	"GPS",				"Type",				(void *)gps_type,					CFGSTRING,		"NMEA"				},
//	{	"GPS",				"InitialBaudRate",	(void *)&initial_baud,				CFGINTEGER,		"9600"				},
//	{	"GPS",				"FinalBaudRate",	(void *)&final_baud,				CFGINTEGER,		"9600"				},
//	{	"GPS",				"FixRate",			(void *)&fix_rate,					CFGINTEGER,		"1"					},
	{	"Barometer",		"Enable",			(void *)&baro_enable,				CFGBOOL,		"1"					},
	{	"Barometer",		"Type",				(void *)baro_type,					CFGSTRING,		"BME280"			},
	{	"Barometer",		"MeasurementRate",	(void *)&baro_rate,					CFGINTEGER,		"10"				},
//	{	"Accelerometer",	"Enable",			(void *)&acc_enable,				CFGBOOL,		"1"					},
//	{	"Accelerometer",	"Type",				(void *)acc_type,					CFGSTRING,		"MPU6050"			},
//	{	"Accelerometer",	"MeasurementRate",	(void *)&accel_rate,				CFGINTEGER,		"10"				},
//	{	"Gyro",				"Enable",			(void *)&gyro_enable,				CFGBOOL,		"1"					},
//	{	"Gyro",				"Type",				(void *)gyro_type,					CFGSTRING,		"MPU6050"			},
//	{	"Gyro",				"MeasurementRate",	(void *)&gyro_rate,					CFGINTEGER,		"10"				},
//	{	"Magnetometer",		"Enable",			(void *)&mag_enable,				CFGBOOL,		"1"					},
//	{	"Magnetometer",		"Type",				(void *)mag_type,					CFGSTRING,		"None"				},
//	{	"Magnetometer",		"MeasurementRate",	(void *)&mag_rate,					CFGINTEGER,		"10"				},
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
		
	Serial.print("\tUsing ");
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
	
	Serial.println("\tIni file exists");
	
	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	
	if(!ini.validate(buffer,bufferLen))
	{
		Serial.print("\tini file ");	Serial.print(ini.getFilename());	Serial.print(" not valid: ");	printErrorMessage(ini.getError());
		
		// Cannot do anything else
		Serial.println("ReadConfigFile() abort");
		return(1);
	}

	Serial.print("\tini file is valid, starting to read the parameters\r\n");
	
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
		Serial.print("\t\tLooking for Tag \"");		Serial.print(config[cnt].tag);	Serial.print("\" in Section \"");	Serial.print(config[cnt].section);	Serial.print("\"");
#endif
		
		if(ini.getValue(config[cnt].section,config[cnt].tag,buffer,sizeof(buffer)))
		{
#if DEBUGCONFIG
			Serial.print("\t-\tvalue = ");			Serial.println(buffer);
#endif
		}
		else
		{
			Serial.print(" Tag \"");		Serial.print(config[cnt].tag);	Serial.print("\" in Section \"");	Serial.print(config[cnt].section);	Serial.print("\"\t-\tnot found, subsituting the default value");
			strcpy(buffer,config[cnt].defaultvalue);
		}
		
		if(config[cnt].variable!=NULL)
		{
			if(config[cnt].type==CFGSTRING)
			{
#if 0
				Serial.println("Copying string ...");
#endif
				strcpy((char *)config[cnt].variable,buffer);
			}
			else if(config[cnt].type==CFGIPADDRESS)	
			{
#if 0
				Serial.println("Copying IP address ...");
#endif
				IPAddress temp;
				temp.fromString(buffer);
				*((IPAddress *)config[cnt].variable)=temp;
			}
			else if(config[cnt].type==CFGBOOL)
			{
#if 0
				Serial.print("Got boolean in section ");
				Serial.print(config[cnt].section);
				Serial.print(" with tag ");
				Serial.print(config[cnt].tag);
				Serial.print(" and value ");
				Serial.println(buffer);
#endif														
#if 0
				Serial.println("Copying boolean ...");		
#endif
				if(atoi(buffer)==0)	*((bool *)config[cnt].variable)=false;
				else				*((bool *)config[cnt].variable)=true;
			}
			else if(config[cnt].type==CFGINTEGER)	
			{
#if 0
				Serial.println("Copying integer ...");
#endif
				*((int *)config[cnt].variable)=atoi(buffer);
			}
			else if(config[cnt].type==CFGFLOAT)
			{
#if 0
				Serial.println("Copying double ...");
#endif
				*((double *)config[cnt].variable)=atof(buffer);
			}
			else
			{
				Serial.println("Unknown data type requested ...");
			}
		}
		
		cnt++;
		
#if 0
		Serial.println("Copied ...");
#endif
	}
	
	
	
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
		case '?':	Serial.print("Config/Settings Test Harness\r\n============================\r\n\n");
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

