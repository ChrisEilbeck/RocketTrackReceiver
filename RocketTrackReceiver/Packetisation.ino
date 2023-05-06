
#define LORA_ID 0

// these are global variables to be populated from the last packet received
// and picked up by the web server

uint8_t beaconid=0;
float beaconlat=52.2;
float beaconlon=-2.21;
float beaconheight=250.0;
float beaconvoltage=4.200;
float beaconhacc=1.23;
uint8_t beaconnumsats=6;
uint8_t beacongpsfix=3;
uint16_t beaconcount=123;

void PackPacket(uint8_t *TxPacket,uint16_t *TxPacketLength)
{
	static uint16_t packetcounter=0;
	uint8_t packet[16];
	
	double longitude=(double)rxlon/1e7;
	double latitude=(double)rxlat/1e7;
	double hght=(double)rxhMSL/1e3;
	
	uint32_t packed_longitude=(uint32_t)(longitude*131072.0);
	uint32_t packed_latitude=(uint32_t)(latitude*131072.0);
	int16_t packed_height=(int16_t)hght;
	
	int cnt;
	uint8_t numsats=0;
	
	for(cnt=0;cnt<MAX_CHANNELS;cnt++)
	{
		if(cno[cnt]>0)	numsats++;
	}
	
	packet[0]=LORA_ID;	// just an id value
	
	packet[1]=numsats;
	packet[1]|=(gpsFix&0x03)<<6;
	
	packet[2]=(rxlon&0x000000ff)>>0;	packet[3]=(rxlon&0x0000ff00)>>8;	packet[4]=(rxlon&0x00ff0000)>>16;	packet[5]=(rxlon&0xff000000)>>24;
	packet[6]=(rxlat&0x000000ff)>>0;	packet[7]=(rxlat&0x0000ff00)>>8;	packet[8]=(rxlat&0x00ff0000)>>16;	packet[9]=(rxlat&0xff000000)>>24;
	
	packet[10]=(packed_height&0x00ff)>>0;
	packet[11]=(packed_height&0xff00)>>8;
	
	// horizontal accuracy estimate from NAV-POSLLH message in units of 0.5m
	packet[12]=hAccValue;
	
	// battery voltage divided by 20 so 4250 would read as a 212 count, already scaled in PMIC.ino
	packet[13]=batvolt;
	
	packet[14]=(packetcounter&0x00ff)>>0;
	packet[15]=(packetcounter&0xff00)>>8;
	
	memcpy(TxPacket,packet,16);
	*TxPacketLength=16;
	
	packetcounter++;
}

void UnpackPacket(uint8_t *RxPacket,uint16_t RxPacketLength)
{
	beaconid=RxPacket[0];
	
	beaconnumsats=RxPacket[1]&0x03f;
	beacongpsfix=(RxPacket[1]&0xc0)>>6;
	
	int32_t longitude=RxPacket[2]+(RxPacket[3]<<8)+(RxPacket[4]<<16)+(RxPacket[5]<<24);
	beaconlon=(float)longitude/1e7;
	
	int32_t latitude=RxPacket[6]+(RxPacket[7]<<8)+(RxPacket[8]<<16)+(RxPacket[9]<<24);
	beaconlat=(float)latitude/1e7;
	
	int height=RxPacket[10]+(RxPacket[11]<<8);
	beaconheight=(float)height;
	
	beaconhacc=(float)(RxPacket[12]<1);
	
	beaconvoltage=(float)(RxPacket[13]*20);
	
	beaconcount=RxPacket[14]+(RxPacket[15]<<8);
}

void DumpHexPacket(uint8_t *packet,uint16_t packetlength)
{
	int cnt;
	for(cnt=0;cnt<packetlength;cnt++)
		Serial.printf("%02x",packet[cnt]);
	
	Serial.print("\r\n");
}

