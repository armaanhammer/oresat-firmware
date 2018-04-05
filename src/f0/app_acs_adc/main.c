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
#include <stdint.h>

//=== Project header files
//#include "can.h"
#include "acs_adc.h"


#define DEBUG_SERIAL SD2
#define DEBUG_CHP ((BaseSequentialStream *) &DEBUG_SERIAL)

//=== Serial configuration
static SerialConfig ser_cfg = {
	115200,     //Baud rate
	0,          //
	0,          //
	0,          //
};


#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      1

#define ADC_GRP2_NUM_CHANNELS   1
#define ADC_GRP2_BUF_DEPTH      8


static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];
static adcsample_t samples2[ADC_GRP2_NUM_CHANNELS * ADC_GRP2_BUF_DEPTH];

/*
 * ADC streaming callback.
 */
long pc0 = 0;
long pc0_volt = 0;

size_t nx = 0, ny = 0;
static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
    (void)adcp;

    pc0 = samples2[0];
    pc0_volt = (pc0 * 333) / 4095;
}

static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
    (void)adcp;
    (void)err;
}

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN10.
 */
static const ADCConversionGroup adcgrpcfg1 = {
    FALSE,
    ADC_GRP1_NUM_CHANNELS,
    NULL,
    adcerrorcallback,
    ADC_CFGR1_RES_12BIT,                              /* CFGRR1 */
    ADC_TR(0, 0),                                     /* TR */
    ADC_SMPR_SMP_239P5,                                 /* SMPR */
    ADC_CHSELR_CHSEL0                                /* CHSELR */
};

/*
 * ADC conversion group.
 * Mode:        Continuous, 16 samples of 1 channel, SW triggered.
 * Channels:    IN10, IN11, Sensor, VRef.
 */
static const ADCConversionGroup adcgrpcfg2 = {
    TRUE,
    ADC_GRP2_NUM_CHANNELS,
    adccallback,
    adcerrorcallback,
    ADC_CFGR1_CONT | ADC_CFGR1_RES_12BIT,             /* CFGRR1 */
    ADC_TR(0, 0),                                     /* TR */
    ADC_SMPR_SMP_239P5,                                /* SMPR */
    ADC_CHSELR_CHSEL0                                /* CHSELR */
};

// ADC control thread
// TODO MAX, stick ADC stuff in the thread? Maybe not needed honestly.
static THD_WORKING_AREA(wa_adcThread,128);
static THD_FUNCTION(adcThread,arg){
  (void)arg;
  chRegSetThreadName("adc");
  
  while(!chThdShouldTerminateX()){
    chThdSleepMilliseconds(500);
  }
}

static void app_init(void){
	// Start up debug output
	sdStart(&SD2, &ser_cfg);
}

static void app_main(void){
	chThdCreateStatic(
		wa_adcThread,
		sizeof(wa_adcThread), 
		NORMALPRIO, 
		adcThread, 
		NULL
	);

	/*
	 * Begin main loop
	 */
	while (true){
    chprintf(DEBUG_CHP, "PA0: %d \n\r", pc0_volt);
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

    /*
    * Activates the ADC1 driver and the temperature sensor.
    */
    adcStart(&ADCD1, NULL);

    /*
    * Linear conversion.
    */
    // TODO MAX implement one shot, although we probably want continuous

    //adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
    

    /*
    * Starts an ADC continuous conversion.
    */
    adcStartConversion(&ADCD1, &adcgrpcfg2, samples2, ADC_GRP2_BUF_DEPTH);



    chThdSleepMilliseconds(1000);
	app_main();

	return 0;
}



//! @}
