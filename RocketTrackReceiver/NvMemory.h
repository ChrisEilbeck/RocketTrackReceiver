
#pragma once

#define EEPROM_SIZE			256

#define COMPASS_CAL_ADDRESS	0x0000

#include "MPU9250.h"

void StoreCompassCalibration(void);
int RetrieveCompassCalibration(void);

