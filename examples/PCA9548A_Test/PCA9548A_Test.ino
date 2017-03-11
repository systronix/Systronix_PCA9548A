
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

uint16_t dtime;  // delay in loop

uint8_t config_value;


Systronix_PCA9548A PCA9548A_70(PCA9548A_SLAVE_ADDR_0);    

/* ========== SETUP ========== */
void setup(void) 
{


  uint8_t stat = 0;

  
  Serial.begin(115200);     // use max baud rate
  // Teensy3 doesn't reset with Serial Monitor as do Teensy2/++2, or wait for Serial Monitor window
  // Wait here for 10 seconds to see if we will use Serial Monitor, so output is not lost
  while((!Serial) && (millis()<10000));    // wait until serial monitor is open or timeout, which seems to fall through
 
  Serial.print("PCA9548A Library Test Code at 0x");
  Serial.println(PCA9548A_70.base_address(), HEX); 
   
  
  // start PCA9548A library
  PCA9548A_70.begin();

  config_value = PCA9548A_CHAN_0_ENABLE;  // 

  // initialize 
	stat = PCA9548A_70.init(config_value);
  Serial.printf(" write CFG: %X\r\n", config_value); 

  if (SUCCESS != stat) 
    Serial.print (" config init error! ");

  if (PCA9548A_70.base_clipped() )
    Serial.printf("base address out of range, clipped to 0x%u", PCA9548A_70.base_address());

  if (PCA9548A_70.exists() )
    Serial.printf("Slave exists\r\n");
  else 
    Serial.printf("Slave does not exist!\r\n");
  
  dtime = 2000;      // msec 1000 = 1 sec, 60,000 = 1 minute
  Serial.print(" Interval is ");
  Serial.print(dtime/1000);
  Serial.print(" sec, ");
 
  Serial.println("Setup Complete!");
  Serial.println(" "); 
  }


/* ========== LOOP ========== */
void loop(void) 
{
//  int16_t temp0;
  uint8_t stat=0;  // status flag

  Serial.print("@");
  Serial.print(millis()/1000);
  Serial.print(" ");
  
  

  Serial.println();
  
  
  delay(dtime);
}



