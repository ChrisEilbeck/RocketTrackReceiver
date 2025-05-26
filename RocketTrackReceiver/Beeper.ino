
bool beeper_enable=true;

int beeper_pin=4;

uint32_t BeeperPattern=0;
uint16_t BeeperRepeatCount=0;
uint8_t BeeperBitCount=0;

int SetupBeeper(void)
{
	if(beeper_enable)
	{
		pinMode(beeper_pin,OUTPUT);		
	}
	
	return(0);
}

void PollBeeper(void)
{
	if(beeper_enable)
	{
		static uint32_t NextBeepers=0;
		
		if(millis()>=NextBeepers)
		{
			digitalWrite(beeper_pin,(BeeperPattern>>31)&0x01);
		
			if(BeeperRepeatCount>0)
			{
				BeeperPattern=(BeeperPattern<<1)|(BeeperPattern>>31);
			}
			else
			{
				BeeperPattern=(BeeperPattern<<1);
			}
			
			BeeperBitCount++;
			if(BeeperBitCount>=32)
			{
				if(BeeperRepeatCount>0)	BeeperRepeatCount--;
				BeeperBitCount=0;
			}
						
			// run the beeper on a 20Hz cycle
			NextBeepers=millis()+50L;
		}
	}
}

void BeeperSetPattern(uint32_t pattern,int repeats)
{
	BeeperPattern=pattern;
	BeeperRepeatCount=repeats;
	BeeperBitCount=0;
}

int BeeperCommandHandler(uint8_t *cmd,uint16_t cmdptr)
{
	// ignore a single key stroke
	if(cmdptr<=2)	return(0);

#if (DEBUG>0)
	Serial.println((char *)cmd);
#endif
	
	int retval=1;
	
	switch(cmd[1]|0x20)
	{
		case 'd':	Serial.println("Rx beep disabled");
					beeper_enable=false;
					retval=0;
					break;
		
		case 'e':	Serial.println("Rx beep enabled");
					beeper_enable=true;
					retval=0;
					break;
		
		case 't':	Serial.print("Triggering Beeper Test pattern\r\n");
					BeeperSetPattern(0b1110111011100011101011100000000,0);
					retval=0;
					break;
		
		case '?':	Serial.print("Beeper Test Harness\r\n========================\r\n\n");
					Serial.print("d\t-\tRx beep disable\r\n");
					Serial.print("e\t-\tRx beep enable\r\n");
					Serial.print("t\t-\tTrigger Beeper Test Pattern\r\n");
					Serial.print("?\t-\tShow this menu\r\n");		
					retval=0;
					break;
		
		default:	// ignore
					break;
	}
	
	return(retval);
}

