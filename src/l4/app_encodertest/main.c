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

/* ChibiOS header files */
#include "ch.h"
#include "hal.h"

/* Project header files */
#include "oresat.h"
#include "encodertest.h"

static worker_t worker1;

static oresat_config_t oresat_conf = {
    &CAND1,
    ORESAT_DEFAULT_ID,
    ORESAT_DEFAULT_BITRATE
};

/**
 * @brief App Initialization
 */
static void app_init(void)
{
    /* App initialization */
    // true means critical worker, so it will start regardless of the system state
    init_worker(&worker1, "Example blinky thread", wa_spiThread, sizeof(wa_spiThread), NORMALPRIO, spiThread, NULL, true); 
    reg_worker(&worker1);

    /* Start up debug output */
    sdStart(&LPSD1, NULL);
}

/**
 * @brief Main Application
 */
int main(void)
{
	// inits from ACS-1.0
	halInit();
	chSysInit();
    
    
    // Initialize and start
    oresat_init();
    app_init();
    oresat_start(&oresat_conf);
    return 0;
}
