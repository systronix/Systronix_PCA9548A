
/** ---------- Mux and Temps Test Code ----------------------------------------

  Drive the PCA9548A Mux and read TMP275 Temp Sensors on each Mux channel

  Part of PCA9548A Library Test Code 

  Copyright 2018 Systronix Inc www.systronix.com

----------------------------------------------------------------------------**/
 
/**---------------------------< REVISIONS >------------------------------------

2017Mar13 bboyes  Start

-----------------------------------------------------------------------------*/

/**---------------------------< TODO LIST >------------------------------------

2017Mar06 bboyes  pick a license to use and put it in here. 
My head hurts every time I try to figure out which is "best" for us.

-----------------------------------------------------------------------------*/


#include <Arduino.h>
#include <Systronix_i2c_common.h>
#include <Systronix_PCA9548A.h>	// best version of I2C library is #included by the library. Don't include it here!
#include <Systronix_TMP275.h>   // temp sensor
#include <SALT_power_FRU.h>     // MUX and sensors pass through PowerFRU, if we have one

 /**
 * debug level
 * 0 = quiet, suppress everything
 * 3 = max, even more or less trivial message are emitted
 * 4 = emit debug info which checks very basic data conversion, etc
 */
 byte DEBUG = 3;

uint16_t dtime = 5000;  // delay in loop

uint8_t config_value;

uint8_t config_tmp275;

// If we have PowerFRU, ext net reset must be turned off
// Also MUX and sensors powered by FRU if a SALT new board
SALT_power_FRU fru;

Systronix_i2c_common i2c_common; 

//Systronix_PCA9548A PCA9548A_70(PCA9548A_SLAVE_ADDR_0);
Systronix_PCA9548A PCA9548A_70;

//
//TMP102 on board SALT has same address as TMP275 0 so don't use that
//

// Drawer 1 uses Mux 0
Systronix_TMP275 Mux0Temp0;   // Ambient Temp back of habitat
Systronix_TMP275 Mux0Temp1;   // D1C1, top animal drawer, left compartment
Systronix_TMP275 Mux0Temp2;   // D1C2, center
Systronix_TMP275 Mux0Temp3;   // D1C3

// End Cap 
Systronix_TMP275 Mux4Temp5;
Systronix_TMP275 Mux4Temp6;

// back of D1C3, mostly for convenience of plugging first drawer cable from mux to ext sensor board not inside the drawer
Systronix_TMP275 Mux0Temp7;

// Drawer 2
Systronix_TMP275 Mux2Temp1;
Systronix_TMP275 Mux2Temp2;
Systronix_TMP275 Mux2Temp3;

Systronix_TMP275 *SensorPtr;


const char * text_ptr;

uint32_t startup_seconds = 0;
uint32_t old_seconds, new_seconds;

uint32_t old_total_error_count = 0;

boolean verbose = false;

//---------------------------< S E T U P >---------------------------------------------------------
//
void setup(void) 
{

  Serial.begin(115200);     // use max baud rate
  // Teensy3 doesn't reset with Serial Monitor as do Teensy2/++2, or wait for Serial Monitor window
  // Wait here for 10 seconds to see if we will use Serial Monitor, so output is not lost
  while((!Serial) && (millis()<10000));    // wait until serial monitor is open or timeout, which seems to fall through
 
  Serial.printf("\r\nHabitat Temp Monitor at MUX address 0x%.2X\r\n", PCA9548A_70.base_get());

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



fru.setup (I2C_FRU);                // POWER FRU
fru.begin ();
if (SUCCESS == fru.init ())
{
  Serial.printf("Power FRU init OK/r/n");
}
else
{
  Serial.printf("Power FRU init failed");
}


PCA9548A_70.setup (PCA9548A_SLAVE_ADDR_0, Wire1, (char*)"Wire1");
PCA9548A_70.begin (I2C_PINS_29_30, I2C_RATE_100);
PCA9548A_70.init ();

/**
TMP102 on board SALT has same address as TMP275 0 so don't use that
*/

/**
  MUX channel is not opened to every temp sensor so we cannot init them all since that requires
  actual access of the TMP275.
  Here all we can do is setup and begin which just get I2C ready for that sensor instance.
*/
Mux0Temp0.setup (TMP275_SLAVE_ADDR_0, Wire1, (char*)"Ambient"); // initialize this sensor instance
Mux0Temp0.begin (I2C_PINS_29_30, I2C_RATE_100);

Mux0Temp1.setup (TMP275_SLAVE_ADDR_1, Wire1, (char*)"D1-C1");	// initialize this sensor instance
Mux0Temp1.begin (I2C_PINS_29_30, I2C_RATE_100);


Mux0Temp2.setup (TMP275_SLAVE_ADDR_2, Wire1, (char*)"D1-C2");	// initialize this sensor instance
Mux0Temp2.begin (I2C_PINS_29_30, I2C_RATE_100);


Mux0Temp3.setup (TMP275_SLAVE_ADDR_3, Wire1, (char*)"D1-C3");	// initialize this sensor instance
Mux0Temp3.begin (I2C_PINS_29_30, I2C_RATE_100);

Mux0Temp7.setup (TMP275_SLAVE_ADDR_7, Wire1, (char*)"D1C3 Back"); // initialize this sensor instance
Mux0Temp7.begin (I2C_PINS_29_30, I2C_RATE_100);

Mux4Temp5.setup (TMP275_SLAVE_ADDR_5, Wire1, (char*)"ECTop"); // initialize this sensor instance
Mux4Temp5.begin (I2C_PINS_29_30, I2C_RATE_100);

Mux4Temp6.setup (TMP275_SLAVE_ADDR_6, Wire1, (char*)"ECBot"); // initialize this sensor instance
Mux4Temp6.begin (I2C_PINS_29_30, I2C_RATE_100);


// Mux2Temp1.setup (TMP275_SLAVE_ADDR_1, Wire1, (char*)"D2-C1");	// initialize this sensor instance
// Mux2Temp1.begin (I2C_PINS_29_30, I2C_RATE_100);


// Mux2Temp2.setup (TMP275_SLAVE_ADDR_2, Wire1, (char*)"D2-C2");	// initialize this sensor instance
// Mux2Temp2.begin (I2C_PINS_29_30, I2C_RATE_100);


// Mux2Temp3.setup (TMP275_SLAVE_ADDR_3, Wire1, (char*)"D2-C3"); // initialize this sensor instance
// Mux2Temp3.begin (I2C_PINS_29_30, I2C_RATE_100);


  uint8_t iter = 0;

  uint8_t stat = 0;

  pinMode(LED_BUILTIN,OUTPUT);    // LED
  
  config_value = PCA9548A_PORT_0_ENABLE;  // 

  // initialize MUX Channel 0, don't proceed unless succeeds
  do
  {
    iter++;
    stat = PCA9548A_70.init(config_value);
    Serial.printf(" Attempt #%u: Init control reg to 0x%.2X - ", iter, config_value); 
    if (SUCCESS != stat)
    {
//      text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.error_val]);
      text_ptr = "where are i2c error strings?";
      Serial.printf("failed - returned 0x%.2X: %s\r\n", PCA9548A_70.error.error_val, text_ptr);
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


  if ( PCA9548A_70.base_clipped() )
    Serial.printf(" base address out of range, clipped to 0x%u", PCA9548A_70.base_get());

  Serial.print(" Interval is ");
  Serial.print(dtime/1000);
  Serial.print(" sec, ");
 
  Serial.printf("Setup Complete!\r\nSend Q/q for quiet, V/v for verbose");
#if defined I2C_T3_H 
  Serial.printf(", r/R for Wire.resetBus()");
#endif
  Serial.printf("\r\n\n");

  delay(2000);

  startup_seconds = millis()/1000;

}



/** ------------------------< sensor_read >-------------------------------------------------------

Read the sensor and fill the data struct

@param tmp275_ptr - pointer to the TMP275 instance whose data we want to sample
*/
uint8_t sensor_read (Systronix_TMP275 *tmp275_ptr)
  {
  uint8_t stat=0;

  stat = tmp275_ptr->init(TMP275_CFG_RES12);
  if (stat != SUCCESS) 
    {
       Serial.printf("TMP275 init %s error, stat=%u\r\n", tmp275_ptr->wire_name, stat);
       return FAIL;
     }

  stat = tmp275_ptr->get_temperature_data();
  if (stat != SUCCESS) 
    {
       Serial.printf("TMP275 read %s error, stat=%u\r\n", tmp275_ptr->wire_name, stat);
       return FAIL;
     }
  else 
    {
      Serial.printf ("%s %3.1f\r\n", tmp275_ptr->wire_name, tmp275_ptr->data.deg_f); 
      return SUCCESS;
    }
  }





//---------------------------< L O O P >-----------------------------------------------------------
//
void loop(void) 
{
//  int16_t temp0;
  uint8_t stat=0;  // status flag
  uint8_t control_read_val = 0;
  // uint16_t rawtemp;
  // float temp;

  uint8_t inbyte = 0;

  // remember for tests at end of this loop
  old_seconds = millis()/1000;
  old_total_error_count = PCA9548A_70.error.total_error_count;

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

      // TODO allow entering m Some message
      // which will then log that message. 
      // Useful for adding comments about the test in progress
      case 'm':
      case 'M':
        break;

#if defined I2C_T3_H 
      case 'r':
      case 'R':     
        Serial.printf("\nWill call resetBus!\r\n");
        Wire.resetBus();
        break;
#endif
        // ignore any others
      }
    }


  digitalWrite(LED_BUILTIN,HIGH); // LED on

  Serial.printf("@%.4u\r\n", millis()/1000); 
  
  // Enable Mux Channel 0
  stat = PCA9548A_70.control_write(PCA9548A_PORT_0_ENABLE);
  if (SUCCESS != stat)
  {
    // text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.error_val]);
    text_ptr = "where are i2c error strings?";
    Serial.printf("mux0 control write failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.error_val, text_ptr);
    delay(dtime/2); // don't blast repeat failures too quickly
  }
  else
  {
    stat = PCA9548A_70.control_read(&control_read_val);
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);
  }

  SensorPtr = &Mux0Temp0;   
  stat = sensor_read (SensorPtr);

  SensorPtr = &Mux0Temp1;   
  stat = sensor_read (SensorPtr);




  // ------------------------------
  SensorPtr = &Mux0Temp2;   
  stat = sensor_read (SensorPtr);

  SensorPtr = &Mux0Temp3;
  stat = sensor_read (SensorPtr);

  SensorPtr = &Mux0Temp7;
  stat = sensor_read (SensorPtr);  

  // Enable Mux Channel 4
  stat = PCA9548A_70.control_write(PCA9548A_PORT_4_ENABLE);
  if (SUCCESS != stat)
  {
    //text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.error_val]);
    text_ptr = "where are i2c error strings?";
    Serial.printf("mux1 control write failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.error_val, text_ptr);
    delay(dtime/2); // don't blast repeat failures too quickly
  }
  else
  {
    stat = PCA9548A_70.control_read(&control_read_val);
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);
  }  


  SensorPtr = &Mux4Temp5;
  stat = sensor_read (SensorPtr);

  SensorPtr = &Mux4Temp6;
  stat = sensor_read (SensorPtr);


  Serial.println();

  digitalWrite(LED_BUILTIN,LOW); // LED off+

  new_seconds = millis()/1000;

  if ( (verbose) || (new_seconds > old_seconds) )
  {
    Serial.printf("et:%u  MUX Good:%u  %u/sec", millis()/1000, PCA9548A_70.error.successful_count, PCA9548A_70.error.successful_count/(new_seconds-startup_seconds));
    if (PCA9548A_70.error.total_error_count) 
      {
        Serial.printf("  bad:%u  ", PCA9548A_70.error.total_error_count);
#if defined I2C_AUTO_RETRY
        if (PCA9548A_70.error.total_error_count > old_total_error_count)
        {
          Serial.printf( "- new errors! - busReset: %u", Wire.resetBusCountRead() );
        }
#endif
      }
    Serial.println(); 
    if (verbose) Serial.println();
  }

  delay(dtime);
}





