// Intended for use with a TTGO T-BEAM

// Power management
#include <axp20x.h>

#include <string.h>
#include <ctype.h>
#include <SPI.h>

// RFM98

#define LORA_NSS	18	// Comment out to disable LoRa code
#define LORA_RESET	14	// Comment out ifnot connected
#define LORA_DIO0	26                
#define SCK			5	// GPIO5  -- SX1278's SCK
#define MISO		19	// GPIO19 -- SX1278's MISO
#define MOSI		27	// GPIO27 -- SX1278's MOSI

//char character;
// int LED=11;
// unsigned long LEDOff=0;

unsigned long UpdateClientAt=0;

byte currentMode=0x81;

double Frequency=434.650;
int ImplicitOrExplicit;
int ErrorCoding;
int Bandwidth;
int SpreadingFactor;
int LowDataRateOptimize;

#define REG_FIFO                    0x00
#define REG_FIFO_ADDR_PTR           0x0D 
#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_OPMODE                  0x01
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_PACKET_SNR              0x19
#define REG_PACKET_RSSI             0x1A
#define REG_RSSI_CURRENT            0x1B
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_HOP_PERIOD              0x24
#define REG_FREQ_ERROR              0x28
#define REG_DETECT_OPT              0x31
#define REG_DETECTION_THRESHOLD     0x37

// MODES

#define RF96_MODE_RX_CONTINUOUS     0x85
#define RF96_MODE_SLEEP             0x80
#define RF96_MODE_STANDBY           0x81

#define PAYLOAD_LENGTH              80

// Modem Config 1

#define EXPLICIT_MODE               0x00
#define IMPLICIT_MODE               0x01

#define ERROR_CODING_4_5            0x02
#define ERROR_CODING_4_6            0x04
#define ERROR_CODING_4_7            0x06
#define ERROR_CODING_4_8            0x08

#define BANDWIDTH_7K8               0x00
#define BANDWIDTH_10K4              0x10
#define BANDWIDTH_15K6              0x20
#define BANDWIDTH_20K8              0x30
#define BANDWIDTH_31K25             0x40
#define BANDWIDTH_41K7              0x50
#define BANDWIDTH_62K5              0x60
#define BANDWIDTH_125K              0x70
#define BANDWIDTH_250K              0x80
#define BANDWIDTH_500K              0x90

// Modem Config 2

#define SPREADING_6                 0x60
#define SPREADING_7                 0x70
#define SPREADING_8                 0x80
#define SPREADING_9                 0x90
#define SPREADING_10                0xA0
#define SPREADING_11                0xB0
#define SPREADING_12                0xC0

#define CRC_OFF                     0x00
#define CRC_ON                      0x04

// POWER AMPLIFIER CONFIG
#define REG_PA_CONFIG               0x09
#define PA_MAX_BOOST                0x8F
#define PA_LOW_BOOST                0x81
#define PA_MED_BOOST                0x8A
#define PA_MAX_UK                   0x88
#define PA_OFF_BOOST                0x00
#define RFO_MIN                     0x00

// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23  // 0010 0011
#define LNA_OFF_GAIN                0x00

AXP20X_Class axp;

void SetParametersFromLoRaMode(int LoRaMode)
{
	LowDataRateOptimize=0;
	
	if(LoRaMode==7)			{	ImplicitOrExplicit=EXPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_5;	Bandwidth=BANDWIDTH_20K8;	SpreadingFactor=SPREADING_7;								}
	else if(LoRaMode==6)	{	ImplicitOrExplicit=IMPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_5;	Bandwidth=BANDWIDTH_41K7;	SpreadingFactor=SPREADING_6;								}
	else if(LoRaMode==5)	{	ImplicitOrExplicit=EXPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_8;	Bandwidth=BANDWIDTH_41K7;	SpreadingFactor=SPREADING_11;								}
	else if(LoRaMode==4)	{	ImplicitOrExplicit=IMPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_5;	Bandwidth=BANDWIDTH_250K;	SpreadingFactor=SPREADING_6;								}
	else if(LoRaMode==3)	{	ImplicitOrExplicit=EXPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_6;	Bandwidth=BANDWIDTH_250K;	SpreadingFactor=SPREADING_7;								}
	else if(LoRaMode==2)	{	ImplicitOrExplicit=EXPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_8;	Bandwidth=BANDWIDTH_62K5;	SpreadingFactor=SPREADING_8;								}
	else if(LoRaMode==1)	{	ImplicitOrExplicit=IMPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_5;	Bandwidth=BANDWIDTH_20K8;	SpreadingFactor=SPREADING_6;								}
	else if(LoRaMode==0)	{	ImplicitOrExplicit=EXPLICIT_MODE;	ErrorCoding=ERROR_CODING_4_8;	Bandwidth=BANDWIDTH_20K8;	SpreadingFactor=SPREADING_11;	LowDataRateOptimize=0x08;	}
}

// initialize the library with the numbers of the interface pins

void setup()
{
	Serial.begin(115200);
	
	Wire.begin(21, 22);
	
	if(!axp.begin(Wire, AXP192_SLAVE_ADDRESS))	{	Serial.println("AXP192 Begin PASS");	}
	else										{	Serial.println("AXP192 Begin FAIL");	}
	
	axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
	axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
	axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
	axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
	axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
	axp.setDCDC1Voltage(3300);
	
	Serial.printf("DCDC1: %s", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");	Serial.println("");
	Serial.printf("DCDC2: %s", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");	Serial.println("");
	Serial.printf("LDO2: %s", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");	Serial.println("");
	Serial.printf("LDO3: %s", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");	Serial.println("");
	Serial.printf("DCDC3: %s", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");	Serial.println("");
	Serial.printf("Exten: %s", axp.isExtenEnable() ? "ENABLE" : "DISABLE");	Serial.println("");
	
	if(axp.isChargeing())	{	Serial.println("Charging");	}  
	
//	pinMode(LED, OUTPUT);
//	digitalWrite(LED, 1);
	
	Serial.println("");
	Serial.println("ESP32 LoRa Receiver V1.2");
	Serial.println("");
	
	SetParametersFromLoRaMode(0);
	
	setupRFM98();
	
	SetParametersFromLoRaMode(0);
	
//	digitalWrite(LED, 0);
}

void loop()
{
	CheckPC();
	CheckRx();
	UpdateClient();
}

void UpdateClient(void)
{
	if(millis()>=UpdateClientAt)
	{
		int CurrentRSSI;
		char Line[20];
		
		if(Frequency>525)	{	CurrentRSSI=readRegister(REG_RSSI_CURRENT)-157;	}
		else				{	CurrentRSSI=readRegister(REG_RSSI_CURRENT)-164;	}

 		sprintf(Line, "CurrentRSSI=%d\r\n", CurrentRSSI);
 		SendToHosts(Line);
		
		UpdateClientAt=millis()+1000;
	}
}
  
void SendToHosts(char *Line)
{
	Serial.print(Line);
}

double FrequencyReference(void)
{
	switch (Bandwidth)
	{
		case  BANDWIDTH_7K8:  	return 7800;
		case  BANDWIDTH_10K4:   return 10400; 
		case  BANDWIDTH_15K6:   return 15600; 
		case  BANDWIDTH_20K8:   return 20800; 
		case  BANDWIDTH_31K25:  return 31250; 
		case  BANDWIDTH_41K7:   return 41700; 
		case  BANDWIDTH_62K5:   return 62500; 
		case  BANDWIDTH_125K:   return 125000; 
		case  BANDWIDTH_250K:   return 250000; 
		case  BANDWIDTH_500K:   return 500000; 
	}

	return 0;
}

double FrequencyError(void)
{
	int32_t Temp;
	double T;
	
	Temp=(int32_t)readRegister(REG_FREQ_ERROR) & 7;
	Temp <<= 8L;
	Temp += (int32_t)readRegister(REG_FREQ_ERROR+1);
	Temp <<= 8L;
	Temp += (int32_t)readRegister(REG_FREQ_ERROR+2);
	
	if(readRegister(REG_FREQ_ERROR) & 8)
	{
		Temp=Temp - 524288;
	}

	T=(double)Temp;
	T *=  (16777216.0 / 32000000.0);
	T *= (FrequencyReference() / 500000.0);

	return -T;
} 

int receiveMessage(unsigned char *message)
{
	int i, Bytes, currentAddr;

	int x=readRegister(REG_IRQ_FLAGS);
	Bytes=0;
	// printf("Message status=%02Xh\n", x);
	
	// clear the rxDone flag
	// writeRegister(REG_IRQ_FLAGS, 0x40); 
	writeRegister(REG_IRQ_FLAGS,0xFF); 
	
	// check for payload crc issues (0x20 is the bit we are looking for
	if((x & 0x20)==0x20)
	{
		Serial.println("CRC Failure");
		// reset the crc flags
		writeRegister(REG_IRQ_FLAGS, 0x20); 
	}
	else
	{
		Serial.println("Received");
		currentAddr=readRegister(REG_FIFO_RX_CURRENT_ADDR);
		Bytes=readRegister(REG_RX_NB_BYTES);
		// printf ("%d bytes in packet\n", Bytes);

		// printf("RSSI=%d\n", readRegister(REG_RSSI) - 137);
		
		writeRegister(REG_FIFO_ADDR_PTR, currentAddr);
		
		// now loop over the fifo getting the data
		for(i=0; i < Bytes; i++)
		{
			message[i]=(unsigned char)readRegister(REG_FIFO);
		}
		
		message[Bytes]='\0';

		// writeRegister(REG_FIFO_ADDR_PTR, 0);  // currentAddr);   
	} 
	
	return Bytes;
}

void ReplyOK(void)
{
	SendToHosts("*");
}

void ReplyBad(void)
{
	SendToHosts("?");
}

void SetFrequency(char *Line)
{
	double Freq=atof(Line);
	char Output[20];
	
	if(Freq>0)	{	ReplyOK();	Frequency=Freq;	sprintf(Output,"Frequency=%.3lf\r\n", Frequency);	SendToHosts(Output);	startReceiving();	}
	else		{	ReplyBad();																													}
}

void SetMode(char *Line)
{
	int Mode=atoi(Line);
	char Output[20];
	
	if((Mode>=0)&&(Mode<=7))	{	ReplyOK();	SetParametersFromLoRaMode(Mode);	sprintf(Output,"Mode=%d\r\n", Mode);	SendToHosts(Output);	startReceiving();	}
	else						{	ReplyBad();																															}
}

void SetBandwidth(char *Line)
{
	if(strcmp(Line,"7K8")==0)			{	ReplyOK();	Bandwidth=BANDWIDTH_7K8;	startReceiving();	}
	else if(strcmp(Line,"10K4")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_10K4;	startReceiving();	}
	else if(strcmp(Line,"15K6")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_15K6;	startReceiving();	}
	else if(strcmp(Line,"20K8")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_20K8;	startReceiving();	}
	else if(strcmp(Line,"31K25")==0)	{	ReplyOK();	Bandwidth=BANDWIDTH_31K25;	startReceiving();	}
	else if(strcmp(Line,"41K7")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_41K7;	startReceiving();	}
	else if(strcmp(Line,"62K5")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_62K5;	startReceiving();	}
	else if(strcmp(Line,"125K")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_125K;	startReceiving();	}
	else if(strcmp(Line,"250K")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_250K;	startReceiving();	}
	else if(strcmp(Line,"500K")==0)		{	ReplyOK();	Bandwidth=BANDWIDTH_500K;	startReceiving();	}
	else								{	ReplyBad();													}
}

void SetErrorCoding(char *Line)
{
	int Coding=atoi(Line);
	
	if((Coding>=5)&&(Coding<=8))		{	ReplyOK();	ErrorCoding=(Coding-4)<<1;	startReceiving();	}
	else								{	ReplyBad();													}
}

void SetSpreadingFactor(char *Line)
{
	int Spread=atoi(Line);

	if((Spread>=6)&&(Spread<=12))		{	ReplyOK();	SpreadingFactor=Spread<<4;	startReceiving();	}
	else								{	ReplyBad();													}
}

void SetImplicit(char *Line)
{
	int Implicit=atoi(Line);

	ReplyOK();
	ImplicitOrExplicit=Implicit?IMPLICIT_MODE:EXPLICIT_MODE;
	startReceiving();
}

void SetLowOpt(char *Line)
{
	int LowOpt=atoi(Line);

	ReplyOK();
	ImplicitOrExplicit=LowOpt?0x08:0;
	startReceiving();
}

void ProcessCommand(char *Line)
{
	char Command;
	
	Command=Line[1];
	Line+=2;
	
	if(Command=='F')		{	SetFrequency(Line);			}
	else if(Command=='M')	{	SetMode(Line);				}
	else if(Command=='B')	{	SetBandwidth(Line);			}
	else if(Command=='E')	{	SetErrorCoding(Line);		}
	else if(Command=='S')	{	SetSpreadingFactor(Line);	}
	else if(Command=='I')	{	SetImplicit(Line);			}
	else if(Command=='L')	{	SetLowOpt(Line);			}
	else					{	ReplyBad();					}
}

void CheckPC()
{
	static char Line[32];
	static int Length=0;
	char Character;

	while (Serial.available())
	{ 
		Character=Serial.read();
		
		if(Character=='~')
		{
			Line[0]=Character;
			Length=1;
		}
		else if(Length >= sizeof(Line))
		{
			Length=0;
		}
		else if(Length > 0)
		{
			if(Character=='\r')
			{
				Line[Length]='\0';
				ProcessCommand(Line);
				Length=0;
			}
			else
			{
				Line[Length++]=Character;
			}
		}
	}
}

void CheckRx()
{
//	if((LEDOff > 0) && (millis() >= LEDOff))
//	{
//		digitalWrite(LED, LOW);
//		LEDOff=0;
//	}
	
	if(digitalRead(LORA_DIO0))
	{
		Serial.println("Received message");
		
		unsigned char Message[256];
		char Line[20];
		int Bytes, SNR, RSSI, i;
		long Altitude;

//		digitalWrite(LED, 1);
//		LEDOff=millis() + 1000;
		
		Bytes=receiveMessage(Message);
		
		sprintf(Line, "FreqErr=%.1lf\r\n", FrequencyError()/1000.0);
		SendToHosts(Line);

		SNR=readRegister(REG_PACKET_SNR);
		SNR /= 4;
		
		if(Frequency>525)	{	RSSI=readRegister(REG_PACKET_RSSI)-157;		}
		else				{	RSSI=readRegister(REG_PACKET_RSSI)-164;		}
		
		if(SNR<0)			{	RSSI+=SNR;	}
		
		sprintf(Line, "Packet RSSI=%d\r\n",RSSI);
		SendToHosts(Line);
		sprintf(Line, "Packet SNR=%d\r\n",SNR);
		SendToHosts(Line);
		sprintf(Line,"Packet size= %d\r\n",Bytes);
		SendToHosts(Line);
		
		// Telemetry=$$LORA1,108,20:30:39,51.95027,-2.54445,00141,0,0,11*9B74
		
		if(Message[0]=='$')
		{
			char Line[256];
			
			// Remove LF
			Message[strlen((char *)Message)-1]='\0';
			
			sprintf(Line, "Message: %s\r\n", Message);
			SendToHosts(Line);
		}
		else if(Message[0]=='%')
		{
			char *ptr;
			char *ptr2;

			Message[0]='$';
			
			ptr=(char *)Message;
			
			do
			{
				if((ptr2=strchr(ptr,'\n'))!=NULL)
				{
					*ptr2='\0';
					
					SendToHosts("Message:");
					SendToHosts((char *)Message);
					SendToHosts("\r\n");
					
					ptr=ptr2+1;
				}
			}
			while (ptr2 != NULL);
		}
		else
		{
			Serial.print("Hex:");
			
			for(i=0;i<Bytes;i++)
			{
				if(Message[i]<0x10)
				{
					Serial.print("0");
				} 
				
				Serial.print(Message[i], HEX);
			}
			
			Serial.println();
		}
	}
}

void setMode(byte newMode)
{
	if(newMode==currentMode)
		return;  
	
	switch (newMode) 
	{
		case RF96_MODE_RX_CONTINUOUS:
										writeRegister(REG_PA_CONFIG,PA_OFF_BOOST);	// TURN PA OFF FOR RECIEVE??
										writeRegister(REG_LNA,LNA_MAX_GAIN);		// LNA_MAX_GAIN);  // MAX GAIN FOR RECIEVE
										writeRegister(REG_OPMODE,newMode);
										currentMode=newMode; 
										break;
		
		case RF96_MODE_SLEEP:
										writeRegister(REG_OPMODE,newMode);
										currentMode=newMode; 
										break;
										
		case RF96_MODE_STANDBY:
										writeRegister(REG_OPMODE,newMode);
										currentMode=newMode; 
										break;
		default: 
										return;
	} 
	
	if(newMode != RF96_MODE_SLEEP)
	{
		delay(10);
	}
	
	return;
}

byte readRegister(byte addr)
{
	select();
	SPI.transfer(addr&0x7F);
	byte regval=SPI.transfer(0);
	unselect();
	
	printf("RD Reg %02X=%02X\n",addr,regval);
	return regval;
}

void writeRegister(byte addr,byte value)
{
	select();
	SPI.transfer(addr|0x80);
	SPI.transfer(value);
	unselect();
	
	printf("WR Reg %02X=%02X\n",addr,value);
}

void select() 
{
	digitalWrite(LORA_NSS,LOW);
}

void unselect() 
{
	digitalWrite(LORA_NSS,HIGH);
}

void SetLoRaFrequency()
{
	unsigned long FrequencyValue;
	double Temp;
	
	Temp=Frequency*7110656/434;
	FrequencyValue=(unsigned long)(Temp);
	
	Serial.print("FrequencyValue is ");
	Serial.println(FrequencyValue);
	
	// Set frequency
	writeRegister(0x06,(FrequencyValue>>16)&0xFF);
	writeRegister(0x07,(FrequencyValue>>8)&0xFF);
	writeRegister(0x08,FrequencyValue&0xFF);
}

void SetLoRaParameters()
{
	writeRegister(REG_MODEM_CONFIG,ImplicitOrExplicit|ErrorCoding|Bandwidth);
	writeRegister(REG_MODEM_CONFIG2,SpreadingFactor|CRC_ON);
	writeRegister(REG_MODEM_CONFIG3,0x04|LowDataRateOptimize);														// 0x04: AGC sets LNA gain
	writeRegister(REG_DETECT_OPT,(readRegister(REG_DETECT_OPT)&0xF8)|((SpreadingFactor==SPREADING_6)?0x05:0x03)); 	// 0x05 For SF6; 0x03 otherwise
	writeRegister(REG_DETECTION_THRESHOLD,(SpreadingFactor==SPREADING_6)?0x0C:0x0A);								// 0x0C for SF6, 0x0A otherwise
}

void startReceiving()
{
	Serial.println("Staring continuous receive ...");
	
	setMode(RF96_MODE_SLEEP);
// 	writeRegister(REG_OPMODE,0x80);
// 	setMode(RF96_MODE_SLEEP);
	
	SetLoRaFrequency();
	SetLoRaParameters();
	
	writeRegister(REG_PAYLOAD_LENGTH,255);
	writeRegister(REG_RX_NB_BYTES,255);
	
	writeRegister(REG_FIFO_RX_BASE_AD,0);
	writeRegister(REG_FIFO_ADDR_PTR,0);
	
	writeRegister(REG_DIO_MAPPING_1,0x00);
	
	// Setup Receive Continous Mode
	setMode(RF96_MODE_RX_CONTINUOUS);
}

void setupRFM98(void)
{
	// initialize the pins
	pinMode(LORA_RESET,OUTPUT);
	digitalWrite(LORA_RESET,HIGH);
	
	delay(10);          // Module needs this before it's ready  
	
	pinMode(LORA_NSS,OUTPUT);
	pinMode(LORA_DIO0,INPUT);
	
	SPI.begin(SCK,MISO,MOSI,LORA_NSS);
	
	startReceiving();
	
	Serial.println("Setup Complete");
}
