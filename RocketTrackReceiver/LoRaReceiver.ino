
#include "Logging.h"
#include "LoRaReceiver.h"
#include "Packetisation.h"

#include <LoRa.h>

#define LORA_FREQ           434150000

double lora_frequency=LORA_FREQ;
double lora_offset=4000;
int lora_mode=LORA_HIGH_RATE_MODE;
int last_good_receive=0;
int lora_rssi=-123;
int lora_snr=9;

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
		case LORA_LONG_RANGE_MODE:	Serial.println("Setting LoRa to long range mode");
									LoRa.setSpreadingFactor(12);
									LoRa.setSignalBandwidth(31.25E3);
									LoRa.setCodingRate4(8);
									break;
		
		case LORA_HIGH_RATE_MODE:	Serial.println("Setting LoRa to high rate mode");
									LoRa.setSpreadingFactor(7);
									LoRa.setSignalBandwidth(125E3);
									LoRa.setCodingRate4(8);
									break;
		
		default:					Serial.println("Duff LoRa mode selected!");
									break;
	}
}

int SetupLoRaReceiver(void)
{
	// initialize the pins
	pinMode(LORA_RESET,OUTPUT);
	digitalWrite(LORA_RESET,HIGH);
	
	delay(100);          // Module needs this before it's ready  
	
	LoRa.setPins(LORA_NSS,LORA_RESET,LORA_DIO0);
	
	if(!LoRa.begin(lora_frequency+lora_offset))
	{
		Serial.println("Starting LoRa failed!");
		while(1)
		{
		}
	}
	
	LoRa.receive();
	
	SetLoRaMode(lora_mode);

	return(0);
}

void PollLoRaReceiver(int fakepacket)
{
	int packetSize=LoRa.parsePacket();

	// try to parse packet
	if(packetSize)
	{
		uint8_t *packet=(uint8_t *)malloc(packetSize);
		
		if(packet!=NULL)
		{
			int offset=LoRa.packetFrequencyError();
			lora_rssi=LoRa.packetRssi();
			lora_snr=LoRa.packetSnr();
			
			// received a packet
			Serial.print("Received packet '");
			
			// TODO: need to check the integrity of the received packet at this point and bail if it doesn't decode properly
			
			int cnt=0;
			
			// read packet
			while(LoRa.available()) 
				packet[cnt++]=LoRa.read();
			
			DecryptPacket(packet);
			UnpackPacket(packet,cnt);
			
			for(cnt=0;cnt<packetSize;cnt++)
			{
				Serial.printf("%02x",packet[cnt]);
			}
			
#if DISPLAY_ASCII_PACKET
			Serial.print("', '");
			
			for(cnt=0;cnt<packetSize;cnt++)
			{
				Serial.print((char)packet[cnt]);
			}
#endif
			
			// print RSSI of packet
			Serial.print("' with RSSI ");	Serial.print(lora_rssi);
			Serial.print(", with SNR ");	Serial.print(lora_snr);
			Serial.print(" and offset ");	Serial.print(offset);	Serial.println(" Hz");
			
#if 1			
			Serial.printf("Rx packet: Lat = %.6f, Long = %.6f, Height = %.1f, Acc = %.2f\t%s Mode\r\n",
				lastfix.latitude,lastfix.longitude,lastfix.height,lastfix.accuracy,lora_mode?"High Rate":"Long Range");
#endif
			
			if(offset>100)			lora_offset-=200.0;
			else if(offset<-100)	lora_offset+=200.0;
			else					lora_offset+=offset;
			
			LoRa.setFrequency(lora_frequency+lora_offset);
			
			free(packet);
		}
		
		last_good_receive=millis();
	}
}

void PollLoRaScheduler(void)
{	
	if(		(		(lora_mode==0)
				&&	(millis()>(last_good_receive+20000))	)
		||	(		(lora_mode==1)
				&&	(millis()>(last_good_receive+5000))	)	)
	{
		lora_mode=!lora_mode;
		SetLoRaMode(lora_mode);
		last_good_receive=millis();
		lora_offset=0;
		LoRa.setFrequency(lora_frequency+lora_offset);
	}
}
  
void PollLoRaRSSI(void)
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
}

int LoRaGetMode(void)
{
	return(lora_mode);
}

uint8_t hexnibble(char val)
{
	uint8_t retval;
	
	if((val>='0')&&(val<='9'))
	{
		retval=val-'0';
	}
	else if((val>='a')&&(val<='f'))
	{
		retval=val-'a'+10;	
	}
	else if((val>='A')&&(val<='F'))
	{
		retval=val-'F'+10;	
	}
	else
	{
		Serial.printf("Invalid hex character received - %c\r\n",val);
		retval=0;
	}
	
	return(retval);
}

int ReceiverCommandHandler(uint8_t *cmd,uint16_t cmdptr)
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

		case 'x':
#if 1		
					Serial.println((char *)cmd);
					Serial.println(cmdptr);
#endif	
		
					if((cmd[2]==' ')&&(cmdptr==(3+32+1)))
					{
						uint8_t packet[16];
						uint16_t packetlength=16;
						for(int cnt=0;cnt<sizeof(packet);cnt++)
							packet[cnt]=16*hexnibble(cmd[3+2*cnt])+hexnibble(cmd[4+2*cnt]);
							
						UnpackPacket(packet,packetlength);
					}
					else
					{
						Serial.println("Invalid packet, not inserting it");
						retval=1;
					}

					break;
		
		case '?':	Serial.print("Receiver Test Harness\r\n================\r\n\n");
//					Serial.print("c\t-\tInsert ciphertext packet\r\n");
					Serial.print("x\t-\tInsert plaintext packet\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

