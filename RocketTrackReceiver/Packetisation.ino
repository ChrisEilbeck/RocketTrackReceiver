
#define LORA_ID 0

// these are global variables to be populated from the last packet received
// and picked up by the web server

uint8_t beaconid=0;

#if 0
	// St Ives Lighthouse on Smeaton's Pier
	float beaconlat=50.213461*1e7;
	float beaconlon=-5.476708*1e7;
	float beaconheight=123.0*1e3;
#endif
#if 1
	// Bredon Hill
	float beaconlat=52.059956*1e7;
	float beaconlon=-2.064869*1e7;
	float beaconheight=500.0*1e3;
#endif

float beaconvoltage=4.200;
float beaconhacc=1.23;
uint8_t beaconnumsats=6;
uint8_t beacongpsfix=3;
uint16_t beaconcount=123;

void PackPacket(uint8_t *TxPacket,uint16_t *TxPacketLength)
{
	Serial.print("PackPacket()\r\n");
	
	static uint16_t packetcounter=0;
	uint8_t packet[16];
	
	Serial.printf("beaconlat = %.6f\r\n",beaconlat);
	Serial.printf("beaconlon = %.6f\r\n",beaconlon);
	
#if 0
	float latitude=beaconlat/1e7;
	float longitude=beaconlon/1e7;
	int32_t packedlat=*(int32_t *)&latitude;
	int32_t packedlon=*(int32_t *)&longitude;
#else
	int32_t packedlat=(int32_t)(131072.0*beaconlat/1e7);
	int32_t packedlon=(int32_t)(131072.0*beaconlon/1e7);
#endif
	
	Serial.printf("packedlat = %d\r\n",packedlat);
	Serial.printf("packedlon = %d\r\n",packedlon);
	
	double hght=(double)beaconheight/1e3;
	int16_t packed_height=(int16_t)hght;
	
	int cnt;
	uint8_t numsats=0;
	
	for(cnt=0;cnt<MAX_CHANNELS;cnt++)
		if(cno[cnt]>0)
			numsats++;
	
	packet[0]=LORA_ID;	// just an id value
	
	packet[1]=beaconnumsats;
	packet[1]|=(beacongpsfix&0x03)<<6;
	
	packet[2]=(packedlon&0x000000ff)>>0;	packet[3]=(packedlon&0x0000ff00)>>8;	packet[4]=(packedlon&0x00ff0000)>>16;	packet[5]=(packedlon&0xff000000)>>24;
	packet[6]=(packedlat&0x000000ff)>>0;	packet[7]=(packedlat&0x0000ff00)>>8;	packet[8]=(packedlat&0x00ff0000)>>16;	packet[9]=(packedlat&0xff000000)>>24;
	
	packet[10]=(packed_height&0x00ff)>>0;
	packet[11]=(packed_height&0xff00)>>8;
	
	// horizontal accuracy estimate from NAV-POSLLH message in units of 0.5m
	packet[12]=beaconhacc;
	
	// battery voltage divided by 20 so 4250 would read as a 212 count, already scaled in PMIC.ino
	packet[13]=beaconvoltage;
	
	packet[14]=(packetcounter&0x00ff)>>0;
	packet[15]=(packetcounter&0xff00)>>8;
	
	memcpy(TxPacket,packet,16);
	*TxPacketLength=16;
	
	packetcounter++;
}

void UnpackPacket(uint8_t *RxPacket,uint16_t RxPacketLength)
{
	Serial.print("UnpackPacket()\r\n");
	
	beaconid=RxPacket[0];
	
	beaconnumsats=RxPacket[1]&0x03f;
	beacongpsfix=(RxPacket[1]&0xc0)>>6;
	
	int32_t longitude=RxPacket[2]+(RxPacket[3]<<8)+(RxPacket[4]<<16)+(RxPacket[5]<<24);
	beaconlon=(float)longitude*1e7/131072.0;
	
	int32_t latitude=RxPacket[6]+(RxPacket[7]<<8)+(RxPacket[8]<<16)+(RxPacket[9]<<24);
	beaconlat=(float)latitude*1e7/131072.0;
	
	int height=RxPacket[10]+(RxPacket[11]<<8);
	beaconheight=(float)height*1e3;
	
	beaconhacc=(float)(RxPacket[12]<1);
	
	beaconvoltage=(float)(RxPacket[13]*20);
	
	beaconcount=RxPacket[14]+(RxPacket[15]<<8);
	
#if 1
	Serial.printf("Beacon ID:\t%d\r\n",beaconid);
	Serial.printf("Beacon Lat:\t%.6f\r\n",beaconlat/1e7);
	Serial.printf("Beacon Lon:\t%.6f\r\n",beaconlon/1e7);
	Serial.printf("Beacon Hght:\t%.6f\r\n",beaconheight/1e3);
#endif
}

void DumpHexPacket(uint8_t *packet,uint16_t packetlength)
{
	int cnt;
	for(cnt=0;cnt<packetlength;cnt++)
		Serial.printf("%02x",packet[cnt]);
	
	Serial.print("\r\n");
}

