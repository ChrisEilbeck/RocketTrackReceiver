
#define LORA_ID 0

uint8_t beaconid=0;
float beaconlat=52.2;
float beaconlon=-2.21;
float beaconheight=250.0;
float beaconvoltage=4.200;
float beaconhacc=1.23;

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

void UnpackPacket(void)
{
	
	
	
	
}

