
#define DEBUG 1

#define GPS_PASSTHROUGH 0

#define MAX_CHANNELS 50

// Globals
byte RequiredFlightMode=0;
byte GlonassMode=0;
byte RequiredPowerMode=-1;
byte LastCommand1=0;
byte LastCommand2=0;
byte HaveHadALock=0;

bool gps_live_mode=0;

// these are all unpacked from UBX messages

// from NAV-STATUS
uint32_t iTOW=0;
uint8_t gpsFix=0;
uint8_t flags=0;
uint8_t fixStat=0;
uint8_t flags2=0;
uint32_t ttff=0;
uint32_t msss=0;

// from NAV-SVINFO
uint8_t numCh=0;
uint8_t globalFlags=0;
uint16_t reserved2=0;

uint8_t chn[MAX_CHANNELS];
uint8_t svid[MAX_CHANNELS];
uint8_t svflags[MAX_CHANNELS];
uint8_t quality[MAX_CHANNELS];
uint8_t cno[MAX_CHANNELS];
int8_t elev[MAX_CHANNELS];
int16_t azim[MAX_CHANNELS];
int32_t prRes[MAX_CHANNELS];

// from NAV-POSLLH
int32_t rxlon=-2.31*1e7;
int32_t rxlat=52.1*1e7;
int32_t rxheight=200*1e3;
int32_t rxhMSL=0;
uint32_t rxhAcc=0;
uint32_t rxvAcc=0;

uint8_t hAccValue=0;

uint16_t gps_year;
uint8_t gps_month;
uint8_t gps_day;
uint8_t gps_hour;
uint8_t gps_min;
uint8_t gps_sec;

void CalculateChecksum(uint8_t *buffer,uint16_t bufferptr,uint8_t *CK_A,uint8_t *CK_B)
{
	uint16_t cnt;
	
	*CK_A=0;
	*CK_B=0;
	
	for(cnt=2;cnt<(bufferptr-2);cnt++)
	{
		*CK_A+=buffer[cnt];
		*CK_B+=*CK_A;
	}
}

void FixUBXChecksum(uint8_t *buffer,uint16_t bufferptr)
{ 
	uint16_t cnt;
	uint8_t CK_A=0;
	uint8_t CK_B=0;
	
	for(cnt=2;cnt<(bufferptr-2);cnt++)
	{
		CK_A+=buffer[cnt];
		CK_B+=CK_A;
	}
	
	buffer[bufferptr-2]=CK_A;
	buffer[bufferptr-1]=CK_B;
}

bool CheckChecksum(uint8_t *buffer,uint16_t bufferptr)
{
	uint8_t CK_A;
	uint8_t CK_B;
	
	CalculateChecksum(buffer,bufferptr,&CK_A,&CK_B);
	
	if((CK_A==buffer[bufferptr-2])&&(CK_B==buffer[bufferptr-1]))	return(1);
	else															return(0);
}

void SendUBX(uint8_t *Message,uint16_t bufferptr)
{
	uint16_t cnt;
	
	LastCommand1=Message[2];
	LastCommand2=Message[3];
	
	for(cnt=0;cnt<bufferptr;cnt++)
		Serial1.write(Message[cnt]);
}

void EnableRawMeasurements(void)
{
	// send both RAM hacks, doesn't seem to hurt if you do both
	
	// for v6.02 ROM
	uint8_t cmd1[]={	0xb5,0x62,0x09,0x01,0x10,0x00,0xdc,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x23,0xcc,0x21,0x00,0x00,0x00,0x02,0x10,0x27,0x0e	};
	
	// for v7.03 ROM
	uint8_t cmd2[]={	0xb5,0x62,0x09,0x01,0x10,0x00,0xc8,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x97,0x69,0x21,0x00,0x00,0x00,0x02,0x10,0x2b,0x22	};
	
	SendUBX(cmd1,sizeof(cmd1));
	SendUBX(cmd2,sizeof(cmd2));
	
#if (DEBUG>0)
	Serial.println("Enabling raw measurements ...");
#endif
}

void SetMessageRate(uint8_t id1,uint8_t id2,uint8_t rate)
{
	unsigned char Disable[]={	0xB5,0x62,0x06,0x01,0x08,0x00,id1,id2,0x00,rate,rate,0x00,0x00,0x01,0x00,0x00	};
	
	FixUBXChecksum(Disable,sizeof(Disable));
	SendUBX(Disable,sizeof(Disable));
}

void SetFlightMode(byte NewMode)
{
	// Send navigation configuration command
	unsigned char setNav[]={	0xB5,0x62,0x06,0x24,0x24,0x00,0xFF,0xFF,0x06,0x03,0x00,0x00,0x00,0x00,0x10,0x27,0x00,0x00,0x05,0x00,0xFA,0x00,0xFA,0x00,0x64,0x00,0x2C,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x16,0xDC	};
	
	setNav[8]=NewMode;
	
	FixUBXChecksum(setNav,sizeof(setNav));
	
	SendUBX(setNav,sizeof(setNav));
}
    
void ChangeBaudRate(uint32_t BaudRate)
{
	char cmd[64];
	
	if(BaudRate==115200)	sprintf(cmd,"$PUBX,41,1,0007,0003,115200,0*18\r\n");
	if(BaudRate==38400)		sprintf(cmd,"$PUBX,41,1,0007,0003,38400,0*20\r\n");
	if(BaudRate==19200)		sprintf(cmd,"$PUBX,41,1,0007,0003,19200,0*25\r\n");
	if(BaudRate==9600)		sprintf(cmd,"$PUBX,41,1,0007,0003,9600,0*10\r\n");
	
	if(strlen(cmd)>0)
	{
		SendUBX((uint8_t *)cmd,strlen(cmd));
	}
}

void Set5Hz_Fix_Rate()
{
	uint8_t cmd[]={	0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A	};
	
	FixUBXChecksum(cmd,sizeof(cmd));
	SendUBX(cmd,sizeof(cmd));
}

int SetupGPS(void)
{
	Serial.println("Open GPS port");
	
	// this could do with some autobauding
	
	Serial1.begin(9600,SERIAL_8N1,34,12);	// Pins for T-Beam v0.8 (3 push buttons) and up

	// this also assumes we're using a u-Blox receiver, not always true

	#warning "this only support a u-Blox GPS receiver"

#if 0
	// turn off all NMEA output	
	SetMessageRate(0xf0,0x00,0x01);	// GPGGA
	SetMessageRate(0xf0,0x01,0x01);	// GPGLL
	SetMessageRate(0xf0,0x02,0x01);	// GPGSA
	SetMessageRate(0xf0,0x03,0x01);	// GPGSV
	SetMessageRate(0xf0,0x04,0x00);	// GPRMC
	SetMessageRate(0xf0,0x05,0x00);	// GPVTG
#else
	// turn off all NMEA output	
	SetMessageRate(0xf0,0x00,0x00);	// GPGGA
	SetMessageRate(0xf0,0x01,0x00);	// GPGLL
	SetMessageRate(0xf0,0x02,0x00);	// GPGSA
	SetMessageRate(0xf0,0x03,0x00);	// GPGSV
	SetMessageRate(0xf0,0x04,0x00);	// GPRMC
	SetMessageRate(0xf0,0x05,0x00);	// GPVTG

	SetMessageRate(0x01,0x02,0x01);	// NAV-POSLLH every fix
	SetMessageRate(0x01,0x03,0x05);	// NAV-STATUS every fifth fix
	SetMessageRate(0x01,0x30,0x05);	// NAV-SVINFO every fifth fix
	SetMessageRate(0x01,0x21,0x05);	// NAV-TIMEUTC every fifth fix
	Set5Hz_Fix_Rate();
#endif
	
	return(0);
}

void PollGPS(void)
{
	static uint8_t buffer[1024];
	static uint16_t bufferptr=0;
	uint8_t rxbyte=0x00;
	static uint8_t lastbyte=0x00;
	
#if GPS_PASSTHROUGH
	if(Serial1.available())	{	rxbyte=Serial1.read();	Serial.write(rxbyte);	}
	if(Serial.available())	{	rxbyte=Serial.read();	Serial1.write(rxbyte);	}
#else
	while(Serial1.available())
	{
		rxbyte=Serial1.read();
		
		if(gps_live_mode)
			Serial.write(rxbyte);
		
		if((lastbyte==0xb5)&&(rxbyte==0x62))
		{
			ProcessUBX(buffer,bufferptr);
			
			// this is the start of a ubx message so we have a full one stored, process it
			buffer[0]=lastbyte;
			buffer[1]=rxbyte;
			bufferptr=2;
		}
		else
		{
			// this is the middle of a ubx message
			
			if(bufferptr<sizeof(buffer))
			{
// 				static uint16_t msglength=0;
				
				buffer[bufferptr++]=rxbyte;
				
// 				if((msglength==0)&&(bufferptr>=6))
// 				{
// 					msglength=*((uint16_t *)(buffer+4));
// 				}
				
// 				if(bufferptr==(8+msglength))
// 				{
// #if (DEBUG>2)
// 					int cnt;
// 					for(cnt=0;cnt<8+msglength;cnt++)
// 						Serial.printf("%02x ",buffer[cnt]);
// 					
// 					Serial.println("");
// #endif
// 					
// 				}
			}
			else
			{
				// ignore the bytes
			}
		}
		
		lastbyte=rxbyte;
	}
#endif
}

void ProcessUBX(uint8_t *buffer,uint16_t bufferptr)
{
	if(bufferptr<=6)
	{
		// this is an invalid ubx message, we need at least the ident, two bytes 
		// for the message type and two bytes of checksum
		return;
	}
	
//	if(!CheckChecksum(buffer,bufferptr))
//	{
//		return;	// failed the checksum test
//	}
	
//	Serial.print("UBX rx\r\n");
	
	if((buffer[2]==0x01)&&(buffer[3]==0x02))	UnpackNAVPOSLLH(buffer);
	if((buffer[2]==0x01)&&(buffer[3]==0x03))	UnpackNAVSTATUS(buffer);
	if((buffer[2]==0x01)&&(buffer[3]==0x30))	UnpackNAVSVINFO(buffer);
	if((buffer[2]==0x01)&&(buffer[3]==0x21))	UnpackNAVTIMEUTC(buffer);
}

void UnpackNAVPOSLLH(uint8_t *buffer)
{
#if (DEBUG>1)
	Serial.println("\t\tNAV-POSLLH");
#endif
	
	iTOW=*((uint32_t *)(buffer+6));
	rxlon=*((int32_t *)(buffer+10));
	rxlat=*((int32_t *)(buffer+14));
	rxheight=*((int32_t *)(buffer+18));
	rxhMSL=*((int32_t *)(buffer+22));
	rxhAcc=*((uint32_t *)(buffer+26));
	rxvAcc=*((uint32_t *)(buffer+30));
	
	if((rxhAcc/500)>255)	hAccValue=255;
	else					hAccValue=(uint8_t)(rxhAcc/500);
	
#if 0
	Serial.printf("\t\thAcc = %ld mm\n",hAcc);
#endif
#if (DEBUG>2)
	Serial.print("\t\t");
	for(int cnt=0;cnt<34;cnt++)
		Serial.printf("%02x ",buffer[cnt]);
	
	Serial.print("\r\n");
	
	Serial.printf("\t\tLat = %.6f, Lon = %.6f, ",rxlat/1e7,rxlon/1e7,rxheight/1e3);
	Serial.printf("height = %.1f\r\n",rxheight/1e3);
#endif
}

void UnpackNAVSTATUS(uint8_t *buffer)
{
#if (DEBUG>1)
	Serial.println("NAV-STATUS");
#endif
	
	iTOW=*((uint32_t *)(buffer+6));
	gpsFix=*(buffer+10);
	flags=*(buffer+11);
	fixStat=*(buffer+12);
	flags2=*(buffer+13);
	ttff=*((uint32_t *)(buffer+14));
	msss=*((uint32_t *)(buffer+18));
	
#if (DEBUG>2)
	if(gpsFix==0x00)		Serial.print("No Fix\r\n");
	else if(gpsFix==0x02)	Serial.print("2D Fix\r\n");
	else if(gpsFix==0x03)	Serial.print("3D Fix\r\n");
#endif
}

void UnpackNAVTIMEUTC(uint8_t *buffer)
{
#if (DEBUG>1)
	Serial.println("NAV-TIMEUTC");
#endif
	
	iTOW=*((uint32_t *)(buffer+6));
	
	gps_year=*((uint16_t *)(buffer+18));
	gps_month=*(buffer+20);
	gps_day=*(buffer+21);
	gps_hour=*(buffer+22);
	gps_min=*(buffer+23);
	gps_sec=*(buffer+24);
	
#if (DEBUG>2)
	char timebuffer[32];
	sprintf(timebuffer,"%04d/%02d/%02d %02d:%02d:%02d\r\n",gps_year,gps_month,gps_day,gps_hour,gps_min,gps_sec);
	display.print(timebuffer);	
#endif
}

void UnpackNAVSVINFO(uint8_t *buffer)
{
#if (DEBUG>1)
	Serial.println("\tNAV-SVINFO");
#endif
	
	iTOW=*((uint32_t *)(buffer+6));
	numCh=*(buffer+10);
	globalFlags=*(buffer+11);
	reserved2=*((uint16_t *)(buffer+12));
	
	uint8_t channels_used=0;
	
	uint8_t cnt;
	for(cnt=0;cnt<numCh;cnt++)
	{
		chn[cnt]=*(buffer+14+12*cnt);
		svid[cnt]=*(buffer+15+12*cnt);
		svflags[cnt]=*(buffer+16+12*cnt);
		quality[cnt]=*(buffer+17+12*cnt);
		cno[cnt]=*(buffer+18+12*cnt);
		elev[cnt]=*((int8_t *)(buffer+19+12*cnt));
		azim[cnt]=*((int16_t *)(buffer+20+12*cnt));
		prRes[cnt]=*((int32_t *)(buffer+22+12*cnt));
		
		if(quality[cnt]&0x01)	channels_used++;
	}
	
	numCh=channels_used;
	
#if (DEBUG>2)
	Serial.printf("\tnumCh = %d\r\n",numCh);
#endif
}

int GPSCommandHandler(uint8_t *cmd,uint16_t cmdptr)
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
		case 'p':	// position fix
					Serial.printf("Lat = %.6f, Lon = %.6f, ",rxlat/1e7,rxlon/1e7);
					Serial.printf("height = %.1f\r\n",rxheight/1e3);
					break;
		
		case 'f':	// fix status
					if(gpsFix==0x00)		Serial.println("No Fix");
					else if(gpsFix==0x02)	Serial.println("2D Fix");
					else if(gpsFix==0x03)	Serial.println("3D Fix");
					break;
		
		case 's':	// satellite info
					Serial.print("Chan\tPRN\tElev\tAzim\tC/No\r\n");
					for(cnt=0;cnt<numCh;cnt++)
					{
						Serial.print(cnt);	Serial.print("\t"); Serial.print(svid[cnt]);	Serial.print("\t");	Serial.print(elev[cnt]);	Serial.print("\t");	Serial.print(azim[cnt]);	Serial.print("\t");	Serial.println(cno[cnt]);
					}
					
					break;
		
		case 'l':	// live mode toggle
					gps_live_mode=!gps_live_mode;
					break;
		
		case '?':	Serial.print("GPS Test Harness\r\n================\r\n\n");
					Serial.print("p\t-\tCheck positon\r\n");
					Serial.print("f\t-\tCheck fix status\r\n");
					Serial.print("s\t-\tCheck satellite status\r\n");
					Serial.print("l\t-\tLive GPS data on/off\r\n");
					Serial.print("?\t-\tShow this menu\r\n");
					break;
		
		default:	retval=0;
					break;
	}
	
	return(retval);
}
