
#define DEBUG 0

#include "network_creds.h"
#include "Packetisation.h"
#include "Webserver.h"

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>	

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String statusprocessor(const String& var)
{
//	Serial.println("webserver process entry");
	
	char buffer[256];
	memset(buffer,0,sizeof(buffer));
	
	if(var=="LORA_MODE")
	{
		if(lora_mode==1)	sprintf(buffer,"High Rate");	else	sprintf(buffer,"Long Range");
	}
	else if(var=="BAT_STATUS")
	{
		if(axp.isChargeingEnable())
			sprintf(buffer,"\"Charging\"");
		else
			sprintf(buffer,"\"Discharging\"");
	}
	else if(var=="BAT_CURRENT")
	{
		sprintf(buffer,"%.1f",axp.getBattChargeCurrent()/1e3);
	}
	else if(var=="BAT_VOLTAGE")
	{
		sprintf(buffer,"%.3f",axp.getBattVoltage()/1000);
	}
	else if(var=="LATITUDE")
	{
		if(rxlat>0)	sprintf(buffer,"%2.6f N",rxlat/1e7);	else	sprintf(buffer,"%2.6f S",-rxlat/1e7);
	}
	else if(var=="LONGITUDE")
	{
		if(rxlon>0)	sprintf(buffer,"%3.6f E",rxlon/1e7);	else	sprintf(buffer,"%3.6f W",-rxlon/1e7);
	}
	else if(var=="ALTITUDE")
	{
		sprintf(buffer,"%.1f",rxheight/1e3);
	}
	else if(var=="NUM_CHANNELS")
	{
		sprintf(buffer,"%d",numCh);
	}
	else if(var=="GPS_FIX")
	{
		sprintf(buffer,"%d",gpsFix);
	}
	else if(var=="HORIZONTAL_ACCURACY")
	{
		sprintf(buffer,"%.1f",rxhAcc);
	}
	else if(var=="SAT_NUMS")
	{
		int cnt=0;
		for(cnt=0;cnt<numCh;cnt++)
		{
			if(cnt!=(numCh-1))
				sprintf(buffer+strlen(buffer),"%d,",svid[cnt]);
			else
				sprintf(buffer+strlen(buffer),"%d",svid[cnt]);
		}
	}
	else if(var=="SAT_ELEVS")
	{
		int cnt=0;
		for(cnt=0;cnt<numCh;cnt++)
		{
			if(cnt!=(numCh-1))
				sprintf(buffer+strlen(buffer),"%d,",elev[cnt]);
			else
				sprintf(buffer+strlen(buffer),"%d",elev[cnt]);
		}
	}
	else if(var=="SAT_AZS")
	{
		int cnt=0;
		for(cnt=0;cnt<numCh;cnt++)
		{
			if(cnt!=(numCh-1))
				sprintf(buffer+strlen(buffer),"%d,",azim[cnt]);
			else
				sprintf(buffer+strlen(buffer),"%d",azim[cnt]);
		}
	}
	else if(var=="SAT_SNRS")
	{
		int cnt=0;
		for(cnt=0;cnt<numCh;cnt++)
		{
			if(cnt!=(numCh-1))
				sprintf(buffer+strlen(buffer),"%d,",cno[cnt]);
			else
				sprintf(buffer+strlen(buffer),"%d",cno[cnt]);
		}
	}
	
//	Serial.println("webserver process exit");
	
	if(strlen(buffer)>0)
		return(buffer);
	else
		return String();
}

String trackingprocessor(const String& var)
{
	char buffer[256];
	memset(buffer,0,sizeof(buffer));
	
	if(var=="LORA_MODE")
	{
		if(lora_mode==1)	sprintf(buffer,"High Rate");	else	sprintf(buffer,"Long Range");
	}
	else if(var=="BEACON_VOLTAGE")	{	sprintf(buffer,"%.3f",beaconvoltage);		}
	else if(var=="BEACON_ID")		{	sprintf(buffer,"%d",beaconid);				}
	else if(var=="BEACON_HACC")		{	sprintf(buffer,"%.3f",beaconhacc);			}
	else if(var=="LORA_FREQUENCY")	{	sprintf(buffer,"%.3f",lora_frequency/1e6);	}
	else if(var=="LORA_RSSI")		{	sprintf(buffer,"%d",lora_rssi);				}
	else if(var=="LORA_SNR")		{	sprintf(buffer,"%d",lora_snr);				}
 	else if(var=="RX_LATITUDE")		{	sprintf(buffer,"%2.6f",rxlat/1e7);			}
	else if(var=="RX_LONGITUDE")	{	sprintf(buffer,"%3.6f",rxlon/1e7);			}
	else if(var=="RX_ALTITUDE")		{	sprintf(buffer,"%.1f",rxheight/1e3);		}
	else if(var=="TX_LATITUDE")		{	sprintf(buffer,"%2.6f",beaconlat/1e7);		}
	else if(var=="TX_LONGITUDE")	{	sprintf(buffer,"%3.6f",beaconlon/1e7);		}
	else if(var=="TX_ALTITUDE")		{	sprintf(buffer,"%.1f",beaconheight/1e3);	}
	else if(var=="NUMSATS")			{	sprintf(buffer,"%d",numCh);					}
	else if(var=="RX_HEADING")		{	sprintf(buffer,"%.1f",rx_heading);			}
	
	if(strlen(buffer)>0)
		return(buffer);
	else
		return String();
}

int SetupWebServer(void)
{
	// Initialize SPIFFS
	if(!SPIFFS.begin(true))
	{
		Serial.println("An error has occurred while mounting SPIFFS");
		return(1);
	}
	
#if 0
	File root=SPIFFS.open("/");
	File file=root.openNextFile();
	
	while(file)
	{
		Serial.print("FILE: ");
		Serial.println(file.name());
		
		file=root.openNextFile();
	}
#endif
#if 0
	File status=SPIFFS.open("/status.html");
	if(!status)
	{
		Serial.println("Error opening /status.html");
	}
	else
	{
//    while(status.available())

      uint8_t buf[64];
      status.read(buf,64);
			Serial.print((char *)buf);
    		
		close(status);
	}
#endif
	
#if STATIONMODE
#if 1
	IPAddress LocalIP(192,168,0,171);
	IPAddress Gateway(192,168,0,254);
	IPAddress Subnet(255,255,255,0);
	IPAddress DNS1(212,23,3,100);
	IPAddress DNS2(212,23,6,100);
	
	WiFi.config(LocalIP,Gateway,Subnet,DNS1,DNS2);
#endif
		
	// Connect to existing Wi-Fi network
	WiFi.begin(ssid, password);
	while(WiFi.status()!=WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}
	
	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());
#else
	// Act as an Access Point

	Serial.print("Setting AP (Access Point)...");
	
	// Remove the password parameter, if you want the AP (Access Point) to be open
	WiFi.softAP(ssid,password);

	IPAddress IP=WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);
#endif
	
	if(!MDNS.begin("rocketrx"))
	{
		Serial.println("Error starting mDNS");
		return(1);
	}
	
	// Route for root / web page
	server.on("/",HTTP_GET,[](AsyncWebServerRequest *request)					{	request->redirect("/tracking.html");		});	
	
	server.on("/status.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/status.html");		});	
	server.on("/status.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/status.css");		});	
	
	server.on("/status.js",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.println("Returning modified /status.js");
		request->send(SPIFFS,"/status.js",String(),false,statusprocessor);
	});	
	
	server.on("/tracking.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/tracking.html");		});	
	server.on("/tracking.css",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/tracking.css");		});	
	
	server.on("/tracking.js",HTTP_GET,[](AsyncWebServerRequest *request)
	{
#if 0
		Serial.println("Returning modified /tracking.js");
#endif
			
		request->send(SPIFFS,"/tracking.js",String(),false,trackingprocessor);
	});	
	
#if 0
	server.on("/config.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/config.html");		});	
	server.on("/config.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/config.css");		});	
	server.on("/config.js",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/config.js");			});	
	
	server.on("/engineering.html",HTTP_GET,[](AsyncWebServerRequest *request)	{	request->send(SPIFFS,"/engineering.html");	});	
	server.on("/engineering.css",HTTP_GET,[](AsyncWebServerRequest *request)	{	request->send(SPIFFS,"/engineering.css");	});	
	server.on("/engineering.js",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/engineering.js");	});	
	
	server.on("/longrange.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
		Serial.println("Setting to Long Range mode");
		SetLoRaMode(0);
		request->redirect("/engineering.html");
	});	
	
	server.on("/highrate.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
		Serial.println("Setting to High Rate mode");
		SetLoRaMode(1);
		request->redirect("/engineering.html");
	});	
#endif
	
	// Start server
	server.begin();
	
	return(0);
}

void PollWebServer(void)
{
	
}
