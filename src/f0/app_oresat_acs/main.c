/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 *	OreSat: Attitude Control System
 *	Portland State Aerospace Society (PSAS)
 *	
 *  // be wery wery quiet i'm hunting wabbits...
 *
 *	// add your name if you code things
 *	// and you are paying attention
 *	// and you want your code things in
 *	// space 
 *	
 *	// o_0
 *
 *	Chad Coates	
 *
 */

//=== ChibiOS header files
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

//=== Project header files
#include "oresat.h"
#include "acs.h"
#include "acs_bldc.h"

ACSdata data;
//uint8_t data[8];
static uint16_t rxbuf[2];

static SerialConfig ser_cfg = {
	115200,     //Baud rate
	0,          //
	0,          //
	0,          //
};

static THD_WORKING_AREA(spi_thread_1_wa, 128);
static THD_FUNCTION(spi_thread_1, arg) {

  (void)arg;
  chRegSetThreadName("SPI thread 1");

  uint16_t encoder_val = 0;

  spiStart(&SPID1, &spicfg);                // Start driver.
  spiAcquireBus(&SPID1);                    // Gain ownership of bus.

  while(true)
  {
		rxbuf[0] = 0;
		spiSelect(&SPID1);                        // Select slave.

		while(SPID1.state != SPI_READY) {}        // Waiting for driver state to be ready.
		spiReceive(&SPID1,1,rxbuf);               // Receive 1 frame (16 bits).
		spiUnselect(&SPID1);                      // Unselect slave.

		encoder_val = 0x3FFF & rxbuf[0];
	 
		// Display results
		chprintf(DEBUG_CHP,"Decimal: %u \n", encoder_val);        
		//chprintf(DEBUG_CHP,"Hex: %x \n", encoder_val);        

		chThdSleepMilliseconds(1000);
  }

	spiReleaseBus(&SPID1);    // Release ownership of bus.
	spiStop(&SPID1);          // Stop driver.

}
static void app_init(void){
	for(int i = 0; i < 2; ++i){   // Initializing receive buffer to zero. 
		rxbuf[i] = 0;
	}

	//	acs_init();	 
//	bldcInit();
	canRPDOObjectInit(CAN_PDO_1,CAN_ID_DEFAULT,8,data.acs);
//	initTPDO();

	sdStart(&SD2,&ser_cfg); // Start up debug output
}

static void app_main(void){
		chThdCreateStatic(
		spi_thread_1_wa,
		sizeof(spi_thread_1_wa),
		NORMALPRIO + 1,
		spi_thread_1,
		NULL
	);
/*
	chThdCreateStatic(
		wa_bldcThread,
		sizeof(wa_bldcThread), 
		NORMALPRIO, 
		bldcThread, 
		NULL
	);
//*/
/*
	chThdCreateStatic(
		wa_acsThread,
		sizeof(wa_acsThread), 
		NORMALPRIO, 
		acsThread, 
		NULL
	);
//*/
	while(true){
		chThdSleepMilliseconds(1000);
	}
}

int main(void) {
	halInit();
	chSysInit();
	
	oresat_init(CAN_NODE);
	
	app_init();
	app_main();
	
	return 0;
}

//! @}
