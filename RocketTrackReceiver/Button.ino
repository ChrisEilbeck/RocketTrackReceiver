
#include "Button.h"

bool button_enable=true;
bool button_live=false;

int button_pin=38;	// for a 3-button T-Beam

int SetupButton(void)
{
	if(button_enable)
	{
		pinMode(button_pin,INPUT);
	}
	
	return(0);
}

int PollButton(void)
{
	int ButtonEvent;
	
	if(button_enable)
	{
		static uint32_t NextButton=0L;
		static uint32_t LastButtonDownTime=0L;
		static bool LastButton=true;
		
		if(millis()>=NextButton)
		{
			bool button=digitalRead(button_pin);
			
			if(button_live)
			{
				Serial.print("Button = ");	Serial.println(button);
			}
						
			if(LastButton&&!button)
			{
				// button has been pressed
				// record the time it was pressed
				LastButtonDownTime=millis();
			}
			
			ButtonEvent=BUTTON_NO_EVENT;
			
			if(!LastButton&&button)
			{
				// button has been released
				
				uint32_t ButtonPressTime=millis()-LastButtonDownTime;
				
				if(ButtonPressTime<10000)
				{
					Serial.println("\tShort button press");
					ButtonEvent=BUTTON_SHORT_PRESS;
				}
				else if(ButtonPressTime>=10000&&ButtonPressTime<30000)
				{
					Serial.println("\tLong button press");
					ButtonPressTime=BUTTON_LONG_PRESS;
				}
				else
				{
					Serial.println("\tReally long button press");
					ButtonPressTime=BUTTON_REALLY_LONG_PRESS;
				}
			}
						
			LastButton=button;
			
			// run the button on a 20Hz cycle
			NextButton=millis()+50L;
		}
	}
	
	return(ButtonEvent);
}

int ButtonCommandHandler(uint8_t *cmd,uint16_t cmdptr)
{
	// ignore a single key stroke
	if(cmdptr<=2)	return(0);

#if (DEBUG>0)
	Serial.println((char *)cmd);
#endif
	
	int retval=1;
	
	switch(cmd[1]|0x20)
	{
		case 't':	
					retval=0;
					break;
			
		default:	// ignore
					break;
	}
	
	return(retval);

}

