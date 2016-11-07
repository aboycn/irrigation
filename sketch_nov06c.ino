/*
 * Sketch for testing sleep mode with wake up on WDT.
 * Donal Morrissey - 2011.
 *
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define LED_PIN (13)

// state 0: sleeping
// state 1: watering
volatile int state = 0;
volatile int nap_count = 0;

// Duration of one sleep in seconds.
const int kSleepDuration = 16;

// Duration of one watering in seconds.
const int kWaterDuration = 8;

// Number of seconds of each nap (i.e. one watchdog timer).
const int kNapDuration = 8;

// Sleep and watering duration in naps.
const int kSleepNaps = kSleepDuration / kNapDuration;
const int kWaterNaps = kWaterDuration / kNapDuration;


/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect)
{
  nap_count++;
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  /* Now enter sleep mode. */
  sleep_mode();

  /* The program will continue from here after the WDT timeout*/
  /* First thing to do is disable sleep. */
  sleep_disable();
}



/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the serial comms and the
 *                Watch dog timeout. 
 *
 ***************************************************/
void setup()
{
  Serial.begin(9600);
  Serial.println("Initialising...");
  delay(100); //Allow for serial print to complete.

  // Setup LED.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  
  Serial.println("Initialisation complete.");
  delay(100); //Allow for serial print to complete.
}



/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  enterSleep();

  if (state == 0) {
    if (nap_count >= kSleepNaps) {
      // Enough sleep, switch to watering state.
      digitalWrite(LED_PIN, HIGH);
      nap_count = 0;
      state = 1;
    }
  } else if (state == 1) {
    if (nap_count >= kWaterNaps) {
      // Enough water, wwitch to sleeping state.
      digitalWrite(LED_PIN, LOW);
      nap_count = 0;
      state = 0;
    }
  } else {
    state = 0;
  }

//  /* Re-enable the peripherals. */
//  power_all_enable();
}

