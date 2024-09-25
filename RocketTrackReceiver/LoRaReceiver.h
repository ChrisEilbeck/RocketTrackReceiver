
#pragma once

#define LORA_LONG_RANGE_MODE	0
#define LORA_HIGH_RATE_MODE		1

extern double lora_freq;
extern int lora_mode;
extern bool lora_crc;

extern int hr_bw;
extern int hr_sf;
extern int hr_cr;
extern int hr_period;

extern int lr_bw;
extern int lr_sf;
extern int lr_cr;
extern int lr_period;

int SetupLoRaReceiver(void);

void PollLoRaReceiver(int fakepacket);
void PollLoRaScheduler(void);

int LoRaGetMode(void);

int ReceiverCommandHandler(uint8_t *cmd,uint16_t cmdptr);


