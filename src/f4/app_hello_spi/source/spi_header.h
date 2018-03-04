/*
 *
 *
 * Header file for encoder SPI program.
 *
 *
 */

#define SETTING_1 0x89
#define SETTING_2 0x03

uint8_t       sx_txbuff[8];
uint8_t       sx_rxbuff[8];

struct reg_addrs
{
    int no_op;       // No operation
    int err;       // Error register
    int program;       // Programming register
    int diag;       // Diagnostic and AGC
    int magnitude;       // CORDIC Magnitude
    int angl_no_err;       // Measured angle with no error compensation.
    int angl_err;       // Measured angle with error compensation.
    int z_pos_msb;       // Zero position msb.
    int z_pos_lsb;       // Zero position lsb.
    int settings_1;       // Custom settings register 1
    int settings_2;       // Custom settings register 2
};



// sx1236_read
void spi_read(SPIDriver * spip, int address, uint8_t * rx_buf, uint8_t n);

// write
void spi_write(SPIDriver * spip, uint8_t address, uint8_t * tx_buf, uint8_t n);

// sx1236_read reg
uint8_t spi_read_reg(SPIDriver *spip, int address);

// write reg
void spi_write_reg(SPIDriver *spip, uint8_t address, uint8_t newval);



