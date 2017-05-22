
/** ---------- PCA9548A Library Test Code ------------------------

Copyright 2017 Systronix Inc www.systronix.com

**/
 
/** ---------- REVISIONS ----------

2017Mar06 bboyes  Start, based on TMP275 example
--------------------------------**/

/** ---------- TODO ----------

2017Mar06 bboyes  pick a license to use and put it in here. 
My head hurts every time I try to figure out which is "best" for us.
--------------------------------**/

#include <Arduino.h>
#include <Systronix_PCA9548A.h>	// best version of I2C library is #included by the library. Don't include it here!


 /**
 * debug level
 * 0 = quiet, suppress everything
 * 3 = max, even more or less trivial message are emitted
 * 4 = emit debug info which checks very basic data conversion, etc
 */
 byte DEBUG = 3;

uint16_t dtime = 1000;  // delay in loop

uint8_t config_value;


Systronix_PCA9548A PCA9548A_70(PCA9548A_SLAVE_ADDR_0);    

const char * text_ptr;

uint32_t startuptime = 0;
uint32_t new_millis = 0;
uint32_t old_millis = 0;
uint32_t old_sec_millis = 0;

uint32_t new_elapsed_seconds = 0;
uint32_t total_elapsed_seconds = 0;
boolean seconds_tick = false;

boolean seconds_ten_tick = false;
boolean minute_tick = false;
boolean minute_ten_tick = false;

boolean verbose = false;
boolean new_errors = false;

uint8_t inbyte = 0;

uint32_t old_total_error_count = 0;



/* ========== SETUP ========== */
void setup(void) 
{


  uint8_t stat = 0;
  uint16_t iter = 0;

  pinMode(LED_BUILTIN,OUTPUT);    // LED

  
  Serial.begin(115200);     // use max baud rate
  // Teensy3 doesn't reset with Serial Monitor as do Teensy2/++2, or wait for Serial Monitor window
  // Wait here for 10 seconds to see if we will use Serial Monitor, so output is not lost
  while((!Serial) && (millis()<10000));    // wait until serial monitor is open or timeout, which seems to fall through
 
  Serial.printf("\r\nPCA9548A Library Test Code at 0x%.2X\r\n", PCA9548A_70.base_address());

  Serial.printf("Build %s - %s\r\n%s\r\n", __DATE__, __TIME__, __FILE__);

#if defined(__MKL26Z64__)
  Serial.println( "CPU is T_LC");
#elif defined(__MK20DX256__)
  Serial.println( "CPU is T_3.1/3.2");
#elif defined(__MK20DX128__)
  Serial.println( "CPU is T_3.0");
#elif defined(__MK64FX512__)
  Serial.println( "CPU is T_3.5");
#elif defined(__MK66FX1M0__)
  Serial.println( "CPU is T_3.6");
#endif
  Serial.print( "F_CPU =");   Serial.println( F_CPU );
  
#if defined I2C_T3_H 
  Serial.printf("Using i2c_t3 I2C library for Teensy\r\n");
#endif

  uint64_t uint64_test = 0;
  uint64_test = UINT64_MAX;
  // Serial.println(uint64_test);
  uint64_test++;
  // Serial.println(uint64_test);
  Serial.printf("More than 32 bits? %llu, 0x%llX\r\n", uint64_test, uint64_test);
  Serial.printf ("UINT32_MAX=0x%X, UINT64_MAX=0x%llX\r\n", UINT32_MAX, UINT64_MAX);
  Serial.printf ("UINT64_MAX=0x%llX, UINT32_MAX=0x%X\r\n", UINT64_MAX, UINT32_MAX);
  Serial.printf ("UINT32_MAX=%u, UINT64_MAX=%llu\r\n", UINT32_MAX, UINT64_MAX);
  if (0xFFFFFFFFFFFFFFFE < ULLONG_MAX) Serial.println ("Test of ULLONG_MAX seems to pass");
  if (0xFFFFFFFFFFFFFFFE < UINT64_MAX) Serial.println ("Test of UINT64_MAX seems to pass");
  
  // start PCA9548A library
  PCA9548A_70.begin();

  config_value = PCA9548A_CHAN_0_ENABLE;  // 

  // initialize MUX, don't proceed unless succeeds
  do
  {
    iter++;
  	stat = PCA9548A_70.init(config_value);
    Serial.printf(" Attempt #%u: Init control reg to 0x%.2X - ", iter, config_value); 
    if (SUCCESS != stat)
    {
      text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
      Serial.printf("failed - returned 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
#if defined I2C_T3_H 
    // reset I2C just to be safe
    // we may have interrupted a message in process of loading new code or Teensy reset
    Wire.resetBus();
    Serial.printf("Tried resetBus()\r\n");
#else
    // TODO toggle SDA 10X while SCL is high to force any incomplete I2C message to finish
    // how to do this without clobbering I2C library already set up?
    Serial.printf("TODO: resetBus() for Wire library!\r\n");
#endif 
    delay(1000);     
    }
    else
    {
      Serial.printf("OK!\r\n");
    }
  } while (SUCCESS != stat);    
//  } while ((SUCCESS != stat) && (3>iter++));


  if (PCA9548A_70.base_clipped() )
    Serial.printf(" base address out of range, clipped to 0x%u", PCA9548A_70.base_address());

  Serial.print(" Interval is ");
  Serial.print(dtime/1000);
  Serial.print(" sec, ");
 
  Serial.printf("Setup Complete!\r\nSend V/v to toggle verbose");
#if defined I2C_T3_H 
  Serial.printf(", r/R for Wire.resetBus()");
#endif
  Serial.printf("\r\n");
  Serial.printf("In quiet mode, detailed output every 10 minutes\r\n");
  Serial.printf("\n");

  delay(2000);

  startuptime = millis()/1000;

}



/* ========== LOOP ========== */
void loop(void) 
{
//  int16_t temp0;
  uint8_t stat=0;  // status flag
  uint8_t control_read_val = 0;


  new_millis = millis();
  if (new_millis > (old_millis + 100UL))  // 100 msec or more has passed, so check for seconds tick
  {
    old_millis = new_millis;

    new_elapsed_seconds = new_millis/1000UL;
    if (new_elapsed_seconds > total_elapsed_seconds)
    {
      // seconds tick
      total_elapsed_seconds = new_elapsed_seconds;
      old_sec_millis = new_millis;
      seconds_tick = true;
    }
  }

  /**
   * update seconds, 10 seconds, minute ticks
   */
  if (seconds_tick)
  {
      Serial.print(".");

      if (0 == (total_elapsed_seconds % 10))
      {
          seconds_ten_tick = true;
      }

      if (0 == (total_elapsed_seconds % 60))
      {
          minute_tick = true;

          if (0 == (total_elapsed_seconds % 600))
            minute_ten_tick = true;
      }
  }



  if (Serial.available()>0)
  {
    inbyte = Serial.read();
    switch (inbyte)
    {
      case 'v':
      case 'V':
        verbose = !verbose;
        break;

#if defined I2C_T3_H 
      case 'r':
      case 'R':     
        Serial.printf("\nWill call resetBus!\r\n");
        Wire.resetBus();
        break;
#endif

      //ignore others
    }
  }


  digitalWrite(LED_BUILTIN,HIGH); // LED on
  for (uint8_t tui = 0; tui <= 7; tui++)
  {
    if (verbose) Serial.printf("@%.4u ch %u", millis(), tui);

    stat = PCA9548A_70.controlWrite(PCA9548A_70.channel[tui]);
    if (SUCCESS != stat)
    {
      text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
      Serial.printf("control write to ch %u of 0x%.2X failed with return of 0x%.2X: %s\r\n", tui, PCA9548A_70.channel[tui], PCA9548A_70.error.ret_val, text_ptr);
      delay(dtime/2); // don't blast repeat failures too quickly
      break;
    }
    else if (verbose) Serial.print(".");  // period to show progress

    stat = PCA9548A_70.controlRead(&control_read_val);
    if (SUCCESS != stat)
    {
      text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
      Serial.printf("control read failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
      delay(dtime/2); // don't blast repeat failures too quickly
      break;
    }
    else if (control_read_val != PCA9548A_70.channel[tui])
    {
      // only check for correct value if I2C message was OK...
      // ... if here, message was successful but somehow value is wrong
      // since msg was OK error counters in lib didn't increment so do it here
      Serial.printf("Error: control read value=0x%.2X\r\n", control_read_val);  
      PCA9548A_70.error.data_value_error_count++;
      PCA9548A_70.error.total_error_count++; 
      delay(dtime/2); // don't blast repeat failures too quickly
      break;
    }
    else if (verbose) Serial.print(".");  // no errors, period to show progress


    // no error in this loop iter
    if (verbose) Serial.printf("OK ch %u control=0x%.2X\r\n", tui, control_read_val);

  } // end of for loop

  // stat = PCA9548A_70.enableManyTest();
  //   if (SUCCESS != stat)
  //   {
  //     text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
  //     Serial.printf("enableManyTest failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
  //     delay(dtime/2); // don't blast repeat failures too quickly
  //     break;
  //   }  

  digitalWrite(LED_BUILTIN,LOW); // LED off+

  if (PCA9548A_70.error.total_error_count > old_total_error_count)
  {
    old_total_error_count = PCA9548A_70.error.total_error_count;   // update our threshold
    new_errors = true;
  }

  if (seconds_tick || verbose || minute_ten_tick || new_errors)
  {
    seconds_tick = false; // we've used it up

    if (verbose || minute_ten_tick || new_errors)
    {
      minute_ten_tick = false;
      new_errors = false;
      Serial.printf("\r\net:%u  Good:%llu  %llu/sec", total_elapsed_seconds, PCA9548A_70.error.successful_count, PCA9548A_70.error.successful_count/(uint64_t)(total_elapsed_seconds-startuptime));
      if (PCA9548A_70.error.total_error_count) 
        {
          Serial.printf("  bad:%llu", PCA9548A_70.error.total_error_count);
#if defined I2C_AUTO_RETRY
    Serial.printf("  resetBus: %u", Wire.resetBusCountRead());
#endif
        }
      Serial.println(); 
    if (verbose) Serial.println();
    }
    else
    {
      // seconds tick but not verbose or no errors, so minimal output
      if (seconds_ten_tick)
      {
        seconds_ten_tick = false; // we've used it up
        Serial.print("'");
      } 
      if (minute_tick)
      {
        minute_tick = false;  // we've used it up
        Serial.println();     // keep periods from piling up on one line
      }
    }  
  }

  // delay(dtime);
}



