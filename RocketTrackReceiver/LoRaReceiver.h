
#pragma once

#define LORA_LONG_RANGE_MODE	0
#define LORA_HIGH_RATE_MODE		1

int SetupLoRaReceiver(void);

void PollLoRaReceiver(int fakepacket);
void PollLoRaScheduler(void);

int LoRaGetMode(void);

int ReceiverCommandHandler(uint8_t *cmd,uint16_t cmdptr);


