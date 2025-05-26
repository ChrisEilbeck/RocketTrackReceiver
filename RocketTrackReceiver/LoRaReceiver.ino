
#include "Global.h"

#include "Beeper.h"
#include "Logging.h"
#include "LoRaReceiver.h"
#include "Packetisation.h"

#include <LoRa.h>

float lora_freq;		//=LORA_CH2;
float lora_offset=0;
int lora_mode;			//=LORA_HIGH_RATE_MODE;
bool lora_crc=true;

// High Rate mode settings

int hr_bw=125000;
int hr_sf=7;
int hr_cr=8;
int hr_period;		// not used in RocketTrackReceiver

// Long Range mode settings

int lr_bw=31250;
int lr_sf=12;
int lr_cr=8;
int lr_period;		// not used in RocketTrackReceiver

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
									LoRa.setSignalBandwidth(lr_bw);
									LoRa.setSpreadingFactor(lr_sf);
									LoRa.setCodingRate4(lr_cr);
									break;
		
		case LORA_HIGH_RATE_MODE:	Serial.println("Setting LoRa to high rate mode");
									LoRa.setSignalBandwidth(hr_bw);
									LoRa.setSpreadingFactor(hr_sf);
									LoRa.setCodingRate4(hr_cr);
									break;
		
		default:					Serial.println("Duff LoRa mode selected!");
									break;
	}
}

int SetupLoRaReceiver(void)
{
	Serial.println("LoRa Receiver Configuration ...");

	// initialize the pins
	pinMode(LORA_RESET,OUTPUT);
	digitalWrite(LORA_RESET,HIGH);
	
	delay(100);          // Module needs this before it's ready  
	
	LoRa.setPins(LORA_NSS,LORA_RESET,LORA_DIO0);
	
//	lora_freq=LORA_CH2;
	
	if(lora_freq<1e6)	lora_freq*=1e6;
	lora_freq+=lora_offset;
	
	Serial.print("\tSetting receive frequency to ");	Serial.printf("%3.3f",lora_freq/1e6);	Serial.println(" MHz");
	
	if(!LoRa.begin(lora_freq))
	{
		Serial.println("Starting LoRa failed!");
		while(1)
		{
		}
	}
	
	LoRa.receive();
	
	SetLoRaMode(lora_mode);

	Serial.println("LoRa Receiver Initialised ...");

	return(0);
}

int ValidatePacket(uint8_t *packet,uint16_t packetsize,int packetrssi,float packetsnr)
{
	// check the length is 16 byts
	if(packetsize!=16)		return(0);
	
	// check the beacon id is between 0 and 15
	if(packet[0]>15)		return(0);	
	
	// check the snr is -10 or better
	if(packetsnr<-10)		return(0);
	
	// check the packet counter is more than the last good one
	
	
	// check the battery voltage is sensible
	
	
	// for now, check the accuracy is 00
//	if(packet[12]!=0)		return(0);
	
	return(1);
}

void PollLoRaReceiver(int fakepacket)
{
	int packetsize=LoRa.parsePacket();

	// try to parse packet
	if(packetsize)
	{
		uint8_t *packet=(uint8_t *)malloc(packetsize);
		
		if(packet!=NULL)
		{
			int packetoffset=LoRa.packetFrequencyError();
			int packetrssi=LoRa.packetRssi();
			float packetsnr=LoRa.packetSnr();
			
			// received a packet
			Serial.print("Received packet ");

			int cnt=0;
			
			// read packet
			while(LoRa.available()) 
				packet[cnt++]=LoRa.read();
			
			DecryptPacket(packet);
						
			Serial.print("'");
			
			for(cnt=0;cnt<packetsize;cnt++)
				Serial.printf("%02x",packet[cnt]);
			
			Serial.print("'");
			
#if DISPLAY_ASCII_PACKET
			Serial.print("', '");
			
			for(cnt=0;cnt<packetsize;cnt++)
			{
				Serial.print((char)packet[cnt]);
			}
#endif
			
			// TODO: need to check the integrity of the received packet at this point and bail if it doesn't decode properly

			if(!ValidatePacket(packet,packetsize,packetrssi,packetsnr))
			{
				Serial.print("with errors\r\n");		
			}
			else
			{
				UnpackPacket(
								packet,
								cnt,
								(int8_t)packetrssi,
								(int8_t)packetsnr,
								lora_mode
							);
				
				if(packetoffset>100)		lora_offset-=200.0;
				else if(packetoffset<-100)	lora_offset+=200.0;
				else						lora_offset+=packetoffset;
				
				LoRa.setFrequency(lora_freq+lora_offset);

				Serial.print(" with RSSI ");	Serial.print(lastfix.rssi);
				Serial.print(", with SNR ");	Serial.print(lastfix.snr);
				Serial.print(" and offset ");	Serial.print(lastfix.offset);	Serial.println(" Hz");
				
				Serial.printf("Rx packet: Lat = %.6f, Long = %.6f, Height = %.1f, Acc = %.2f\t%s Mode\r\n",
					lastfix.latitude,lastfix.longitude,lastfix.height,lastfix.accuracy,lastfix.rxmode?"High Rate":"Long Range");
				
				if(lora_mode==LORA_LONG_RANGE_MODE)
					BeeperSetPattern(0b10100000000000000000000000000000,0);
				else
					BeeperSetPattern(0b10000000000000000000000000000000,0);
				
				UpdateFlightEvents(lastfix.latitude,lastfix.longitude,lastfix.height);			
			}

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
		LoRa.setFrequency(lora_freq+lora_offset);
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
		case '1':	lora_freq=LORA_CH1;
					Serial.println("Setting to LoRa Channel 1");
					retval=1;
					break;

		case '2':	lora_freq=LORA_CH2;
					Serial.println("Setting to LoRa Channel 2");
					retval=1;
					break;

		case '3':	lora_freq=LORA_CH3;
					Serial.println("Setting to LoRa Channel 3");
					retval=1;
					break;

		case '4':	lora_freq=LORA_CH4;
					Serial.println("Setting to LoRa Channel 4");
					retval=1;
					break;

		case '5':	lora_freq=LORA_CH5;
					Serial.println("Setting to LoRa Channel 5");
					retval=1;
					break;

		case '6':	lora_freq=LORA_CH6;
					Serial.println("Setting to LoRa Channel 6");
					retval=1;
					break;

		case 'x':
#if 1
					Serial.println((char *)cmd);
#endif
#if 0
					Serial.println(cmdptr);
#endif	
		
					if((cmd[2]==' ')&&(cmdptr==(3+32+1)))
					{
						uint8_t packet[16];
						uint16_t packetlength=16;
						for(int cnt=0;cnt<sizeof(packet);cnt++)
							packet[cnt]=16*hexnibble(cmd[3+2*cnt])+hexnibble(cmd[4+2*cnt]);
						
						if(!ValidatePacket(packet,packetlength,-100,3))
							Serial.println("Rejecting errored packet");
						else
						{
							UnpackPacket(packet,packetlength,-100,10,0);
							
							Serial.printf("Rx packet: Lat = %.6f, Long = %.6f, Height = %.1f, Acc = %.2f\t%s Mode\r\n",
								lastfix.latitude,lastfix.longitude,lastfix.height,lastfix.accuracy,lora_mode?"High Rate":"Long Range");
				
							UpdateFlightEvents(lastfix.latitude,lastfix.longitude,lastfix.height);
							
							BeeperSetPattern(0b10000000000000000000000000000000,0);
						}
					}
					else
					{
						Serial.println("Invalid packet, not inserting it");
						retval=1;
					}

					break;

/*
	Worcestershire Beacon 52.10464,-2.34014, 427
	Whittington Tump 52.1687, -2.1843, 50
*/	


		
		case '?':	Serial.print("Receiver Test Harness\r\n================\r\n\n");
					Serial.print("1..6\t-\tSet LoRa Channel\r\n");
//					Serial.print("a\t-\tInsert Worcestershire Beacon packet\r\n");
//					Serial.print("c\t-\tInsert ciphertext packet\r\n");
					Serial.print("x\t-\tInsert plaintext packet\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}

