
#define DEBUG 0

//#include "network_creds.h"
#include "Logging.h"
#include "Packetisation.h"
#include "Webserver.h"

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <ESPmDNS.h>	

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

char ssid[32]="rocketrx";
char password[32]="eggsbenedict";

int track_compass=1;

#if 0
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
#endif

String trackingprocessor(const String& var)
{
	char buffer[256];
	memset(buffer,0,sizeof(buffer));
	
	if(var=="LORA_MODE")
	{
		if(lora_mode==1)	sprintf(buffer,"High Rate");	else	sprintf(buffer,"Long Range");
	}
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
		if(track_compass)	sprintf(buffer,"%.1f",rx_heading);
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
	
#if 0
	server.on("/status.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/status.html"String(),false,statusprocessor););				});
	server.on("/status.css",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/status.css");												});
	server.on("/status.js",HTTP_GET,[](AsyncWebServerRequest *request)				{	request->send(SPIFFS,"/status.js",String(),false,statusprocessor);					});
#endif
	
	server.on("/beacon_up.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/beacon_up.html");	track_compass=1;						});
	server.on("/north_up.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/north_up.html");		track_compass=0;						});
		
	server.on("/rockettrackreceiver.js",HTTP_GET,[](AsyncWebServerRequest *request)	{	request->send(SPIFFS,"/rockettrackreceiver.js",String(),false,trackingprocessor);	});
	
	server.on("/tracking.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/tracking.html",String(),false,trackingprocessor);			});
	server.on("/tracking.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/tracking.css");												});
	
	server.on("/telemetry.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/telemetry.html",String(),false,trackingprocessor);			});
	server.on("/telemetry.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/telemetry.css");												});

	server.on("/calibrate.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.html");											});
	server.on("/calibrate.css",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.css");												});
	server.on("/calibrate.js",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/calibrate.js");												});
	
	server.on("/cal_accel.html",HTTP_GET,[](AsyncWebServerRequest *request)			{	request->send(SPIFFS,"/cal_accel.html");											});

	server.on("/cal_accel_xplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_xplus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Nose up",&vals);			accelmax.x=vals.x;
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});
	
	server.on("/cal_accel_xminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_xminus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Nose down",&vals);		accelmin.x=vals.x;
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});
	
	server.on("/cal_accel_yplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_yplus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Right down",&vals);		accelmax.y=vals.y;
		
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});
	
	server.on("/cal_accel_yminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_yminus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Left down",&vals);		accelmin.y=vals.y;
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});
	
	server.on("/cal_accel_zplus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_zplus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Flat and level",&vals);	accelmax.z=vals.z;
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});
	
	server.on("/cal_accel_zminus.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_accel_zminus.html");
		xyzFloat vals;		CalibrateMPU6500Accelerometer("Upside-down",&vals);		accelmin.z=vals.z;
		mpu6500.setAccOffsets(accelmin.x,accelmax.x,accelmin.y,accelmax.y,accelmin.z,accelmax.z);
	});

	server.on("/cal_gyro.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_gyro.html");
		CalibrateQMC5883LMagnetometer();
	});
		
	server.on("/cal_magnetometer.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS,"/cal_magnetometer.html");
		CalibrateQMC5883LMagnetometer();
	});

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

