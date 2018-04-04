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
 *
 * Andrew Capatina
 *
 * This code reads the value reported from the 
 * AS5047P encoder. The HW configuration used
 * is unidirectional SPI.
 *
 *
 */


//=== ChibiOS header files
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

//=== Project header files
#include "can.h"
#include "spi_header.h"

#define DEBUG_SERIAL SD2
#define DEBUG_CHP ((BaseSequentialStream *) &DEBUG_SERIAL)

/*
static uint16_t no_op = 0x0000;       // No operation
static uint16_t err  = 0x0001;       // Error register
static uint16_t program      = 0x0003;       // Programming register
static uint16_t diag         = 0x3FFC;       // Diagnostic and AGC   
static uint16_t magnitude    = 0x3FFD;       // CORDIC Magnitude
static uint16_t angl_no_err  = 0x3FFE;       // Measured angle with no error compensation.
static uint16_t angl_err     = 0x3FFF;       // Measured angle with error compensation.
static uint16_t z_pos_msb    = 0x0016;       // Zero position msb.
static uint16_t z_pos_lsb    = 0x0017;       // Zero position lsb.
static uint16_t settings_1   = 0x0018;       // Custom settings register 1
static uint16_t settings_2   = 0x0019;       // Custom settings register 2
*/


/*
 * Maximum speed SPI configuration (21MHz, CPHA=1, CPOL=0, MSb first).
 */
static const SPIConfig spicfg = {
    false,              // Enables circular buffer if == 1
    NULL,               // Operation complete call back.
    GPIOA,              // Chip select line
    GPIOA_SPI1_NSS,     // Chip select port
    // SPE: Enable SPI peripheral / MSTR: Set MCU as master. / CPHA: Second clock phase data is ready / DFF: 16 bit data frames.
    SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_SPE | SPI_CR1_CPHA | SPI_CR1_RXONLY,  //| SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3, 
    0,                  // Chip select port mask
};


/*
 * SPI RX buffer.
 * Use 16 bit buffers for receive and transmit. 
 */
static uint16_t rxbuf[8];
static uint16_t  rxbuf_2[8];


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
  char to_disp[30] = {0};

  
      spiAcquireBus(&SPID1);                    // Gain ownership of bus.
      spiStart(&SPID1, &spicfg);                // Start driver.
  while(true)
  {
      rxbuf[0] = 0;

      spiSelect(&SPID1);                        // Select slave.

      while(SPID1.state != SPI_READY) {}        // Waiting for driver state to be ready.
      //SPI1->CR2 |= 0xE00;
      SPI1->CR2 |= 0xF00;
      spiReceive(&SPID1,2,rxbuf);               // Receive 1 frame (8 bits).
      spiUnselect(&SPID1);                      // Unselect slave.
    
      rxbuf[0] = rxbuf[0] & 0x3FFF;             
      //itoa(rxbuf[0],to_disp,2); 
      

      // Display results
      //chprintf(DEBUG_CHP,"Decimal: %u\n", rxbuf[0]);        
      chprintf(DEBUG_CHP,"Hex: %x\n", rxbuf[0]);        

      chThdSleepMilliseconds(1000);


  }

      spiStop(&SPID1);          // Stop driver.
      spiReleaseBus(&SPID1);    // Release ownership of bus.

}

static void app_init(void) {
    //=== App initialization
    
    int i;

    for(i = 0; i < 8; ++i)      // Initializing receive buffer to zero. 
      rxbuf_2[i] = 0;


    // Initializing SPI pins. 
    
   /* 
    palSetPadMode(GPIOA,GPIOA_SPI1_NSS,PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);// New NSS.
    palSetPadMode(GPIOA,GPIOA_SPI1_SCK,PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   // New SCK.
    palSetPadMode(GPIOA,GPIOA_SPI1_MISO,PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);  // New MISO.

*/


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

/*
static void main_app(void) {
    //=== Start application threads

    chThdSleepMilliseconds(500);



    
    //Begin main loop
     
    while (true)
    {

        chThdSleepMilliseconds(1000);
    }
}
*/ 
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

/*
 * Following functions are not needed. May be needed if we implement bidirectional comms.
 *
 */
/*-------------------------------------------------------------------------
void spi_read(SPIDriver * spip, uint16_t address, uint16_t * rx_buf, int n)
{
    spiSelect(spip);
    spiSend(spip,1,&address);
    //spiStartSend(spip,1,&address);
    while((*spip).state != SPI_READY) {}


    //spiStartReceive(spip,n,rx_buf);
    spiReceive(spip,n,rx_buf);
    while((*spip).state != SPI_READY) {}

    spiUnselect(spip);   


}

uint8_t spi_read_reg(SPIDriver *spip, uint16_t address)
{

    spi_read(spip,address,rxbuf,sizeof(uint16_t));

    return(rxbuf[0]);    

    
}

void spi_write(SPIDriver * spip, uint16_t address, uint16_t * tx_buf, int n)
{
    spiSelect(spip);

    //spiStartSend(spip,1,&address);
    spiSend(spip,1,&address); // Transmitting one word.
    //while((*spip).state != SPI_READY) {}

    //spiStartSend(spip,n,txbuf);
    spiSend(spip,n,txbuf);    // Transmitting entire transfer buffer.
    //while((*spip).state != SPI_READY) {}

    spiUnselect(spip);

}


void spi_write_reg(SPIDriver *spip, uint16_t address, int newval)
{
    txbuf[0] = newval;
    spi_write(spip,address, txbuf,1);

}


----------------------------------------------------------------------------*/


//! @}
