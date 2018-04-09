#ifndef _ACS_FEEDBACK_H_
#define _ACS_FEEDBACK_H_


#include "ch.h"
#include "hal.h"

static const SPIConfig spicfg = {
    false,              // Enables circular buffer if == 1
    NULL,               // Operation complete call back.
    GPIOA,              // Chip select line
    GPIOA_SPI1_NSS,     // Chip select port
    SPI_CR1_BR_0 |  SPI_CR1_BR_1 | SPI_CR1_BR_2 | SPI_CR1_CPHA, 	// SPI control register 1 mask. 
    SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3,	// SPI control register 2 mask.
};

#endif

