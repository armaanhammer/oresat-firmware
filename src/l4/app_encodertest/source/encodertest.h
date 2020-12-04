#ifndef _ENCODERTEST_H_
#define _ENCODERTEST_H_

#include "ch.h"
#include "hal.h"

/// TODO: had to reduce this to compile with new ADC code
/// We should figure out what an actual good value is
#define THREAD_SIZE	(96)

/* Example blinker thread prototypes */
//extern THD_WORKING_AREA(blink_wa, 0x40);
//extern THD_FUNCTION(blink, arg);


extern THD_WORKING_AREA(wa_spiThread,THREAD_SIZE);
/// Prototype for spi thread function.
extern THD_FUNCTION(spiThread,arg);

#endif
