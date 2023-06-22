
#pragma once

extern char gps_type[];
extern int initial_baud;
extern int final_baud;
extern int fix_rate;

extern uint8_t numCh;

extern uint8_t gpsFix;

extern int32_t rxlat;
extern int32_t rxlon;
extern int32_t rxheight;
extern int32_t rxhMSL;
extern int32_t max_gps_hMSL;
extern uint32_t gps_hAcc;
extern uint32_t gps_vAcc;

extern uint8_t gps_hAccValue;

extern uint16_t gps_year;
extern uint8_t gps_month;
extern uint8_t gps_day;
extern uint8_t gps_hour;
extern uint8_t gps_min;
extern uint8_t gps_sec;
