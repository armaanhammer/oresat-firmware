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


/**
 * @brief Control structure used to configure the SPI driver
 *
 * GPIOA_SPI1_NSS is the pin used to initially select the SPI slave.
 * The mask for SPI Control Register 1 sets the frequency of data transfers
 * and sets the clock polarity.
 * The mask for SPI control Register 2 sets the size of the transfer buffer, 16 bits.
 *
 */
static const SPIConfig spicfg = {
	false,                                                  // Not using circular buffer.
	NULL,                                                   // Not using operation complete callback.
	GPIOA,                                                  // Chip select line.
	GPIOA_SPI1_NSS,                                         // Chip select port.
	SPI_CR1_BR_0|SPI_CR1_BR_1|SPI_CR1_BR_2|SPI_CR1_CPHA,    // SPI Control Register 1 mask.
	SPI_CR2_DS_0|SPI_CR2_DS_1|SPI_CR2_DS_2|SPI_CR2_DS_3,    // SPI Control Register 2 mask.
};


extern THD_WORKING_AREA(wa_spiThread,THREAD_SIZE);
/// Prototype for spi thread function.
extern THD_FUNCTION(spiThread,arg);

#endif
