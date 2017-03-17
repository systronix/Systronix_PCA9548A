
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
 
  Serial.printf("Setup Complete!\r\nSend Q/q for quiet, V/v for verbose");
#if defined I2C_T3_H 
  Serial.printf(", r/R for Wire.resetBus()");
#endif
  Serial.printf("\r\n\n");

  delay(2000);

  startuptime = millis()/1000;

}

boolean verbose = false;

uint32_t oldtime, newtime;
uint8_t inbyte = 0;

/* ========== LOOP ========== */
void loop(void) 
{
//  int16_t temp0;
  uint8_t stat=0;  // status flag
  uint8_t control_read_val = 0;



  oldtime = millis()/1000;

  if (Serial.available()>0)
  {
    inbyte = Serial.read();
    switch (inbyte)
    {
      case 'q':
      case 'Q':
        verbose = false;
        break;

      case 'v':
      case 'V':
        verbose = true;
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
      Serial.printf("control write of %u failed with return of 0x%.2X: %s\r\n", tui, PCA9548A_70.error.ret_val, text_ptr);
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
    else if (verbose) Serial.print(".");  // period to show progress

    if (control_read_val != PCA9548A_70.channel[tui])
    {
      Serial.printf("Error: control read value=0x%.2X\r\n", control_read_val);
      delay(dtime/2); // don't blast repeat failures too quickly
      break;
    }
    else if (verbose) Serial.print(".");  // period to show progress


    // no error in this loop iter
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);

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

  newtime = millis()/1000;

  if ( (verbose) || (newtime > oldtime) )
  {
    Serial.printf("et:%u  Good:%u  %u/sec", millis()/1000, PCA9548A_70.error.successful_count, PCA9548A_70.error.successful_count/(newtime-startuptime));
    if (PCA9548A_70.error.total_error_count) 
      {
        Serial.printf("  bad:%u", PCA9548A_70.error.total_error_count);
#if defined I2C_AUTO_RETRY
        Serial.printf("  resetBus: %u", Wire.resetBusCountRead());
#endif
      }
    Serial.println(); 
    if (verbose) Serial.println();
  }

  // delay(dtime);
}



