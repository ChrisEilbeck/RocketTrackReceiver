
#pragma once

int SetupWebServer(void);
void PollWebServer(void);

extern char ssid[32];
extern char password[32];

extern bool track_compass;

