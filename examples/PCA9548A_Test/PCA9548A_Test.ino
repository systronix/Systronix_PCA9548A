
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


//AKW0	KEYWORD0
//AKW1	KEYWORD1
//AKW2	KEYWORD2
//AKW3	KEYWORD3
//AKW4	KEYWORD4
//AKW5	KEYWORD5
//AKW6	KEYWORD6

 /**
 * debug level
 * 0 = quiet, suppress everything
 * 3 = max, even more or less trivial message are emitted
 * 4 = emit debug info which checks very basic data conversion, etc
 */
 byte DEBUG = 3;

uint16_t dtime;  // delay in loop

uint8_t config_value;


Systronix_PCA9548A PCA9548A_70(PCA9548A_SLAVE_ADDR_0);    // We can pass constructor a value

/* ========== SETUP ========== */
void setup(void) 
{


//  uint16_t wrt16=0;  // temp write variable
  int8_t stat = -1;
//  int16_t temp_int16 = 0;
  
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

  if (SUCCESS != stat) Serial.print (" config init error! ");
  stat = tmp275_48.configRead (&raw8);
  if (SUCCESS != stat) Serial.print (" config read error! ");

  Serial.printf(" read CFG: %X\r\n", raw8); 

  if ( tmp275_48.base_clipped() )
    Serial.printf("base address out of range, clipped to 0x%u", tmp275_48.base_address());

  if ( tmp275_48.exists() )
    Serial.printf("Sensor exists\r\n");
  else 
    Serial.printf("Sensor does not exist!\r\n");
  
  delay(250);    // 220 msec for conversion


  dtime = 1000;      // msec between samples, 1000 = 1 sec, 60,000 = 1 minute
  Serial.print(" Interval is ");
  Serial.print(dtime/1000);
  Serial.print(" sec, ");
 
  Serial.println("Setup Complete!");
  Serial.println(" "); 
  
  if (1 == DEBUG)
  {
    Serial.println("sec deg C");
  }
}


uint16_t good=0;
uint16_t bad=0;


/* ========== LOOP ========== */
void loop(void) 
{
//  int16_t temp0;
  int8_t stat=-1;  // status flag
  float temp;
  
  Serial.print("@");
  Serial.print(millis()/1000);
  Serial.print(" ");
  
  if (!fake)  // get real temperature data from sensor
  {
  //  Serial.print("good:");
  //  Serial.print(good);
  //  Serial.print(" ");
    if (bad > 0) 
    {
      Serial.print(" bad:");
      Serial.print(bad);
      Serial.print(" ");
    }
  
    if (DEBUG >=3)
    {
      Serial.print("ALpin:");
      Serial.print(digitalRead(3));
      Serial.print(" ");
    }
  
    stat = tmp275_48.pointerWrite(TMP275_CONF_REG_PTR);
    if (DEBUG >=3)
    {
      stat = tmp275_48.configRead (&raw8);
      Serial.print("CFG:");
      Serial.print(raw8, HEX);
      Serial.print(" ");
    }
  
    // pointer set to read temperature
    stat = tmp275_48.pointerWrite(TMP275_TEMP_REG_PTR); 
    // read two bytes of temperature
    stat = tmp275_48.register16Read (&rawtemp);
//    if (2==stat) good++;
    if (SUCCESS==stat) good++;
    else bad++;
  } // if !fake 
  else rawtemp = faketemp;    // fresh simulated value
  
  if (DEBUG >= 2)
  {
    Serial.print ("Raw16:0x");
    if (0==(rawtemp & 0xF000)) Serial.print("0");
    Serial.print(rawtemp, HEX);
    Serial.print (" ");
  }

  temp = tmp275_48.raw12_to_c(rawtemp);
  
  temperature = temp;  // for Ethernet client
  
  if (DEBUG >= 2)
  {
    Serial.print("ms12:0x");
    Serial.print(rawtemp, HEX);
    Serial.print (" ");
  }
  
  if (fake) Serial.print (temp, 4);  // no rounding of simulated data
  else Serial.print (temp, 4);       // 2 dec pts good enough for real data 0.0625 deg C per count
  
  Serial.print (" C ");
  
  if (DEBUG >= 2)
  {
    Serial.print(rawtemp, DEC);
    Serial.print ("D ");
  }
  
  // test with all values of simulated rawtemp data
  if (fake) 
  {
    // faketemp raw change of 0x280 is 5 deg C
    faketemp -= 0x280;  
    // 0xE480 is min legal value
    if (temp <= -55) 
    {
      faketemp = 0x4B00;  // if min then reset to max    
      
      fake = false;      // switch to real data after full cycle of simulated
      dtime = 2000;
      Serial.println();
      Serial.print ("Changing to real data");
    }
  }
  
  Serial.println();
  
  
  delay(dtime);
}



/**
Read the most current temperature already converted and present in the TMP275 temperature registers

In continuous mode, this could be one sample interval old
In one shot mode this data is from the last-requested one shot conversion
**/
uint8_t readTempDegC (float *tempC) 
{
    return 1;
}



/**
Convert deg C float to a raw 13-bit temp value in TMP275 format.
This is needed for Th and Tl registers as thermostat setpoint values

return 0 if OK, error codes if float is outside range of TMP275
**/
int8_t degCToRaw13 (uint16_t *raw13, float *tempC)
{
    return 1;
}



/**
Trigger a one-shot temperature conversion, wait for the new value, about 26 msec, and update 
the variable passed.

If the TMP275 is in continuous conversion mode, this places the part in One Shot mode, 
triggers the conversion, waits for the result, updates the variable, and leaves the TMP275 in one shot mode.

returns 0 if no error
**/
uint8_t getOneShotDegC (float *tempC)
{
    return 1;
}


/**
Set the TMP275 mode to one-shot, with low power sleep in between

mode: set to One Shot if true. 
If false, sets to continuous sampling mode at whatever sample rate was last set.

returns: 0 if successful
**/
int8_t setModeOneShot (boolean mode)
{
    return 1;
}

/**
Set TMP275 mode to continuous sampling at the rate given.

rate: must be one of the manifest constants such as TMP275_CFG_RATE_1HZ
if rate is not one of the four supported, it is set to the default 4 Hz

returns: 0 if successful
**/
int8_t setModeContinuous (int8_t rate)
{
    return 1;
}





