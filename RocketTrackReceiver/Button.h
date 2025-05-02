
#pragma once

extern bool button_enable;

int SetupButton(void);
int PollButton(void);

int ButtonCommandHandler(uint8_t *cmd,uint16_t cmdptr);

enum ButtonEvents
{
	BUTTON_NO_EVENT=0,
	BUTTON_SHORT_PRESS,
	BUTTON_LONG_PRESS,
	BUTTON_REALLY_LONG_PRESS
};

