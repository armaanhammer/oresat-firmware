#include "encodertest.h"



/**
 * @brief Handles the SPI transaction, getting the position from the encoder
 *
 */
THD_WORKING_AREA(wa_spiThread,THREAD_SIZE);
THD_FUNCTION(spiThread,arg){
  (void)arg;
  chRegSetThreadName("spiThread");

  spiStart(&SPID1,&spicfg);            	// Start driver.
  spiAcquireBus(&SPID1);                // Gain ownership of bus.

  // hacky
  bldc motor;
  uint16_t testbit;	

  // debug
  palSetLineMode(LINE_LED,PAL_MODE_OUTPUT_PUSHPULL);
  
  while (!chThdShouldTerminateX()) {
	motor.spi_rxbuf[0] = 0;
	spiSelect(&SPID1);              // Select slave.

	// debug
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(200);
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(200);

	while(SPID1.state != SPI_READY) {}

	// debug
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(500);
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(500);

	spiReceive(&SPID1,1,motor.spi_rxbuf);  // Receive 1 frame (16 bits).

	// debug
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(1500);
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(1500);


	spiUnselect(&SPID1);                	// Unselect slave.

	/// TODO: figure out what this bit mask is for. 
	//motor->position = 0x3FFF & motor->spi_rxbuf[0];
	motor.position = 0x3FFF & motor.spi_rxbuf[0];



	// hacky
	testbit = (0x200 & motor.position) >> 9;  // test the 10th bit
	if (testbit >= 1) {
		palWriteLine(LINE_LED, PAL_HIGH)
	} else {
		palWriteLine(LINE_LED, PAL_LOW)
	}
	
	// debug
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(200);
	palToggleLine(LINE_LED);
        chThdSleepMilliseconds(200);

  }

	spiReleaseBus(&SPID1);    // Release ownership of bus.
	spiStop(&SPID1);          // Stop driver.
	
	// hacky
	palClearLine(LINE_LED);

}



/**
 * @brief planning to use the LED blink thread for debugging
 *
 */
THD_WORKING_AREA(blink_wa, 0x40);
THD_FUNCTION(blink, arg)
{
    (void)arg;

    //palSetLineMode(LINE_LED,PAL_MODE_OUTPUT_PUSHPULL);

    uint16_t testbit;	

    while (!chThdShouldTerminateX()) {
	//testbit = (0x200 & motor.position) >> 9;  // test the 10th bit
	//if (testbit >= 1) {
	//	palWriteLine(LINE_LED, PAL_HIGH)
	//} else {
	//	palWriteLine(LINE_LED, PAL_LOW)
	//}

 				
        //palToggleLine(LINE_LED);
	
        chThdSleepMilliseconds(500);
    }

    //palClearLine(LINE_LED);
    chThdExit(MSG_OK);
}


