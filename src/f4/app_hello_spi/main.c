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

//=== ChibiOS header files
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

//=== Project header files
#include "can.h"

/*
 * Maximum speed SPI configuration (21MHz, CPHA=0, CPOL=0, MSb first).
 */
/*
static const SPIConfig hs_spicfg = {
    0,
  NULL,         // SPI operation complete callback
  NULL,         // Chip select line port
  GPIOA,
  4,            // Chip select pad number
  0,            // SPI initialization data.
};
*/
static const SPIConfig hs_spicfg = {
    false,          // Enables circular buffer if == 1
    NULL,       // Operation complete call back.
    GPIOA,          // Chip select line
    GPIOA_SPI1_NSS,      // Chip select port
    SPI_CR1_BR_1 | SPI_CR1_BR_2,
    0,            // Chip select port mask
};

/*
 * Low speed SPI configuration (328.125kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
  false,
  NULL,
  GPIOA,
  GPIOA_SPI1_NSS,
  SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

/*
 * SPI TX and RX buffers.
 */
static uint8_t txbuf[512];
static uint8_t rxbuf[512];

//=== Serial configuration
static SerialConfig ser_cfg =
{
    115200,     //Baud rate
    0,          //
    0,          //
    0,          //
};

//=== Thread definitions

// Example blinker thread
static THD_WORKING_AREA(spi_thread_1_wa, 128);
static THD_FUNCTION(spi_thread_1, arg) {

  (void)arg;
  chRegSetThreadName("SPI thread 1");

  while (true) {
      
    spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
    palSetPad(GPIOD, GPIOD_PIN5);       // *****WE CHOOSE THIS**********************
    spiStart(&SPID1, &hs_spicfg);       /* Setup transfer parameters.       */
    spiSelect(&SPID1);                  /* Slave Select assertion.          */
    spiExchange(&SPID1, 512,
               txbuf, rxbuf);          /* Atomic transfer operations.      */
    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */
  }
}

/*

 * SPI bus contender 2.

 */

static THD_WORKING_AREA(spi_thread_2_wa, 256);
static THD_FUNCTION(spi_thread_2, p) {

  (void)p;
  chRegSetThreadName("SPI thread 2");
  while (true) {
    spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
    palClearPad(GPIOD, GPIOD_PIN5);     // *****WE CHOOSE THIS**********************
    spiStart(&SPID1, &ls_spicfg);       /* Setup transfer parameters.       */
    spiSelect(&SPID1);                  /* Slave Select assertion.          */
    spiExchange(&SPID1, 512,
                txbuf, rxbuf);          /* Atomic transfer operations.      */
    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */
    
  }
}



static void app_init(void) {
    //=== App initialization
 /*
   * SPI2 I/O pins setup.
   */
    
  //palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5) |
  //                         PAL_STM32_OSPEED_HIGHEST);       /* New SCK.     */

  //palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5) |
  //                         PAL_STM32_OSPEED_HIGHEST);       /* New MISO.    */

  //palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5) |
  //                         PAL_STM32_OSPEED_HIGHEST);       /* New MOSI.    */

//  palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL |
 //                          PAL_STM32_OSPEED_HIGHEST);       /* New CS.      */

//  palSetPad(GPIOA, 4);
  
    // Start up debug output
    sdStart(&SD2, &ser_cfg);

}

static void main_app(void) {
    //=== Start application threads
    unsigned i;

    /*
     * Prepare transmit pattern.
     */
    for(i=0; i < sizeof(txbuf); i++)
        txbuf[i] = (uint8_t)i;



     /*
   * Starting the transmitter and receiver threads.
   */
  chThdCreateStatic(spi_thread_1_wa, sizeof(spi_thread_1_wa),
                    NORMALPRIO + 1, spi_thread_1, NULL);

  chThdCreateStatic(spi_thread_2_wa, sizeof(spi_thread_2_wa),
                    NORMALPRIO + 1, spi_thread_2, NULL);
    /*
     * Begin main loop
     */
    while (true)
    {
        chThdSleepMilliseconds(1000);
    }
}

int main(void) {
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();
    // Initialize CAN Subsystem
    can_init();
    // Start CAN threads
    can_start();

    // Initialize and start app
    app_init();
    main_app();

    return 0;
}

//! @}
