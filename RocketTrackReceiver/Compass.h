
#pragma once

int SetupCompass(void);
void PollCompass(void);

int CompassCommandHandler(uint8_t *cmd,uint16_t cmdptr);

void CalibrateCompass(void);
Void CalibrateAccelGyro(void);

