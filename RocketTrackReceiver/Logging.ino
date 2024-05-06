
#include "Logging.h"

uint16_t fixcounter=0;
fix flightdata[MAX_FIXES];

void ResetLogging(void)
{
	fixcounter=0;
	memset(&flightdata,0,MAX_FIXES*sizeof(fix));
}

