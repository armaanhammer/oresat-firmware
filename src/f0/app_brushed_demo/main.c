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

//#define PWM_TIMER_FREQ 48e3     // 48k Hz
#define PWM_TIMER_FREQ 48e3     // 48k Hz
//#define PWM_FREQ 3e3           // periods per second.
#define PWM_FREQ 48           // periods per second.

#define PWM_PERIOD PWM_TIMER_FREQ/PWM_FREQ

#define PWM_SW_CHANNEL 0        // Software channel

#define DEBUG_SERIAL SD2
#define DEBUG_CHP ((BaseSequentialStream *) &DEBUG_SERIAL)

/*
static void pwmCallback(uint8_t channel)
{
  //palSetLine(8u);
  //chprintf(DEBUG_CHP,"pwmCallback \n");

}
*/

static void pwmpcb(PWMDriver *pwmp) {
  (void)pwmp;

  //palClearLine(8u);
}

static PWMConfig pwm_config = {
    //PWM_TIMER_FREQ,     // Frequency
    2000000,       // Frequency Hz
    //PWM_PERIOD,         // Period
    1000,         // Period
    NULL,               // Callback
    {                   // Channels
      {PWM_OUTPUT_ACTIVE_HIGH,pwmpcb},
      {PWM_OUTPUT_DISABLED,NULL},
      {PWM_OUTPUT_DISABLED,NULL},
      {PWM_OUTPUT_DISABLED,NULL}
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
    
    //palClearPad(GPIOB,6u);    // Phase selection (direction of motor).
    palSetPad(GPIOB,7u);        // Set Enable high.

    // Start up debug output
    sdStart(&SD2, &ser_cfg);

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
      palSetPad(GPIOB,6u);    // Phase selection (direction of motor).
      pwmEnableChannel(
                &PWMD1,
                PWM_SW_CHANNEL,
                PWM_PERCENTAGE_TO_WIDTH(&PWMD1,4000)
        );

    
      chThdSleepMilliseconds(5000);
      pwmDisableChannel(&PWMD1,PWM_SW_CHANNEL);

      palClearPad(GPIOB,6u);    // Phase selection (direction of motor).
      pwmEnableChannel(
                &PWMD1,
                PWM_SW_CHANNEL,
                PWM_PERCENTAGE_TO_WIDTH(&PWMD1,8000)
        );

      chThdSleepMilliseconds(5000);
      pwmDisableChannel(&PWMD1,PWM_SW_CHANNEL);
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

    // test ports for motor phase and driver enable.
    palSetPadMode(GPIOB,6u,PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB,7u,PAL_MODE_OUTPUT_PUSHPULL);

    // Initialize and start app
    app_init();
    main_app();

    return 0;
}
