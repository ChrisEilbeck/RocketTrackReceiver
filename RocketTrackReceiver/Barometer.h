
#pragma once

extern bool baro_enable;
extern char baro_type[];
extern int baro_rate;
extern bool baro_gps_sync;
extern bool baro_trigger;

int SetupBarometer(void);
void PollBarometer(void);

int BarometerCommandHandler(uint8_t *cmd,uint16_t cmdptr);

