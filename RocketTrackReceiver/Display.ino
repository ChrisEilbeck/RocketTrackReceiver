
#include "Display.h"
#include "GPS.h"
#include "GreatCircle.h"
#include "Logging.h"
#include "LoRaReceiver.h"
#include "Packetisation.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define DISPLAY_UPDATE_PERIOD	200

Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

int display_update_suspend=0;

int SetupDisplay(void)
{
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS))
	{
		Serial.println(F("SSD1306 allocation failed"));
		return(1);
	}
	
	Serial.println(F("SSD1306 display configured ..."));
	
	display.setRotation(1);
	
	display.clearDisplay();
	
	display.setTextSize(3);					// Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);	// Draw white text
	display.cp437(true);					// Use full 256 char 'Code Page 437' font
	
	display.setCursor(8,16);
	display.write("RKT");
	display.setCursor(8,48);
	display.write("Trk");
	display.setCursor(16,80);
	display.write("RX");
	
	display.display();
	
	return(0);
}

void PollDisplay(void)
{
	static int DisplayState=0;
	static int LastDisplayChange=3000;
	
	if(millis()<display_update_suspend)
		return;
	
	if(millis()>=(LastDisplayChange+DISPLAY_UPDATE_PERIOD))
	{
		display.clearDisplay();
		
		// portrait
		display.setRotation(1);
		
		// draw white on black if logging is active, inverted otherwise
		display.setTextColor(SSD1306_WHITE);
		
		display.setCursor(0,0);
		
		char buffer[32];
		
		display.setTextSize(1);
		
		sprintf(buffer,"%04d/%02d/%02d\r\n",gps_year,gps_month,gps_day);
		display.print(buffer);
		
		sprintf(buffer,"  %02d%02d%02d\r\n",gps_hour,gps_min,gps_sec);
		display.print(buffer);
		
		switch(DisplayState)
		{
			case 0 ... 8:	display.setTextSize(1);
							display.print("\nReceiver\r\n\n");
							display.printf("Lat:\r\n %.6f\r\n",rxfix.latitude);
							display.printf("Lon:\r\n %.6f\r\n",rxfix.longitude);		
							display.printf("Altitude:\r\n %.1f m\r\n",rxfix.height);							
							break;
			
			case 9 ... 15:	display.setTextSize(1);
							display.print("\nReceiver\r\n\n# Sats:\r\n\n  ");
							display.setTextSize(2);
							display.println(numCh);
							break;
			
			case 16 ... 24:	display.setTextSize(1);
							display.printf("\nBeacon %d\r\n\n",lastfix.id);
							display.printf("Lat:\r\n %.6f\r\n",lastfix.latitude);
							display.printf("Lon:\r\n %.6f\r\n",lastfix.longitude);		
							display.printf("Altitude:\r\n %.1f m\r\n",lastfix.height);
							break;
			
			case 25 ... 29:	display.setTextSize(1);
							display.printf("\nBeacon %d\r\n\n# Sats:\r\n\n  ",lastfix.id);
							display.setTextSize(2);
							display.println(lastfix.numsats);
							break;
			
			case 30 ... 39:	display.setTextSize(1);
							display.printf("\nBeacon %d\r\n\nBearing:\r\n\n",lastfix.id);
							display.setTextSize(2);
							display.printf("%.1f\r\n  deg",GreatCircleBearing(lastfix.latitude,lastfix.longitude,rxfix.latitude,rxfix.longitude));
							break;
			
			case 40 ... 49:	display.setTextSize(1);
							display.printf("\nBeacon %d\r\n\nRange:\r\n\n",lastfix.id);
							display.setTextSize(2);
														
							if(GreatCircleDistance(lastfix.latitude,lastfix.longitude,rxfix.latitude,rxfix.longitude)<2000)
								display.printf("%.1f\n    m",GreatCircleDistance(lastfix.latitude,lastfix.longitude,rxfix.latitude,rxfix.longitude));
							else
								display.printf("%.1f\n   km",GreatCircleDistance(lastfix.latitude,lastfix.longitude,rxfix.latitude,rxfix.longitude)/1000);
							
							break;
			
			case 50 ... 59:	display.setTextSize(1);
							display.printf("\nReceiver\r\n\nHeading:\r\n\n");
							display.setTextSize(2);
							display.printf("%.1f\r\n  deg",get_compass_bearing());
							break;
			
			default:		
							DisplayState=0;
							break;
		}
		
		display.setTextSize(2);
		display.setCursor(0,104);
		if(gpsFix==3)		display.print("3d ");
		else if(gpsFix==2)	display.print("2d ");
		else				display.print("No ");
		
		display.print(LoRaGetMode()==LORA_LONG_RANGE_MODE?"LR":"HR");
		
		display.display();
		
		DisplayState++;
		
		LastDisplayChange=millis();
	}
}

