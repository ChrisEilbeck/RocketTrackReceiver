
#include "Logging.h"

#define LORA_ID 0

// these are global variables to be populated from the last packet received
// and picked up by the web server

#if 0
	// St Ives Lighthouse on Smeaton's Pier
	float beaconlat=50.213461*1e7;
	float beaconlon=-5.476708*1e7;
	float beaconheight=123.0*1e3;
#endif

// Bredon Hill
fix lastfix={1,3,6,13,-63,0,0,0,52.059956,-2.064869,500.0,1.23,3.99,1234,0};

fix rxfix={0,0,0,0,0,0,0,0,0.0,0.0,0.0,0,0,0,0};

void PackPacket(uint8_t *TxPacket,uint16_t *TxPacketLength)
{
	Serial.print("PackPacket()\r\n");
	
	static uint16_t packetcounter=0;
	uint8_t packet[16];
	
	Serial.printf("TX latitude = %.6f\r\n",lastfix.latitude);
	Serial.printf("TX longitude = %.6f\r\n",lastfix.longitude);
	
#if 0
	float latitude=lastfix.latitude/1e7;
	float longitude=lastfix.longitude/1e7;
	int32_t packedlat=*(int32_t *)&latitude;
	int32_t packedlon=*(int32_t *)&longitude;
#else
	int32_t packedlat=(int32_t)(131072.0*lastfix.latitude/1e7);
	int32_t packedlon=(int32_t)(131072.0*lastfix.longitude/1e7);
#endif
	
	Serial.printf("packedlat = %d\r\n",packedlat);
	Serial.printf("packedlon = %d\r\n",packedlon);
	
	double hght=(double)lastfix.height/1e3;
	int16_t packed_height=(int16_t)hght;
	
	int cnt;
	uint8_t numsats=0;
	
	for(cnt=0;cnt<MAX_CHANNELS;cnt++)
		if(cno[cnt]>0)
			numsats++;
	
	packet[0]=LORA_ID;	// just an id value
	
	packet[1]=lastfix.numsats;
	packet[1]|=(lastfix.gpsfix&0x03)<<6;
	
	packet[2]=(packedlon&0x000000ff)>>0;	packet[3]=(packedlon&0x0000ff00)>>8;	packet[4]=(packedlon&0x00ff0000)>>16;	packet[5]=(packedlon&0xff000000)>>24;
	packet[6]=(packedlat&0x000000ff)>>0;	packet[7]=(packedlat&0x0000ff00)>>8;	packet[8]=(packedlat&0x00ff0000)>>16;	packet[9]=(packedlat&0xff000000)>>24;
	
	packet[10]=(packed_height&0x00ff)>>0;
	packet[11]=(packed_height&0xff00)>>8;
	
	// horizontal accuracy estimate from NAV-POSLLH message in units of 0.5m
	packet[12]=lastfix.accuracy;
	
	// battery voltage divided by 20 so 4250 would read as a 212 count, already scaled in PMIC.ino
	packet[13]=lastfix.voltage;
	
	packet[14]=(packetcounter&0x00ff)>>0;
	packet[15]=(packetcounter&0xff00)>>8;
	
	memcpy(TxPacket,packet,16);
	*TxPacketLength=16;
	
	packetcounter++;
}

void UnpackPacket(uint8_t *RxPacket,uint16_t RxPacketLength)
{
	Serial.print("UnpackPacket()\r\n");
	
	lastfix.id=RxPacket[0];
	
	lastfix.numsats=RxPacket[1]&0x03f;
	lastfix.gpsfix=(RxPacket[1]&0xc0)>>6;
	
	int32_t longitude=RxPacket[2]+(RxPacket[3]<<8)+(RxPacket[4]<<16)+(RxPacket[5]<<24);
	lastfix.longitude=(float)longitude/131072.0;
	
	int32_t latitude=RxPacket[6]+(RxPacket[7]<<8)+(RxPacket[8]<<16)+(RxPacket[9]<<24);
	lastfix.latitude=(float)latitude/131072.0;
	
	int height=RxPacket[10]+(RxPacket[11]<<8);
	lastfix.height=(float)height;
	
	lastfix.accuracy=(float)(RxPacket[12]<1);
	
	lastfix.voltage=(float)(RxPacket[13]*20);
	
	lastfix.counter=RxPacket[14]+(RxPacket[15]<<8);
	
#if 1
	Serial.printf("Beacon ID:\t%d\r\n",lastfix.id);
	Serial.printf("Beacon Latitude:\t%.6f\r\n",lastfix.latitude);
	Serial.printf("Beacon Longitude:\t%.6f\r\n",lastfix.longitude);
	Serial.printf("Beacon Hght:\t%.6f\r\n",lastfix.height);
#endif
}

void DumpHexPacket(uint8_t *packet,uint16_t packetlength)
{
	int cnt;
	for(cnt=0;cnt<packetlength;cnt++)
		Serial.printf("%02x",packet[cnt]);
	
	Serial.print("\r\n");
}

