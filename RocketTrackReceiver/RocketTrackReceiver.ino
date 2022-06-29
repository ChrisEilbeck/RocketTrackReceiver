
#define LIVE_RSSI_DISPLAY 0

#define DISPLAY_ASCII_PACKET 0

#define OLD_LORA 0

// Intended for use with a TTGO T-BEAM

#define DEBUG 2

// Power management
#include <axp20x.h>

#include <string.h>
#include <ctype.h>
#include <SPI.h>

#include <LoRa.h>

// RFM98

#define LORA_NSS	18	// Comment out to disable LoRa code
#define LORA_RESET	14	// Comment out ifnot connected
#define LORA_DIO0	26                

#define SCK			5	// GPIO5  -- SX1278's SCK
#define MISO		19	// GPIO19 -- SX1278's MISO
#define MOSI		27	// GPIO27 -- SX1278's MOSI

unsigned long UpdateClientAt=0;

#define LORA_FREQ           434650000

double lora_frequency=LORA_FREQ;
double lora_offset=4000;
int lora_mode=1;
int last_good_receive=0;

AXP20X_Class axp;

// initialize the library with the numbers of the interface pins

void SetLoRaMode(int mode)
{
#if LOW_POWER_TRANSMIT
	LoRa.setTxPower(5);
#else
	LoRa.setTxPower(17);
#endif
	
	switch(mode)
	{
		case 0:		Serial.println("Setting LoRa to long range mode");
					LoRa.setSpreadingFactor(12);
					LoRa.setSignalBandwidth(31.25E3);
					LoRa.setCodingRate4(8);
					break;
		
		case 1:		Serial.println("Setting LoRa to high rate mode");
					LoRa.setSpreadingFactor(7);
					LoRa.setSignalBandwidth(125E3);
					LoRa.setCodingRate4(8);
					break;
		
		default:	Serial.println("Duff LoRa mode selected!");
					break;
	}
}

void setup()
{
	Serial.begin(115200);
	
	Wire.begin(21,22);
	
	if(!axp.begin(Wire, AXP192_SLAVE_ADDRESS))	{	Serial.println("AXP192 Begin PASS");	}
	else										{	Serial.println("AXP192 Begin FAIL");	}
	
	axp.setPowerOutPut(AXP192_LDO2,AXP202_ON);
	axp.setPowerOutPut(AXP192_LDO3,AXP202_ON);
	axp.setPowerOutPut(AXP192_DCDC2,AXP202_ON);
	axp.setPowerOutPut(AXP192_EXTEN,AXP202_ON);
	axp.setPowerOutPut(AXP192_DCDC1,AXP202_ON);
	
	axp.setDCDC1Voltage(3300);
	
	Serial.printf("\tDCDC1: %s",axp.isDCDC1Enable()?"ENABLE":"DISABLE");	Serial.println("");
	Serial.printf("\tDCDC2: %s",axp.isDCDC2Enable()?"ENABLE":"DISABLE");	Serial.println("");
	Serial.printf("\tLDO2: %s",axp.isLDO2Enable()?"ENABLE":"DISABLE");		Serial.println("");
	Serial.printf("\tLDO3: %s",axp.isLDO3Enable()?"ENABLE":"DISABLE");		Serial.println("");
	Serial.printf("\tDCDC3: %s",axp.isDCDC3Enable()?"ENABLE":"DISABLE");	Serial.println("");
	Serial.printf("\tExten: %s",axp.isExtenEnable()?"ENABLE":"DISABLE");	Serial.println("");
	
	if(axp.isChargeing())	{	Serial.println("Charging");	}  
	
	Serial.println("");
	Serial.println("ESP32 LoRa Receiver V1.2");
	Serial.println("");

#if OLD_LORA
	SetParametersFromLoRaMode(2);
	setupRFM98();
#else
	Serial.println("LoRa Receiver");
	
	// initialize the pins
	pinMode(LORA_RESET,OUTPUT);
	digitalWrite(LORA_RESET,HIGH);
	
	delay(100);          // Module needs this before it's ready  
	
	LoRa.setPins(LORA_NSS,LORA_RESET,LORA_DIO0);
	
	if(!LoRa.begin(lora_frequency+lora_offset))
	{
		Serial.println("Starting LoRa failed!");
		while (1);
	}
	
	LoRa.receive();
	
	SetLoRaMode(lora_mode);
	
#if 0
	LoRa.setSpreadingFactor(12);
	LoRa.setSignalBandwidth(31.25E3);
	LoRa.setCodingRate4(8);
	LoRa.setFrequency(lora_frequency+lora_offset);
#endif
	
#endif
}

void loop()
{
	// try to parse packet
	int packetSize=LoRa.parsePacket();
	if(packetSize)
	{
		uint8_t *packet=(uint8_t *)malloc(packetSize);
		
		if(packet!=NULL)
		{
			int offset=LoRa.packetFrequencyError();
			int rssi=LoRa.packetRssi();
			
			// received a packet
			Serial.print("Received packet '");
			
			int cnt=0;
			
			// read packet
			while(LoRa.available()) 
				packet[cnt++]=LoRa.read();
			
			DecryptPacket(packet);
			
			for(cnt=0;cnt<packetSize;cnt++)
			{
				if(packet[cnt]<16)	Serial.print("0");
				Serial.print(packet[cnt],HEX);
			}
			
#if DISPLAY_ASCII_PACKET
			Serial.print("', '");
			
			for(cnt=0;cnt<packetSize;cnt++)
			{
				Serial.print((char)packet[cnt]);
			}
#endif
			
			// print RSSI of packet
			Serial.print("' with RSSI ");
			Serial.print(rssi);
			
			Serial.print(" and offset ");
			Serial.print(offset);
			Serial.println(" Hz");
			
			if(offset>100)			lora_offset-=20.0;
			else if(offset<-100)	lora_offset+=200.0;
			else					lora_offset+=offset;
			
			LoRa.setFrequency(lora_frequency+lora_offset);
			
			free(packet);
		}
		
		last_good_receive=millis();
	}
	
	UpdateClient();
}

void UpdateClient(void)
{
	if(millis()>=UpdateClientAt)
	{
		int CurrentRSSI;
		char Line[256];
		
		CurrentRSSI=LoRa.rssi();
		
#if LIVE_RSSI_DISPLAY
	#if 0
 		sprintf(Line,"CurrentRSSI=%d\r\n",CurrentRSSI);
	#else
		memset(Line,0,sizeof(Line));
		memset(Line,'#',CurrentRSSI+135);
//		sprintf(Line+CurrentRSSI+140,"\r\n");
	#endif
		
 		Serial.println(Line);
#endif
		
		UpdateClientAt=millis()+100;
	}
	
	if(		(		(lora_mode==0)
				&&	(millis()>(last_good_receive+30000))	)
		||	(		(lora_mode==1)
				&&	(millis()>(last_good_receive+5000))	)	)
	{
		lora_mode=!lora_mode;
		SetLoRaMode(lora_mode);
		last_good_receive=millis();
	}
}
  
