#include "acs.h"
#include "acs_lut.h"


// static virtual_timer_t speed_vt;

// static sysinterval_t interval = 0;
//static uint32_t anglePrev, angleCurr;
//static uint32_t angleDelta = 0;
//static uint32_t rps = 0;
//static uint32_t time = 0;

//static  systime_t now;
//static  systime_t prv;

BldcConfig bldc;
uint16_t rxbuf[2] = {0}; // receive buffer

/*
static void gpt4cb(GPTDriver *gptp) {
  (void)gptp;
  // Perform your action here

  anglePrev = angleCurr;
  angleCurr = bldc.position;
  angleDelta = 5;//angleCurr - anglePrev;

  rps = angleDelta * 10;

 
  gptStartOneShotI(&GPTD14, 100000); // ~100 ns delay, then stop timer
}

static const GPTConfig gpt4cfg = {
  100000, // 1 MHz timer clock.
  gpt4cb, // No callback
  0, 0
};*/

/*
static void speed_cb(void * arg)
{
  //chVTSetI(&speed_vt, TIME_MS2I(100), speed_cb, NULL);

  anglePrev = angleCurr;
  angleCurr = bldc.position;
  angleDelta = angleCurr - anglePrev;

  rps = angleDelta * 10;
}*/

static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
    (void)adcp;
    (void)err;
}

/*
 * ADC conversion group.
 * Mode:        Continuous, 8 samples of 1 channel, SW triggered.
 * Channels:    A0 
 */
static const ADCConversionGroup adcgrpcfg = {
    TRUE,
    ADC_GRP_NUM_CHANNELS,
    NULL,
    adcerrorcallback,
    ADC_CFGR1_CONT | ADC_CFGR1_RES_12BIT,             /* CFGRR1 */
    ADC_TR(0, 0),                                     /* TR */
    ADC_SMPR_SMP_239P5,                                /* SMPR */
    ADC_CHSELR_CHSEL0                                /* CHSELR */
};


uint32_t step_size = STRETCH;

uint32_t current_sin_u, next_sin_u;
uint32_t current_sin_v, next_sin_v;
uint32_t current_sin_w, next_sin_w;
uint16_t sin_diff;
uint16_t sin_size;
uint16_t sin_step = STRETCH;

THD_WORKING_AREA(wa_acsThread,THREAD_SIZE);
THD_FUNCTION(acsThread,arg) {
  (void)arg;
  chRegSetThreadName("acsThread");

	bldcStart();

  while (!chThdShouldTerminateX()) {
    
		chprintf(DEBUG_CHP,"enc pos: %u \n\r", bldc.position);
 	  chprintf(DEBUG_CHP,"phase 1: %u \n\r", bldc.u);     
		chprintf(DEBUG_CHP,"phase 2: %u \n\r", bldc.v);     
		chprintf(DEBUG_CHP,"phase 3: %u \n\r", bldc.w);     
    chprintf(DEBUG_CHP,"step_size: %u \n\n\r", step_size);
    chThdSleepMilliseconds(500); 

    chprintf(DEBUG_CHP, "ADC: %u \n\r", (int)bldc.samples[0]);
   
  }
}

THD_WORKING_AREA(wa_spiThread,THREAD_SIZE);
THD_FUNCTION(spiThread,arg){
  (void)arg;
  chRegSetThreadName("spiThread");

  spiStart(&SPID1, &spicfg);            // Start driver.
  spiAcquireBus(&SPID1);                // Gain ownership of bus.

  while (!chThdShouldTerminateX()) {
		rxbuf[0] = 0; 
		spiSelect(&SPID1);                  // Select slave.

		spiReceive(&SPID1,1,rxbuf);     		// Receive 1 frame (16 bits).
		
    spiUnselect(&SPID1);                // Unselect slave.
    bldc.position = 0x3FFF & rxbuf[0];
	 
		chThdSleepMicroseconds(1);
  }

	spiReleaseBus(&SPID1);    // Release ownership of bus.
	spiStop(&SPID1);          // Stop driver.
}


THD_WORKING_AREA(wa_debugThread,THREAD_SIZE);
THD_FUNCTION(debugThread,arg) {
  (void)arg;
  chRegSetThreadName("debugThread");
 //chThdSleepMilliseconds(100);
}

/*
  Sections off value from encoder (0 - 2^14)
  into chunks of 0-360 for the LUT.
  TODO MAX: clean this up.
*/

static uint16_t chunk0L = 0 * ENCODER_CHUNK_SIZE;
static uint16_t chunk0H = 1 * ENCODER_CHUNK_SIZE;

static uint16_t chunk1L = 1 * ENCODER_CHUNK_SIZE;
static uint16_t chunk1H = 2 * ENCODER_CHUNK_SIZE;

static uint16_t chunk2L = 2 * ENCODER_CHUNK_SIZE;
static uint16_t chunk2H = 3 * ENCODER_CHUNK_SIZE;

static uint16_t chunk3L = 3 * ENCODER_CHUNK_SIZE;
static uint16_t chunk3H = 4 * ENCODER_CHUNK_SIZE;

static uint16_t chunk4L = 4 * ENCODER_CHUNK_SIZE;
static uint16_t chunk4H = 5 * ENCODER_CHUNK_SIZE;

static uint16_t chunk5L = 5 * ENCODER_CHUNK_SIZE;
static uint16_t chunk5H = 6 * ENCODER_CHUNK_SIZE;


// Uses above defines to translate encoder feedback
// into the proper LUT index.
static uint16_t encoderToLUT(uint16_t position)
{
  uint16_t step = 0;
  if (position > chunk0L && position < chunk0H)
  {
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }
  else if (position > chunk1L && position < chunk1H)
  {
    position = position - (ENCODER_CHUNK_SIZE*1);
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }
  else if (position > chunk2L && position < chunk2H)
  {

    position = position - (ENCODER_CHUNK_SIZE*2); 
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }
  else if (position > chunk3L && position < chunk3H)
  {

    position = position - (ENCODER_CHUNK_SIZE*3);
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }
  else if (position > chunk4L && position < chunk4H)
  {

    position = position - (ENCODER_CHUNK_SIZE*4);
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }
  else if (position > chunk5L && position < chunk5H)
  {

    position = position - (ENCODER_CHUNK_SIZE*5);
    step = position * STEPS / (ENCODER_CHUNK_SIZE);
  }

  return step;
}

//static uint32_t LUTrevCount = 0;

// TODO Max, instead of increments of 10%, get finer control.
static sinctrl_t scale(sinctrl_t duty_cycle){
	return (duty_cycle*bldc.scale)/10;	
}

// Temporary for ramping function, hopefully can simulate this with CAN
uint16_t count = 0;

bool down = false;


// TODO Max, find a way to separate the types of control
// easily with CAN. Probably a simple case statement

// pwm period callback
static void pwmpcb(PWMDriver *pwmp) {
  (void)pwmp;

 	uint16_t step; 
  uint16_t next_step;
  palClearLine(LINE_LED_GREEN);
  
  if (count > 2000)
  {
    if (!down)
    {
      step_size++;
    }
    else
    {
      step_size = step_size - 1;
    }
    count = 0;
  }

  if (step_size >= 80)
  {
    down = true;
  }
  else if (step_size < 2)
  {
    down = false;
  }

// TODO Max, do we want the configure stuff for any reason?
#ifdef CONFIGURE

    if (LUTrevCount < 6 * 360)
    {
      if (bldc.count == bldc.stretch){
      bldc.u += STEP_SIZE;
      bldc.v += STEP_SIZE;
      bldc.w += STEP_SIZE;

      LUTrevCount++;

      if(bldc.u >= bldc.steps){
			  bldc.u = 0 + bldc.u - bldc.steps;
		  }
		  if(bldc.v >= bldc.steps){
			  bldc.v = 0 + bldc.v - bldc.steps;
		  }
		  if(bldc.w >= bldc.steps){
			  bldc.w = 0 + bldc.w - bldc.steps;
		  }
      bldc.count = 0;
     }
     ++bldc.count;
    }
    else
    {
      bldc.u = 0;
      bldc.v = 120;
      bldc.w = 240;
    }
    
  
#endif

#ifndef CONFIGURE    
#ifdef BRUTEFORCE
		  bldc.u += step_size;
		  bldc.v += step_size;
		  bldc.w += step_size;

		  if(bldc.u >= bldc.steps){
			  bldc.u = 0 + bldc.u - bldc.steps;
		  }
		  if(bldc.v >= bldc.steps){
			  bldc.v = 0 + bldc.v - bldc.steps;
		  }
		  if(bldc.w >= bldc.steps){
			  bldc.w = 0 + bldc.w - bldc.steps;
		  }
#endif
#ifndef BRUTEFORCE
// This is the encoder feedback.
// TODO Max, this really needs to be cleaned up.
if (sin_step == step_size)
{
      step = encoderToLUT(bldc.position);
		  next_step = step + SKIP;



      bldc.u = (next_step)%360;
		  bldc.v = (bldc.u + bldc.phase_shift)%360;
		  bldc.w = (bldc.v + bldc.phase_shift)%360;

}
#endif
#endif
if (SKIP)
{
if (sin_step == step_size)
{
  current_sin_u = bldc.sinctrl[bldc.u];
  current_sin_v = bldc.sinctrl[bldc.v];
  current_sin_w = bldc.sinctrl[bldc.w];
  next_sin_u = bldc.sinctrl[bldc.u + 1];
  next_sin_v = bldc.sinctrl[bldc.v + 1];
  next_sin_w = bldc.sinctrl[bldc.w + 1];
  
  if (current_sin_u < next_sin_u)
  {
    sin_diff = next_sin_u - current_sin_u;

    sin_size = sin_diff / step_size;
  }
  else if (current_sin_v < next_sin_v)
  {
    sin_diff = next_sin_v - current_sin_v;
    sin_size = sin_diff / step_size;
  }  
  else
  {
    sin_diff = next_sin_w - current_sin_w;
    sin_size = sin_diff / step_size;
  }
}

current_sin_u = current_sin_u + sin_size;
current_sin_v = current_sin_v + sin_size;
current_sin_w = current_sin_w + sin_size;

sin_step = sin_step - 1;

if (sin_step == 0)
{
  current_sin_u = next_sin_u;
  current_sin_v = next_sin_v;
  current_sin_w = next_sin_w;
  sin_step = step_size;
}
}
else
{
  current_sin_u = bldc.sinctrl[bldc.u];
  current_sin_v = bldc.sinctrl[bldc.v];
  current_sin_w = bldc.sinctrl[bldc.w];
}

pwmEnableChannelI(
		&PWMD1,
		PWM_U,
		PWM_PERCENTAGE_TO_WIDTH(&PWMD1,scale(current_sin_u)));

pwmEnableChannelI(
		&PWMD1,
		PWM_V,
		PWM_PERCENTAGE_TO_WIDTH(&PWMD1,scale(current_sin_v))
	);

pwmEnableChannelI(
		&PWMD1,
		PWM_W,
		PWM_PERCENTAGE_TO_WIDTH(&PWMD1,scale(current_sin_w))
	);

  count++;
}





static void pwmCallback(uint8_t channel,sinctrl_t step){
 // palSetLine(LINE_LED_GREEN);
  pwmEnableChannelI(
		&PWMD1,
		channel,
		PWM_PERCENTAGE_TO_WIDTH(&PWMD1,scale(bldc.sinctrl[step]))
	);
  
}

static void pwmUcb(PWMDriver *pwmp){ // channel 1 callback
  (void)pwmp;
	//pwmCallback(PWM_U,bldc.u);
}

static void pwmVcb(PWMDriver *pwmp){ // channel 2 callback
  (void)pwmp;
	//pwmCallback(PWM_V,bldc.v);
}

static void pwmWcb(PWMDriver *pwmp){ // channel 3 callback
  (void)pwmp;
	//pwmCallback(PWM_W,bldc.w);
}

static PWMConfig pwmcfg = {
  PWM_TIMER_FREQ,	
  PWM_PERIOD,	
  pwmpcb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH|PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH,pwmUcb},
   {PWM_OUTPUT_ACTIVE_HIGH|PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH,pwmVcb},
   {PWM_OUTPUT_ACTIVE_HIGH|PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH,pwmWcb},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
	0,
  0
};

extern void acsInit(ACSdata *data){
	bldc.data = data;
	bldcInit();
  // TODO MAX wrap up into adcInit
  adcStart(&ADCD1, NULL);

  /*
  * Starts an ADC continuous conversion.
  */
  adcStartConversion(&ADCD1, &adcgrpcfg, bldc.samples, ADC_GRP_BUF_DEPTH);

  
  //gptStart(&GPTD14, &gpt4cfg);
  //gptStartOneShot(&GPTD14, 100000); // ~100 ns delay, then stop timer



  //chVTObjectInit(&speed_vt);
  //chVTSet(&speed_vt, TIME_MS2I(100), speed_cb, NULL);
}

extern void bldcInit(){
	bldc.steps = STEPS;
	bldc.stretch = STRETCH;
	bldc.scale = SCALE;
	bldc.sinctrl = sinctrl360;
	bldc.count = 0;
  bldc.position = 0;
  bldc.phase_shift = bldc.steps/3;
  bldc.u = 0;
  bldc.v = bldc.u + bldc.phase_shift;
  bldc.w = bldc.v + bldc.phase_shift;
}



extern void bldcStart(){
	palSetLine(LINE_LED_GREEN);
	pwmStart(&PWMD1,&pwmcfg);
  pwmEnablePeriodicNotification(&PWMD1);

	pwmEnableChannel(&PWMD1,PWM_U,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.u));
  pwmEnableChannel(&PWMD1,PWM_V,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.v));
  pwmEnableChannel(&PWMD1,PWM_W,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.w));

	pwmEnableChannelNotification(&PWMD1,PWM_U);
  pwmEnableChannelNotification(&PWMD1,PWM_V);
  pwmEnableChannelNotification(&PWMD1,PWM_W);
}

extern void bldcStop(){
  palClearLine(LINE_LED_GREEN);
	pwmStop(&PWMD1);
/*
  pwmEnablePeriodicNotification(&PWMD1);

	pwmEnableChannel(&PWMD1,PWM_U,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.u));
  pwmEnableChannel(&PWMD1,PWM_V,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.v));
  pwmEnableChannel(&PWMD1,PWM_W,PWM_PERCENTAGE_TO_WIDTH(&PWMD1,bldc.w));

	pwmEnableChannelNotification(&PWMD1,PWM_U);
  pwmEnableChannelNotification(&PWMD1,PWM_V);
  pwmEnableChannelNotification(&PWMD1,PWM_W);
*/
}
