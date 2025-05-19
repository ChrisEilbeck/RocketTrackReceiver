
#define DEBUG 0

//#include "network_creds.h"
#include "Logging.h"
#include "LoRaReceiver.h"
#include "Packetisation.h"
#include "Webserver.h"

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>	

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

char ssid[32]="RocketRx";
char password[32]="marsflightcrew";

bool track_compass=true;

char jsonstring[1024];

String magprocessor(const String& var)
{
#if DEBUG>2
	Serial.println("magprocessor() entry");
#endif
	
	char buffer[256];
	memset(buffer,0,sizeof(buffer));
	
	if(var=="MAG_A11")		sprintf(buffer,"%.6f",Mag_A11);
	else if(var=="MAG_A12")	sprintf(buffer,"%.6f",Mag_A12);
	else if(var=="MAG_A13")	sprintf(buffer,"%.6f",Mag_A13);
	else if(var=="MAG_A21")	sprintf(buffer,"%.6f",Mag_A21);
	else if(var=="MAG_A22")	sprintf(buffer,"%.6f",Mag_A22);
	else if(var=="MAG_A23")	sprintf(buffer,"%.6f",Mag_A23);
	else if(var=="MAG_A31")	sprintf(buffer,"%.6f",Mag_A31);
	else if(var=="MAG_A32")	sprintf(buffer,"%.6f",Mag_A32);
	else if(var=="MAG_A33")	sprintf(buffer,"%.6f",Mag_A33);
	else if(var=="MAG_B1")	sprintf(buffer,"%.6f",Mag_B1);
	else if(var=="MAG_B2")	sprintf(buffer,"%.6f",Mag_B2);
	else if(var=="MAG_B3")	sprintf(buffer,"%.6f",Mag_B3);

#if DBEUG>2
	Serial.println("magprocessor() exit");
#endif
	
	if(strlen(buffer)>0)	return(buffer);
	else					return(String());
}

String chanprocessor(const String& var)
{
#if DEBUG>2
	Serial.println("chanprocessor() entry");
#endif
	
	char buffer[256];
	memset(buffer,0,sizeof(buffer));

	if(var=="CH1_FREQ")			sprintf(buffer,"%3.3f",LORA_CH1);
	else if(var=="CH2_FREQ")	sprintf(buffer,"%3.3f",LORA_CH2);
	else if(var=="CH3_FREQ")	sprintf(buffer,"%3.3f",LORA_CH3);
	else if(var=="CH4_FREQ")	sprintf(buffer,"%3.3f",LORA_CH4);
	else if(var=="CH5_FREQ")	sprintf(buffer,"%3.3f",LORA_CH5);
	else if(var=="CH6_FREQ")	sprintf(buffer,"%3.3f",LORA_CH6);

	else if((var=="CH1_CHECKED")&&(fabs(lora_freq-LORA_CH1*1e6)<10e3))	sprintf(buffer,"checked");
	else if((var=="CH2_CHECKED")&&(fabs(lora_freq-LORA_CH2*1e6)<10e3))	sprintf(buffer,"checked");
	else if((var=="CH3_CHECKED")&&(fabs(lora_freq-LORA_CH3*1e6)<10e3))	sprintf(buffer,"checked");
	else if((var=="CH4_CHECKED")&&(fabs(lora_freq-LORA_CH4*1e6)<10e3))	sprintf(buffer,"checked");
	else if((var=="CH5_CHECKED")&&(fabs(lora_freq-LORA_CH5*1e6)<10e3))	sprintf(buffer,"checked");
	else if((var=="CH6_CHECKED")&&(fabs(lora_freq-LORA_CH6*1e6)<10e3))	sprintf(buffer,"checked");
		
#if DBEUG>2
	Serial.println("chanprocessor() exit");
#endif
	
	if(strlen(buffer)>0)	return(buffer);
	else					return(String());
}

String loraprocessor(const String& var)
{
#if DEBUG>2
	Serial.println("loraprocessor() entry");
#endif
	
	char buffer[256];
	memset(buffer,0,sizeof(buffer));
	
	if((var=="HR10.4K")&&(hr_bw==10400))			sprintf(buffer,"checked");
	else if((var=="HR15.6K")&&(hr_bw==15600))		sprintf(buffer,"checked");
	else if((var=="HR20.8K")&&(hr_bw==20800))		sprintf(buffer,"checked");
	else if((var=="HR31.25K")&&(hr_bw==31250))		sprintf(buffer,"checked");
	else if((var=="HR41.7K")&&(hr_bw==41700))		sprintf(buffer,"checked");
	else if((var=="HR62.5K")&&(hr_bw==62500))		sprintf(buffer,"checked");
	else if((var=="HR125K")&&(hr_bw==125000))		sprintf(buffer,"checked");
	else if((var=="HR250K")&&(hr_bw==250000))		sprintf(buffer,"checked");
	
	else if((var=="HRSF6")&&(hr_sf==6))				sprintf(buffer,"checked");
	else if((var=="HRSF7")&&(hr_sf==7))				sprintf(buffer,"checked");
	else if((var=="HRSF8")&&(hr_sf==8))				sprintf(buffer,"checked");
	else if((var=="HRSF9")&&(hr_sf==9))				sprintf(buffer,"checked");
	else if((var=="HRSF10")&&(hr_sf==10))			sprintf(buffer,"checked");
	else if((var=="HRSF11")&&(hr_sf==11))			sprintf(buffer,"checked");
	else if((var=="HRSF12")&&(hr_sf==12))			sprintf(buffer,"checked");
	
	else if((var=="HRCR4")&&(hr_cr==4))				sprintf(buffer,"checked");
	else if((var=="HRCR5")&&(hr_cr==5))				sprintf(buffer,"checked");
	else if((var=="HRCR6")&&(hr_cr==6))				sprintf(buffer,"checked");
	else if((var=="HRCR7")&&(hr_cr==7))				sprintf(buffer,"checked");
	else if((var=="HRCR8")&&(hr_cr==8))				sprintf(buffer,"checked");
	
	else if((var=="LR10.4K")&&(lr_bw==10400))		sprintf(buffer,"checked");
	else if((var=="LR15.6K")&&(lr_bw==15600))		sprintf(buffer,"checked");
	else if((var=="LR20.8K")&&(lr_bw==20800))		sprintf(buffer,"checked");
	else if((var=="LR31.25K")&&(lr_bw==31250))		sprintf(buffer,"checked");
	else if((var=="LR41.7K")&&(lr_bw==41700))		sprintf(buffer,"checked");
	else if((var=="LR62.5K")&&(lr_bw==62500))		sprintf(buffer,"checked");
	else if((var=="LR125K")&&(lr_bw==125000))		sprintf(buffer,"checked");
	else if((var=="LR250K")&&(lr_bw==250000))		sprintf(buffer,"checked");
	
	else if((var=="LRSF6")&&(lr_sf==6))				sprintf(buffer,"checked");
	else if((var=="LRSF7")&&(lr_sf==7))				sprintf(buffer,"checked");
	else if((var=="LRSF8")&&(lr_sf==8))				sprintf(buffer,"checked");
	else if((var=="LRSF9")&&(lr_sf==9))				sprintf(buffer,"checked");
	else if((var=="LRSF10")&&(lr_sf==10))			sprintf(buffer,"checked");
	else if((var=="LRSF11")&&(lr_sf==11))			sprintf(buffer,"checked");
	else if((var=="LRSF12")&&(lr_sf==12))			sprintf(buffer,"checked");
	
	else if((var=="LRCR4")&&(lr_cr==4))				sprintf(buffer,"checked");
	else if((var=="LRCR5")&&(lr_cr==5))				sprintf(buffer,"checked");
	else if((var=="LRCR6")&&(lr_cr==6))				sprintf(buffer,"checked");
	else if((var=="LRCR7")&&(lr_cr==7))				sprintf(buffer,"checked");
	else if((var=="LRCR8")&&(lr_cr==8))				sprintf(buffer,"checked");
	
#if DBEUG>2
	Serial.println("loraprocessor() exit");
#endif
	
	if(strlen(buffer)>0)	return(buffer);
	else					return(String());
}

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
		if(rxlat>0)	sprintf(buffer,"%2.6f N",rxlat);	else	sprintf(buffer,"%2.6f S",-rxlat);
	}
	else if(var=="LONGITUDE")
	{
		if(rxlon>0)	sprintf(buffer,"%3.6f E",rxlon);	else	sprintf(buffer,"%3.6f W",-rxlon);
	}
	else if(var=="ALTITUDE")
	{
		sprintf(buffer,"%.1f",rxheight);
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
	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	
	if(var=="LORA_MODE")
	{
		if(lora_mode==1)	sprintf(buffer,"High Rate");	else	sprintf(buffer,"Long Range");
	}
	else if(var=="BEACONS")			{	GenerateJson();
										strcpy(buffer,jsonstring);								}
	else if(var=="BEACON_VOLTAGE")	{	sprintf(buffer,"%.3f",lastfix.voltage);					}
	else if(var=="BEACON_ID")		{	sprintf(buffer,"%d",lastfix.id);						}
	else if(var=="BEACON_HACC")		{	sprintf(buffer,"%.3f",lastfix.accuracy);				}
	else if(var=="LORA_FREQUENCY")	{	sprintf(buffer,"%.3f",lora_freq/1e6);					}
	else if(var=="LORA_RSSI")		{	sprintf(buffer,"%d",lastfix.rssi);						}
	else if(var=="LORA_SNR")		{	sprintf(buffer,"%d",lastfix.snr);						}
 	else if(var=="RX_LATITUDE")		{	sprintf(buffer,"%2.6f",rxfix.latitude);					}
	else if(var=="RX_LONGITUDE")	{	sprintf(buffer,"%3.6f",rxfix.longitude);				}
	else if(var=="RX_ALTITUDE")		{	sprintf(buffer,"%.1f",rxfix.height);					}
	else if(var=="ALTITUDE")		{	sprintf(buffer,"%.1f",(lastfix.height-rxfix.height));	}
	else if(var=="TX_LATITUDE")		{	sprintf(buffer,"%2.6f",lastfix.latitude);				}
	else if(var=="TX_LONGITUDE")	{	sprintf(buffer,"%3.6f",lastfix.longitude);				}
	else if(var=="TX_ALTITUDE")		{	sprintf(buffer,"%.1f",lastfix.height);					}
	else if(var=="NUMSATS")			{	sprintf(buffer,"%d",numCh);								}
	else if(var=="RX_HEADING")		
	{
		if(track_compass)	sprintf(buffer,"%.1f",heading);
		else				sprintf(buffer,"0.0");
	}
	else if(var=="FLIGHT_STATE")
	{
		if(strlen(flight_state_text)>0)		sprintf(buffer,"%s",flight_state_text);
		else								strcpy(buffer,"Unknown");
	}
	
	if(strlen(buffer)>0)	return(buffer);
	else					return String();
}

int SetupWebServer(void)
{
	Serial.println("SetupWebServer() entry");

	if(!SPIFFS.begin(true))
	{
		Serial.println("An error has occurred while mounting SPIFFS");
		return(1);
	}
	
	// Act as an Access Point
	Serial.print("Setting up WiFi AP with ssid \"");		Serial.print(ssid);	Serial.print("\", password \"");	Serial.print(password);	Serial.println("\"");
	
	WiFi.softAP(ssid,password);

	IPAddress IP=WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);
	
	if(!MDNS.begin(ssid))
	{
		Serial.println("Error starting mDNS");
		return(1);
	}
	
	// Route for root / web page
	server.on("/",HTTP_GET,[](AsyncWebServerRequest *request)						{	request->redirect("/index.html");													});
	
	server.on("/index.html",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/index.html");												});
	server.on("/index.css",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/index.css");													});

	server.on("/overall.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/overall.css");												});
	
	server.on("/logo.jpg",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/logo.jpg");													});
	server.on("/small_logo.jpg",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/small_logo.jpg");											});
	
	server.on("/status.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/status.html",String(),false,statusprocessor);				});
	server.on("/status.css",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/status.css");												});
	server.on("/status.js",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/status.js",String(),false,statusprocessor);					});
	
	server.on("/beacon_up.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/beacon_up.html");	track_compass=true;						});
	server.on("/north_up.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/north_up.html");		track_compass=false;					});
		
	server.on("/rockettrackreceiver.js",HTTP_GET,[](AsyncWebServerRequest *request)	{	request->send(SPIFFS,"/rockettrackreceiver.js",String(),false,trackingprocessor);	});
	
	server.on("/tracking.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/tracking.html",String(),false,trackingprocessor);			});
	
	server.on("/select.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/telemetry.html",String(),false,trackingprocessor);			});
	
	server.on("/telemetry.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/telemetry.html",String(),false,trackingprocessor);			});
	server.on("/telemetry.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/telemetry.css");												});

	server.on("/configure.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/configure.html");											});

	server.on("/channels.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/channels.html",String(),false,chanprocessor);				});

	server.on("/lora_settings.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/lora_settings.html",String(),false,loraprocessor);			});

	server.on("/mag_calibration.html",HTTP_GET,[](AsyncWebServerRequest *request)	{	request->send(SPIFFS,"/mag_calibration.html",String(),false,magprocessor);			});

	server.on("/mag_save.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
    	// display params
    	size_t count=request->params();
    	
    	Serial.printf("Got %d params in request\r\n",count);
    	
    	for(size_t cnt=0;cnt<count;cnt++)
    	{
    		const AsyncWebParameter *param=request->getParam(cnt);
    		Serial.printf("PARAM[%u]: \"%s\" = \"%s\"\r\n",cnt,param->name().c_str(),param->value().c_str());
    		
    		if(strncmp(param->name().c_str(),"A11",3)==0)	{	Mag_A11=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A12",3)==0)	{	Mag_A12=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A13",3)==0)	{	Mag_A13=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A21",3)==0)	{	Mag_A21=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A22",3)==0)	{	Mag_A22=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A23",3)==0)	{	Mag_A23=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A31",3)==0)	{	Mag_A31=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A32",3)==0)	{	Mag_A32=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"A33",3)==0)	{	Mag_A33=atof(param->value().c_str());		}
    		
    		if(strncmp(param->name().c_str(),"B1",2)==0)	{	Mag_B1=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"B2",2)==0)	{	Mag_B2=atof(param->value().c_str());		}
    		if(strncmp(param->name().c_str(),"B3",2)==0)	{	Mag_B3=atof(param->value().c_str());		}
		}
		
		StoreCalibrationData();
		
		request->redirect("/configure.html");
	});
	
	server.on("/chan_save.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
    	// display params
    	size_t count=request->params();
    	
    	Serial.printf("Got %d params in request\r\n",count);
    	
    	for(size_t cnt=0;cnt<count;cnt++)
    	{
    		const AsyncWebParameter *param=request->getParam(cnt);
    		Serial.printf("PARAM[%u]: \"%s\" = \"%s\"\r\n",cnt,param->name().c_str(),param->value().c_str());
    		
    		if(strncmp(param->name().c_str(),"channel",7)==0)
    		{
    			if(strncmp(param->value().c_str(),"ch1",3)==0)	lora_freq=LORA_CH1*1e6;
    			if(strncmp(param->value().c_str(),"ch2",3)==0)	lora_freq=LORA_CH2*1e6;
    			if(strncmp(param->value().c_str(),"ch3",3)==0)	lora_freq=LORA_CH3*1e6;
    			if(strncmp(param->value().c_str(),"ch4",3)==0)	lora_freq=LORA_CH4*1e6;
    			if(strncmp(param->value().c_str(),"ch5",3)==0)	lora_freq=LORA_CH5*1e6;
    			if(strncmp(param->value().c_str(),"ch6",3)==0)	lora_freq=LORA_CH6*1e6;
    		}    		
		}
		
		StoreCalibrationData();
		
		request->redirect("/configure.html");
	});
	
	server.on("/lora_save.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
    	// display params
    	size_t count=request->params();
    	
    	Serial.printf("Got %d params in request\r\n",count);
    	
    	for(size_t cnt=0;cnt<count;cnt++)
    	{
    		const AsyncWebParameter *param=request->getParam(cnt);
    		Serial.printf("PARAM[%u]: \"%s\" = \"%s\"\r\n",cnt,param->name().c_str(),param->value().c_str());
    		
    		if(strncmp(param->name().c_str(),"hrbw",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"10.4K",5)==0)	hr_bw=10400;
    			if(strncmp(param->value().c_str(),"15.6K",5)==0)	hr_bw=15600;
    			if(strncmp(param->value().c_str(),"20.8K",5)==0)	hr_bw=20800;
    			if(strncmp(param->value().c_str(),"31.25K",6)==0)	hr_bw=31250;
    			if(strncmp(param->value().c_str(),"41.7K",5)==0)	hr_bw=41700;
    			if(strncmp(param->value().c_str(),"62.5K",5)==0)	hr_bw=62500;
    			if(strncmp(param->value().c_str(),"125K",4)==0)		hr_bw=125000;
    			if(strncmp(param->value().c_str(),"250K",4)==0)		hr_bw=125000;
    		}
    		
    		if(strncmp(param->name().c_str(),"hrsf",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"SF6",3)==0)		hr_sf=6;
    			if(strncmp(param->value().c_str(),"SF7",3)==0)		hr_sf=7;
    			if(strncmp(param->value().c_str(),"SF8",3)==0)		hr_sf=8;
    			if(strncmp(param->value().c_str(),"SF9",3)==0)		hr_sf=9;
    			if(strncmp(param->value().c_str(),"SF10",4)==0)		hr_sf=10;
    			if(strncmp(param->value().c_str(),"SF11",4)==0)		hr_sf=11;
    			if(strncmp(param->value().c_str(),"SF12",4)==0)		hr_sf=12;
			}
    		
    		if(strncmp(param->name().c_str(),"hrcr",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"CR4",3)==0)		hr_cr=4;
    			if(strncmp(param->value().c_str(),"CR5",3)==0)		hr_cr=5;
    			if(strncmp(param->value().c_str(),"CR6",3)==0)		hr_cr=6;
    			if(strncmp(param->value().c_str(),"CR7",3)==0)		hr_cr=7;
    			if(strncmp(param->value().c_str(),"CR8",3)==0)		hr_cr=8;
			}
    		
    		if(strncmp(param->name().c_str(),"lrbw",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"10.4K",5)==0)	lr_bw=10400;
    			if(strncmp(param->value().c_str(),"15.6K",5)==0)	lr_bw=15600;
    			if(strncmp(param->value().c_str(),"20.8K",5)==0)	lr_bw=20800;
    			if(strncmp(param->value().c_str(),"31.25K",6)==0)	lr_bw=31250;
    			if(strncmp(param->value().c_str(),"41.7K",5)==0)	lr_bw=41700;
    			if(strncmp(param->value().c_str(),"62.5K",5)==0)	lr_bw=62500;
    			if(strncmp(param->value().c_str(),"125K",4)==0)		lr_bw=125000;
    			if(strncmp(param->value().c_str(),"250K",4)==0)		lr_bw=125000;
    		}
    		
    		if(strncmp(param->name().c_str(),"lrsf",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"SF6",3)==0)		lr_sf=6;
    			if(strncmp(param->value().c_str(),"SF7",3)==0)		lr_sf=7;
    			if(strncmp(param->value().c_str(),"SF8",3)==0)		lr_sf=8;
    			if(strncmp(param->value().c_str(),"SF9",3)==0)		lr_sf=9;
    			if(strncmp(param->value().c_str(),"SF10",4)==0)		lr_sf=10;
    			if(strncmp(param->value().c_str(),"SF11",4)==0)		lr_sf=11;
    			if(strncmp(param->value().c_str(),"SF12",4)==0)		lr_sf=12;
			}
    		
    		if(strncmp(param->name().c_str(),"lrcr",4)==0)
    		{
    			if(strncmp(param->value().c_str(),"CR4",3)==0)		lr_cr=4;
    			if(strncmp(param->value().c_str(),"CR5",3)==0)		lr_cr=5;
    			if(strncmp(param->value().c_str(),"CR6",3)==0)		lr_cr=6;
    			if(strncmp(param->value().c_str(),"CR7",3)==0)		lr_cr=7;
    			if(strncmp(param->value().c_str(),"CR8",3)==0)		lr_cr=8;
			}
		}
		
		StoreCalibrationData();
		
		request->redirect("/configure.html");
	});
	
	server.on("/lora_defaults.html",HTTP_POST,[](AsyncWebServerRequest *request)
	{
		Serial.println("Should be resetting the LoRa settings to default here");
		
		SetLoRaDefaults();
		StoreCalibrationData();
		
		request->redirect("/configure.html");
	});
	





























	
#if 0	
	server.on("/calibrate.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.html");											});
	server.on("/calibrate.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.css");												});
	server.on("/calibrate.js",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.js");												});
	
	server.on("/cal_accel.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/cal_accel.html");											});

	server.on("/cal_accel_xplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_xplus.html");
		CalibrateAccelerometer("Nose up",&(accelmax.x),'x');
	});
	
	server.on("/cal_accel_xminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_xminus.html");
		CalibrateAccelerometer("Nose down",&(accelmin.x),'x');
	});
	
	server.on("/cal_accel_yplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_yplus.html");
		CalibrateAccelerometer("Right down",&(accelmax.y),'y');
	});
	
	server.on("/cal_accel_yminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_yminus.html");
		CalibrateAccelerometer("Left down",&(accelmin.y),'y');
	});
	
	server.on("/cal_accel_zplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_zplus.html");
		CalibrateAccelerometer("Flat and level",&(accelmax.z),'z');
	});
	
	server.on("/cal_accel_zminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_zminus.html");
		CalibrateAccelerometer("Upside-down",&(accelmin.z),'z');
	});

	server.on("/cal_gyro.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_gyro.html");
	});
		
	server.on("/cal_magnetometer.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_magnetometer.html");
	});
#endif

	server.on("/cal_complete.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/cal_complete.html");								});

	server.on("/store_calibration.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/store_calibration.html");		
		StoreCalibrationData();
	});
	
	server.on("/retrieve_calibration.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/retrieve_calibration.html");		
		RetrieveCalibrationData();
	});
	
	server.on("/reset_calibration.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/reset_calibration.html");

		#warning "more work here"		
	});
	
	server.on("/favicon.ico",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/favicon.ico");									});
	
#if 0
	server.on("/engineering.html",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/engineering.html");								});
	server.on("/engineering.css",HTTP_GET,[](AsyncWebServerRequest *request)		{	request->send(SPIFFS,"/engineering.css");								});
	server.on("/engineering.js",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/engineering.js");								});
#endif
	
	// Start server
	server.begin();
	
	return(0);
}

void JsonTest(void)
{
#if 0
	JsonDocument beacons;
	char string[]="[{\"id\":0,\"lat\":52.07968333,\"long\":-2.30775833,\"alt\":50.0,\"hdop\":1.5,\"numsats\":5,\"hacc\":5.1},\r\n{\"id\":1,\"lat\":52.10981389,\"long\":-2.25207222,\"alt\":100.0,\"hdop\":1.6,\"numsats\":6,\"hacc\":6.1}]";
	
	Serial.println(string);
	
	DeserializationError error=deserializeJson(beacons,string);

	// Test if parsing succeeds.
	if (error)
	{
    	Serial.print(F("deserializeJson() failed: "));
    	Serial.println(error.f_str());
    	return;
	}
	
	JsonArray arr=beacons.as<JsonArray>();
	
	for(JsonVariant value:arr)
	{
		Serial.print("id   = ");	Serial.println(value["id"].as<const int>());
		Serial.print("lat  = ");	Serial.println(value["lat"].as<const float>());
		Serial.print("long = ");	Serial.println(value["long"].as<const float>());
	}
#endif

	GenerateJson();
}

void GenerateJson(void)
{
	JsonDocument construction;
	
	JsonArray c_arr=construction.to<JsonArray>();

	c_arr[0]["id"]=0;
	c_arr[0]["numsats"]=5;
	c_arr[0]["gpsfix"]=3;
	c_arr[0]["long"]=-2.30775833;
	c_arr[0]["lat"]=52.07968333;
	c_arr[0]["height"]=50.0;
	c_arr[0]["accuracy"]=2.5;
	c_arr[0]["voltage"]=4.15;
	c_arr[0]["counter"]=123;
	c_arr[0]["snr"]=12;
	c_arr[0]["rssi"]=-100;
	c_arr[0]["millis"]=12345;

	c_arr[1]["id"]=1;
	c_arr[1]["numsats"]=6;
	c_arr[1]["gpsfix"]=3;
	c_arr[1]["long"]=-2.25207222;
	c_arr[1]["lat"]=52.10981389;
	c_arr[1]["height"]=100.0;
	c_arr[1]["accuracy"]=3.5;
	c_arr[1]["voltage"]=3.750;
	c_arr[1]["counter"]=124;
	c_arr[1]["snr"]=14;
	c_arr[1]["rssi"]=-95;
	c_arr[1]["millis"]=12345;

	for(JsonVariant value:c_arr)
	{
		Serial.print("id   = ");	Serial.println(value["id"].as<const int>());
		Serial.print("lat  = ");	Serial.println(value["lat"].as<const float>());
		Serial.print("long = ");	Serial.println(value["long"].as<const float>());
		Serial.print("alt  = ");	Serial.println(value["height"].as<const float>());
	}
	
	serializeJson(construction,jsonstring);
	Serial.println(jsonstring);

}

