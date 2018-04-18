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
#include "oresat.h"

#include "thread1.h"

#define PWM_TIMER_FREQ 20e3     // 48k Hz
#define PWM_FREQ 2           // periods per second.

#define PWM_PERIOD PWM_TIMER_FREQ/PWM_FREQ

#define PWM_U 0U        // Software channel

#define DEBUG_SERIAL SD2
#define DEBUG_CHP ((BaseSequentialStream *) &DEBUG_SERIAL)

static void pwmCallback(uint8_t channel)
{
  //palSetLine(8u);
  /*
  pwmEnableChannel(
                &PWMD1,
                channel,
                PWM_PERCENTAGE_TO_WIDTH(&PWMD1,5000)
      );
  */
  //chprintf(DEBUG_CHP,"pwmCallback \n");

}

static void pwmpcb(PWMDriver *pwmp) {
  (void)pwmp;

  //palClearLine(8u);
}

static PWMConfig pwm_config = {
    PWM_TIMER_FREQ,     // Frequency
    PWM_PERIOD,         // Period
    NULL,               // Callback
    {                   // Channels
      {PWM_OUTPUT_ACTIVE_HIGH,pwmpcb},
      //{PWM_OUTPUT_ACTIVE_HIGH,NULL},
      {0},
      {0},
      {0}
    },
    0,
    0
};



//=== Serial configuration
static SerialConfig ser_cfg =
{
    115200,     //Baud rate
    0,          //
    0,          //
    0,          //
};


static void app_init(void) {
    //=== App initialization
  

    pwmStart(&PWMD1,&pwm_config);
    
    pwmEnableChannel(
                &PWMD1,
                0U,
                PWM_PERCENTAGE_TO_WIDTH(&PWMD1,5000)
      );


    // Start up debug output
    sdStart(&SD2, &ser_cfg);
    chprintf(DEBUG_CHP,"Hello wrld!\n");

}

static void main_app(void) {
    //=== Start application threads

    //Example thread creation
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /*
     * Begin main loop
     */
    while (true)
    {
        
        chThdSleepMilliseconds(5000);
        if(test == 1)
        chprintf(DEBUG_CHP,"Hello wrld!\n");
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
    oresat_init(0);

    // Initialize and start app
    app_init();
    main_app();

    return 0;
}
