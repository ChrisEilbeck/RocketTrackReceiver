
#pragma once

#define MAX_FIXES 1000

typedef struct
{
	uint8_t id;
	uint8_t gpsfix;
	uint8_t numsats;
	float longitude;
	float latitude;
	float height;
	float accuracy;
	float voltage;
	uint16_t counter;
	uint32_t millis;
} fix;

