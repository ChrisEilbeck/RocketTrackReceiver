
#pragma once

#define EEPROM_SIZE			256

#define COMPASS_CAL_ADDRESS	0x0000

int SetupNvMemory(void);

#if 0
	void StoreCalibrationData(void);
	int RetrieveCalibrationData(void);
#endif