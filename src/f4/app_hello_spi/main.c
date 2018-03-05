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
#include "spi_header.h"

/*
reg_addrs addr_init
{
    no_op = 0x0000;       // No operation
    err  = 0x0001;       // Error register
    program      = 0x0003;       // Programming register
    diag         = 0x3FFC;       // Diagnostic and AGC   
    magnitude    = 0x3FFD;       // CORDIC Magnitude
    angl_no_err  = 0x3FFE;       // Measured angle with no error compensation.
    angl_err     = 0x3FFF;       // Measured angle with error compensation.
    z_pos_msb    = 0x0016;       // Zero position msb.
    z_pos_lsb    = 0x0017;       // Zero position lsb.
    settings_1   = 0x0018;       // Custom settings register 1
    settings_2   = 0x0019;       // Custom settings register 2
}
*/




/*
 * Maximum speed SPI configuration (21MHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig spicfg = {
    false,              // Enables circular buffer if == 1
    NULL,               // Operation complete call back.
    GPIOA,              // Chip select line
    GPIOA_SPI1_NSS,     // Chip select port
    //SPI_CR1_BR_1 | SPI_CR1_BR_0,
    SPI_CR1_BR_0,
    0,                  // Chip select port mask
};

/*
 * Low speed SPI configuration (328.125kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
  false,
  NULL,
  GPIOA,
  GPIOA_SPI1_NSS,
  SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

/*
 * SPI TX and RX buffers.
 */
static int txbuf[1024];
static int rxbuf[14];

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
  int address = 0x3FFF;
  int newval = 1;

  spiStart(&SPID1, &spicfg);
  

  while(true)
  {
    
      spiSelect(&SPID1);        // Lower chip select

//      spiSend(&SPID1,512,&txbuf); // Transmitting one word.
      spi_write(&SPID1,address,txbuf,sizeof(txbuf));

      spiUnselect(&SPID1);      // Raising chip select

      chThdSleepMilliseconds(1000);

  }

  //spiStop(&SPID1);

}

static void app_init(void) {
    //=== App initialization


    // Start up debug output
    sdStart(&SD2, &ser_cfg);
    
}

static void start_threads(void)
{
  /*
   * Starting the transmitter and receiver threads.
   */

    chThdCreateStatic(spi_thread_1_wa, sizeof(spi_thread_1_wa),
                    NORMALPRIO + 1, spi_thread_1, NULL);



}

static void transmit_func(void)
{

  int addr = 0x3FFF;
  int newval = 1;

    //spi_write(&SPID1, addr,txbuf, newval);

}


static void main_app(void) {
    //=== Start application threads
    unsigned i;

    chThdSleepMilliseconds(500);

    /*
     * Prepare transmit pattern
     */
     for(i=0; i < sizeof(txbuf); i++)
        txbuf[i] = (uint8_t)i;


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

    // Initialize and start app
    app_init();

    start_threads();

    //main_app();

    return 0;
}

void spi_read(SPIDriver * spip, int address, int * rx_buf, int n)
{
    spiSelect(spip);
    spiStartSend(spip,1,&address);
    while((*spip).state != SPI_READY) {}


    spiStartReceive(spip,n,rx_buf);
    while((*spip).state != SPI_READY) {}

    spiUnselect(spip);   


}

uint8_t spi_read_reg(SPIDriver *spip, int address)
{

    spi_read(spip,address,rxbuf,8);

    return(rxbuf[0]);    

    
}

void spi_write(SPIDriver * spip, int address, int * tx_buf, int n)
{

    //spiSelect(spip);

    //spiStartSend(spip,1,&address);
    spiSend(spip,1,&address); // Transmitting one word.
    //while((*spip).state != SPI_READY) {}

    //spiStartSend(spip,n,txbuf);
    spiSend(spip,n,txbuf);    // Transmitting entire transfer buffer.
    //while((*spip).state != SPI_READY) {}

    //spiUnselect(spip);


}


void spi_write_reg(SPIDriver *spip, int address, int newval)
{
    txbuf[0] = newval;
    spi_write(spip,address, txbuf,1);

}

//! @}
