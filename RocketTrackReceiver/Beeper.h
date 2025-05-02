
#pragma once

extern bool beeper_enable;

int SetupBeeper(void);
void PollBeeper(void);

void BeeperSetPattern(uint32_t pattern,int repeats);

int BeeperCommandHandler(uint8_t *cmd,uint16_t cmdptr);

