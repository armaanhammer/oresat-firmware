#include "encodertest.h"

/* Example blinker thread */
THD_WORKING_AREA(blink_wa, 0x40);
THD_FUNCTION(blink, arg)
{
    (void)arg;

    palSetLineMode(LINE_LED,PAL_MODE_OUTPUT_PUSHPULL);

    while (!chThdShouldTerminateX()) {
        palToggleLine(LINE_LED);
        chThdSleepMilliseconds(500);
    }

    palClearLine(LINE_LED);
    chThdExit(MSG_OK);
}



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

  bldc motor;

  while (!chThdShouldTerminateX()) {
	motor.spi_rxbuf[0] = 0;
	spiSelect(&SPID1);              // Select slave.

	while(SPID1.state != SPI_READY) {}   
	spiReceive(&SPID1,1,motor.spi_rxbuf);  // Receive 1 frame (16 bits).
	spiUnselect(&SPID1);                	// Unselect slave.

	/// TODO: figure out what this does. 
	//motor->position = 0x3FFF & motor->spi_rxbuf[0];
	motor.position = 0x3FFF & motor.spi_rxbuf[0];
	
  }

	spiReleaseBus(&SPID1);    // Release ownership of bus.
	spiStop(&SPID1);          // Stop driver.
}
