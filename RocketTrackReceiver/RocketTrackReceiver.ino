
/*
	To Do
	
	-	support tracking multiple beacons at the same time
	
	-	move away from configuring things in an ini file, just use the
		nvmemory instead with ways to set back to defaults from the 
		test harness
	
	-	changing the wifi setup and crypto keys from the web UI is probably
		a bad thing to do and should be done via the test harness
	
	-	check it works when built in 868 MHz mode
	
	-	support different barometers
	
	-	add a button to the web UI do download a spot in KML format to use
		with a mapping application instead of just using the radar plot and
		following it in to the landing location
	
	-	add a search mechanism to get a beacon and a receiver to sync
		together even if set on the wrong channel, maybe using a bluetooth
		connection triggered by pressing and holding on a button to go into
		a pairing mode
	
	-	maybe start using an sd card rather than an external logger so we
		can log more data e.g.  the route taken to the landing site by the
		receiver
	
	-	merge with RocketTrack so there's only one codebase to maintain


















*/

#define LIVE_RSSI_DISPLAY 0

#define DISPLAY_ASCII_PACKET 0

// Intended for use with a TTGO T-BEAM, 3 button

#define DEBUG 2

// Power management
#include <axp20x.h>

#include <string.h>
#include <ctype.h>
#include <SPI.h>

#include "Global.h"

#include "Beeper.h"
#include "Button.h"
#include "HardwareAbstraction.h"
#include "Logging.h"
#include "LoRaReceiver.h"
#include "NvMemory.h"
#include "Packetisation.h"
#include "SpiffsSupport.h"

char *unit_mode="rx";

unsigned long UpdateClientAt=0;

void setup()
{
	Serial.begin(115200);
	
	// SPI
	SPI.begin(SCK,MISO,MOSI);
	
	// I2C
	Wire.begin(21,22);
	
#if LORA_BAND==434
	Serial.print("\n"
				 "--------  RocketTrack Flight Telemetry Receiver  --------\r\n"
				 "---------------------  434 MHz Mode  --------------------\r\n"
				 "\n");
#else
	Serial.print("\n"
				 "--------  RocketTrack Flight Telemetry Receiver  --------\r\n"
				 "---------------------  868 MHz Mode  --------------------\r\n"
				 "\n");
#endif
	
#if 0
	packhack();
#endif
	
	// mandatory peripherals
	
	if(SetupPMIC())					{	Serial.print("PMIC Setup failed, halting ...\r\n");					while(1);				}
	if(SetupSPIFFS())				{	Serial.println("SPIFFS Setup failed ...\r\n");												}
	if(SetupNvMemory())				{	Serial.print("EEPROM failed, you must calibrate the IMU\r\n");								}

	if(ReadConfigFile(unit_mode))
	{
		Serial.print("Reading the config file failed, substituting defaults ...\r\n");
		SetDefaultConfigValues();
	}
	
	if(RetrieveCalibrationData())	{	Serial.println("Failed to retrieve settings from NvMemory");								}
	
	if(SetupLoRaReceiver())			{	Serial.print("LoRa Setup failed, halting ...\r\n");					while(1);				}
	if(SetupGPS())					{	Serial.print("GPS Setup failed, halting ...\r\n");					while(1);				}
	if(SetupCrypto())				{	Serial.print("Crypto Setup failed, halting ...\r\n");				while(1);				}

	if(SetupWebServer())			{	Serial.print("Web Server Setup failed, disabling ...\r\n");									}
	if(SetupDisplay())				{	Serial.print("OLED display setup failed, ignoring\r\n");									}
	if(SetupIMU())					{	Serial.print("IMU setup failed, disabling ...\r\n");										}
	if(SetupBarometer())			{	Serial.print("Pressure Sensor Setup failed, disabling ...\r\n");							}
	
#if 0
	DumpHexPacket(crypto_key,32);
#endif

	if(SetupButton())				{	Serial.print("Button Setup failed, disabling ...\r\n");				button_enable=false;	}
	if(SetupBeeper())				{	Serial.print("Beeper Setup failed, disabling ...\r\n");				beeper_enable=false;	}

#if 0
	// optional peripherals
	if(SetupLEDs())					{	Serial.print("LED Setup failed, halting ...\r\n");					while(1);				}
	if(SetupNeopixels())			{	Serial.print("Neopixels Setup failed, disabling ...\r\n");			neopixels_enable=false;	}
#endif

#if 1
	// insert a dummy unencrypted packet for testing without turning on a transmitter

//	uint8_t *packet=(uint8_t *)"\x00\xC9\xC9\x96\x9E\xFE\x6C\x44\x0E\x1F\x4D\x00\x0B\xCA\x09\x00";
//	uint8_t *packet=(uint8_t *)"\x00\xCC\xDC\x2E\x9F\xFE\xB6\xB7\x0A\x1F\x3A\x00\x0F\xC9\xE3\x00";
	
	// Smeaton's Pier Lighthouse, St Ives
//	uint8_t *packet=(uint8_t *)"\x00\xc6\xed\x0b\xf5\xff\x4a\x6d\x64\x00\x7b\x00\x01\x04\x00\x00";

	// Bredon Hill, 18.6km at 97.3 degrees
	uint8_t *packet=(uint8_t *)"\x00\xc6\xca\xde\xfb\xff\xb2\x1e\x68\x00\xf4\x01\x01\x04\x00\x00";
	uint16_t packetlength=16;

	UnpackPacket(packet,packetlength);
#endif
#if 0
	RetrieveCalibrationData();
#endif
}

void packhack(void)
{
	uint8_t packet[16];
	uint16_t packetlength;
	
	PackPacket(packet,&packetlength);
	DumpHexPacket(packet,packetlength);
	
	UnpackPacket(packet,packetlength);

#if 0
	Serial.print("Hanging ...\r\n");
	while(1);
#endif
}

void loop()
{
	PollLoRaReceiver(0);
	PollLoRaScheduler();
	PollLoRaRSSI();
	PollGPS();
	PollDisplay();
	PollSerial();
	PollPMIC();
	PollIMU();
	PollBeeper();
	
	int ButtonEvent=PollButton();
	
	if(ButtonEvent==BUTTON_LONG_PRESS)
	{
		beeper_enable=!beeper_enable;
		
		if(beeper_enable)	Serial.println("Beeper enabled");
		else				Serial.println("Beeper disabled");
	}
}

void PollSerial(void)
{
	static uint8_t cmd[128];
	static uint16_t cmdptr=0;
	char rxbyte;
	
	while(Serial.available())
	{ 
		rxbyte=Serial.read();
		
		cmd[cmdptr++]=rxbyte;
		
		if((rxbyte=='\r')||(rxbyte=='\n'))
		{
			ProcessCommand(cmd,cmdptr);
			memset(cmd,0,sizeof(cmd));
			cmdptr=0;
		}
		else if(cmdptr>=sizeof(cmd))
		{
			cmdptr--;
		}
	}
}

void ProcessCommand(uint8_t *cmd,uint16_t cmdptr)
{
	int OK=0;
	
	switch(cmd[0]|0x20)
	{
		case 'b':	OK=BarometerCommandHandler(cmd,cmdptr);				break;
		case 'c':	OK=SensorCalibrationCommandHandler(cmd,cmdptr);		break;
//		case 'e':	OK=LEDCommandHandler(cmd,cmdptr);					break;
//		case 'g':	OK=GPSCommandHandler(cmd,cmdptr);					break;
//		case 'h':	OK=HighRateCommandHandler(cmd,cmdptr);				break;
		case 'i':	OK=IMUCommandHandler(cmd,cmdptr);					break;
//		case 'l':	OK=LORACommandHandler(cmd,cmdptr);					break;
//		case 'n':	OK=NeopixelCommandHandler(cmd,cmdptr);				break;
		case 'n':	OK=NvMemoryCommandHandler(cmd,cmdptr);				break;
//		case 'o':	OK=LongRangeCommandHandler(cmd,cmdptr);				break;
		case 'p':	OK=PMICCommandHandler(cmd,cmdptr);					break;
		case 'r':	OK=ReceiverCommandHandler(cmd,cmdptr);				break;
		case 's':	OK=ConfigCommandHandler(cmd,cmdptr);				break;
		case 't':	OK=SettingsCommandHandler(cmd,cmdptr);				break;
		case 'u':	OK=ButtonCommandHandler(cmd,cmdptr);				break;
		case 'v':	OK=BeeperCommandHandler(cmd,cmdptr);				break;
//		case 'z':	OK=BeeperCommandHandler(cmd,cmdptr);				break;
		
		case 'x':	i2c_bus_scanner();
					OK=1;
					break;
		
		case '?':	Serial.print("RocketTrack Test Harness Menu\r\n========================\r\n\n");
					Serial.print("b\t-\tBarometer\r\n");
					Serial.print("c\t-\tSensor Calibration\r\n");
//					Serial.print("e\t-\tLed\r\n");
//					Serial.print("g\t-\tGPS\r\n");
//					Serial.print("h\t-\tHigh Rate Mode\r\n");
					Serial.print("i\t-\tIMU\r\n");
//					Serial.print("l\t-\tLoRa\r\n");
					Serial.print("n\t-\tNon-volatile Memory\r\n");
//					Serial.print("o\t-\tLong Range Mode\r\n");
					Serial.print("p\t-\tPower Management IC\r\n");
//					Serial.print("n\t-\tNeopixel\r\n");
					Serial.print("r\t-\tReceiver Commands\r\n");
					Serial.print("s\t-\tConfig/Settinsg Commands\r\n");
//					Serial.print("t\t-\tTransmitter Commands\r\n");
					Serial.print("x\t-\tScan I2c Bus for Devices\r\n");
					Serial.print("u\t-\tButton\r\n");
					Serial.print("v\t-\tBeeper\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					OK=1;
					break;
					
		default:	// do nothing
					break;
	}
	
	if(OK)	{	Serial.println("ok ...");	}
	else	{	Serial.println("?");	}
}

void i2c_bus_scanner(void)
{
	byte error;
	byte address;
	int nDevices;
	
	Serial.println("Scanning...");
	
	nDevices=0;
	for(address=1;address<127;address++) 
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		
		if(error==0)
		{
			Serial.print("I2C device found at address 0x");
			if(address<16) 
				Serial.print("0");
			Serial.print(address,HEX);
			
			if(address==0x0c)	Serial.print("\tAK8963 Magnetometer");
			if(address==0x0d)	Serial.print("\tQMC5883L Magnetometer");
			if(address==0x1e)	Serial.print("\tHMC5883L Magnetometer");
			if(address==0x34)	Serial.print("\tAXP192 PMIC");
			if(address==0x3C)	Serial.print("\tSSD1306 OLED Display");
			if(address==0x3D)	Serial.print("\tSSD1306 OLED Display");
			if(address==0x68)	Serial.print("\tMPU6050 Accelerometer/Gyro");
			if(address==0x69)	Serial.print("\tMPU6050 Accelerometer/Gyro");
			if(address==0x76)	Serial.print("\tBMP280 Barometer");
			if(address==0x77)	Serial.print("\tBMP280 Barometer");
			
			Serial.println("");
			
			nDevices++;
		}
		else if(error==4) 
		{
			Serial.print("Unknown error at address 0x");
			if(address<16) 
				Serial.print("0");
			Serial.println(address,HEX);
		}    
	}
	
	if(nDevices==0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("done\n");
}

