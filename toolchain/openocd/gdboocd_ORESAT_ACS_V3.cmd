target remote | openocd -s ../../../toolchain/openocd -f oocd_ORESAT_ACS_V3.cfg -c "gdb_port pipe"
monitor stm32f0x.cpu configure -rtos ChibiOS
monitor reset halt
