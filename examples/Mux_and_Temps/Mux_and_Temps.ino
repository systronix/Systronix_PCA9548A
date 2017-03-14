
/** ---------- Mux and Temps Test Code ----------------------------------------

  Drive the PCA9548A Mux and read TMP275 Temp Sensors on each Mux channel

  Part of PCA9548A Library Test Code 

  Copyright 2017 Systronix Inc www.systronix.com

----------------------------------------------------------------------------**/
 
/**---------------------------< REVISIONS >------------------------------------

2017Mar13 bboyes  Start

-----------------------------------------------------------------------------*/

/**---------------------------< TODO LIST >------------------------------------

2017Mar06 bboyes  pick a license to use and put it in here. 
My head hurts every time I try to figure out which is "best" for us.

-----------------------------------------------------------------------------*/


#include <Arduino.h>
#include <Systronix_PCA9548A.h>	// best version of I2C library is #included by the library. Don't include it here!
#include <Systronix_TMP275.h>   // temp sensor


 /**
 * debug level
 * 0 = quiet, suppress everything
 * 3 = max, even more or less trivial message are emitted
 * 4 = emit debug info which checks very basic data conversion, etc
 */
 byte DEBUG = 3;

uint16_t dtime = 1000;  // delay in loop

uint8_t config_value;

uint8_t config_tmp275;


Systronix_PCA9548A PCA9548A_70(PCA9548A_SLAVE_ADDR_0);    

Systronix_TMP275 Mux0Temp1(TMP275_SLAVE_ADDR_1);
Systronix_TMP275 Mux1Temp1(TMP275_SLAVE_ADDR_1);
Systronix_TMP275 Mux2Temp1(TMP275_SLAVE_ADDR_1);

const char * text_ptr;

uint32_t startuptime = 0;

/* ========== SETUP ========== */
void setup(void) 
{


  uint8_t stat = 0;

  pinMode(LED_BUILTIN,OUTPUT);    // LED

  
  Serial.begin(115200);     // use max baud rate
  // Teensy3 doesn't reset with Serial Monitor as do Teensy2/++2, or wait for Serial Monitor window
  // Wait here for 10 seconds to see if we will use Serial Monitor, so output is not lost
  while((!Serial) && (millis()<10000));    // wait until serial monitor is open or timeout, which seems to fall through
 
  Serial.printf("\r\nPCA9548A Library Test Code at 0x%.2X\r\n", PCA9548A_70.base_address());
   
  
  // start PCA9548A library
  PCA9548A_70.begin();

  config_value = PCA9548A_CHAN_0_ENABLE;  // 

  // initialize MUX
	stat = PCA9548A_70.init(config_value);
  Serial.printf(" Init control reg to 0x%.2X\r\n", config_value); 
  if (SUCCESS != stat)
  {
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    Serial.printf(" Init failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
  }
  if ( PCA9548A_70.base_clipped() )
    Serial.printf(" base address out of range, clipped to 0x%u", PCA9548A_70.base_address());

  Serial.print(" Interval is ");
  Serial.print(dtime/1000);
  Serial.print(" sec, ");
 
  Serial.printf("Setup Complete!\r\nSend Q/q for quiet, V/v for verbose output.\r\n\n");

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
  uint16_t rawtemp;
  float temp;


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

      //ignore others
    }
  }


  digitalWrite(LED_BUILTIN,HIGH); // LED on

  if (verbose) Serial.printf("@%.4u  ", millis());
  
  // Enable Mux Channel 0
  stat = PCA9548A_70.controlWrite(PCA9548A_CHAN_0_ENABLE);
  if (SUCCESS != stat)
  {
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    Serial.printf("mux0 control write failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    delay(dtime/2); // don't blast repeat failures too quickly
  }
  else
  {
    stat = PCA9548A_70.controlRead(&control_read_val);
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);
  }

  // Init Mux0Temp1 Sensors
  stat = Mux0Temp1.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp1 init failed with return of 0x%.2X: %s\r\n", Mux0Temp1.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp1.pointerWrite(TMP275_TEMP_REG_PTR);

  // Read Temp on sensors Mux Channel 0
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp1.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp1 error, stat=%u\r\n", stat);
  temp = Mux0Temp1.raw12_to_c(rawtemp);
  Serial.printf ("Mux0Temp1 %6.4f C\r\n", temp);

  // Enable Mux Channel 1
  stat = PCA9548A_70.controlWrite(PCA9548A_CHAN_1_ENABLE);
  if (SUCCESS != stat)
  {
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    Serial.printf("mux1 control write failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    delay(dtime/2); // don't blast repeat failures too quickly
  }
  else
  {
    stat = PCA9548A_70.controlRead(&control_read_val);
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);
  }  

  // Init Mux1Temp1 Sensors
  stat = Mux1Temp1.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux1Temp1 init failed with return of 0x%.2X: %s\r\n", Mux0Temp1.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux1Temp1.pointerWrite(TMP275_TEMP_REG_PTR);

  // Read Temp on sensors Mux Channel 1
  rawtemp=0;
  temp=0.0;
  stat = Mux1Temp1.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux1Temp1 error, stat=%u\r\n", stat);
  temp = Mux1Temp1.raw12_to_c(rawtemp);
  Serial.printf ("Mux1Temp1 %6.4f C\r\n", temp);

  // Enable Mux Channel 2
  stat = PCA9548A_70.controlWrite(PCA9548A_CHAN_2_ENABLE);
  if (SUCCESS != stat)
  {
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    Serial.printf("mux2 control write failed with return of 0x%.2X: %s\r\n", PCA9548A_70.error.ret_val, text_ptr);
    text_ptr = (PCA9548A_70.status_text[PCA9548A_70.error.ret_val]);
    delay(dtime/2); // don't blast repeat failures too quickly
  }
  else
  {
    stat = PCA9548A_70.controlRead(&control_read_val);
    if (verbose) Serial.printf("OK control=0x%.2X\r\n", control_read_val);
  }  

  // Init Mux2Temp1 Sensors
  stat = Mux2Temp1.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux2Temp1 init failed with return of 0x%.2X: %s\r\n", Mux0Temp1.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux2Temp1.pointerWrite(TMP275_TEMP_REG_PTR);

  // Read Temp on sensors Mux Channel 1
  rawtemp=0;
  temp=0.0;
  stat = Mux2Temp1.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux2Temp1 error, stat=%u\r\n", stat);
  temp = Mux2Temp1.raw12_to_c(rawtemp);
  Serial.printf ("Mux2Temp1 %6.4f C\r\n", temp);  


  Serial.println();

  digitalWrite(LED_BUILTIN,LOW); // LED off+

  newtime = millis()/1000;

  if ( (verbose) || (newtime > oldtime) )
  {
    Serial.printf("et:%u  MUX Good:%u  %u/sec", millis()/1000, PCA9548A_70.error.successful_count, PCA9548A_70.error.successful_count/(newtime-startuptime));
    if (PCA9548A_70.error.total_error_count) 
      {
        Serial.printf("  bad:%u", PCA9548A_70.error.total_error_count);
#if defined I2C_AUTO_RETRY
        Serial.printf("  busReset: %u", Wire.resetBusCountRead());
#endif
      }
    Serial.println(); 
    if (verbose) Serial.println();
  }

  delay(dtime);
}



