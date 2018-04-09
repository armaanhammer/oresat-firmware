#ifndef _ACS_H_
#define _ACS_H_

#include "ch.h"
#include "hal.h"
#include "acs_feedback.h"

#define THREAD_SIZE	(1<<7) 
#define CAN_NODE 0x3F // max 0x7f

#define DEBUG_SERIAL SD2
#define DEBUG_CHP ((BaseSequentialStream *) &DEBUG_SERIAL)

typedef struct{
	// o_0
}ACSConfig;

typedef struct{
	uint8_t acs[8];
} ACSdata;

extern void acs_init(void);

extern THD_WORKING_AREA(wa_acsThread,THREAD_SIZE);
extern THD_FUNCTION(acsThread, arg);

#endif
