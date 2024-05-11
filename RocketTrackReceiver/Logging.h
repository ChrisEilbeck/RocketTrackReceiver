
#pragma once

#define MAX_FIXES 1000

typedef struct
{
	uint8_t id;
	uint8_t gpsfix;
	uint8_t numsats;
	int8_t snr;
	int8_t rssi;
	uint8_t spare1;
	uint8_t spare2;
	uint8_t spare3;
	float longitude;
	float latitude;
	float height;
	float accuracy;
	float voltage;
	uint16_t counter;
	uint32_t millis;
} fix;

