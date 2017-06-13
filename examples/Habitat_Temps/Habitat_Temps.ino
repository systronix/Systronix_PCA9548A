
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

/**
TMP102 on board SALT has same address as TMP275 0 so don't use that
*/

Systronix_TMP275 Mux0Temp1(TMP275_SLAVE_ADDR_1);  // D1 ambient temp
Systronix_TMP275 Mux0Temp2(TMP275_SLAVE_ADDR_2);  // D1 C1 outside
Systronix_TMP275 Mux0Temp3(TMP275_SLAVE_ADDR_3);  // D1 C1 inside
Systronix_TMP275 Mux0Temp4(TMP275_SLAVE_ADDR_4);
Systronix_TMP275 Mux0Temp5(TMP275_SLAVE_ADDR_5);
Systronix_TMP275 Mux0Temp6(TMP275_SLAVE_ADDR_6);
Systronix_TMP275 Mux0Temp7(TMP275_SLAVE_ADDR_7);

Systronix_TMP275 Mux1Temp1(TMP275_SLAVE_ADDR_1);
Systronix_TMP275 Mux2Temp1(TMP275_SLAVE_ADDR_1);

const char * text_ptr;

uint32_t startup_seconds = 0;
uint32_t old_seconds, new_seconds;

uint32_t old_total_error_count = 0;

boolean verbose = false;

/* ========== SETUP ========== */
void setup(void) 
{

  uint8_t iter = 0;

  uint8_t stat = 0;

  pinMode(LED_BUILTIN,OUTPUT);    // LED

  
  Serial.begin(115200);     // use max baud rate
  // Teensy3 doesn't reset with Serial Monitor as do Teensy2/++2, or wait for Serial Monitor window
  // Wait here for 10 seconds to see if we will use Serial Monitor, so output is not lost
  while((!Serial) && (millis()<10000));    // wait until serial monitor is open or timeout, which seems to fall through
 
  Serial.printf("\r\nPCA9548A and TMP275 Test Code at 0x%.2X\r\n", PCA9548A_70.base_get());

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

  // initialize MUX Channel 0, don't proceed unless succeeds
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




uint8_t inbyte = 0;


/* ========== LOOP ========== */
void loop(void) 
{
//  int16_t temp0;
  uint8_t stat=0;  // status flag
  uint8_t control_read_val = 0;
  uint16_t rawtemp;
  float temp;

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

  // ------------------------------
  // Init Mux0Temp7 Sensors
  stat = Mux0Temp7.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp7 init failed with return of 0x%.2X: %s\r\n", Mux0Temp7.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp7.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp7 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp7.error.ret_val);    

  // Read Temp on Mux0Temp7
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp7.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp7 error, stat=%u\r\n", stat);
  temp = Mux0Temp7.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp7 %6.4f F\r\n", temp);

    // ------------------------------
  // Init Mux0Temp6 Sensors
  stat = Mux0Temp6.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp6 init failed with return of 0x%.2X: %s\r\n", Mux0Temp6.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp6.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp6 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp6.error.ret_val);  

  // Read Temp on Mux0Temp6
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp6.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp6 error, stat=%u\r\n", stat);
  temp = Mux0Temp6.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp6 %6.4f F\r\n", temp);  

  // ------------------------------
  // Init Mux0Temp5 Sensors
  stat = Mux0Temp5.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp5 init failed with return of 0x%.2X: %s\r\n", Mux0Temp5.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp5.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp5 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp5.error.ret_val);  

  // Read Temp on Mux0Temp5
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp5.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp5 error, stat=%u\r\n", stat);
  temp = Mux0Temp5.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp5 %6.4f F\r\n", temp);  

  // ------------------------------
  // Init Mux0Temp4 Sensors
  stat = Mux0Temp4.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp4 init failed with return of 0x%.2X: %s\r\n", Mux0Temp4.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp4.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp4 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp4.error.ret_val);  

  // Read Temp on Mux0Temp4
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp4.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp4 error, stat=%u\r\n", stat);
  temp = Mux0Temp4.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp4 %6.4f F\r\n", temp);    


  // ------------------------------
  // Init Mux0Temp3 Sensors
  stat = Mux0Temp3.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp3 init failed with return of 0x%.2X: %s\r\n", Mux0Temp3.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp3.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp3 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp3.error.ret_val);  

  // Read Temp on Mux0Temp3
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp3.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp3 error, stat=%u\r\n", stat);
  temp = Mux0Temp3.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp3 %6.4f F\r\n", temp);  


  // ------------------------------
  // Init Mux0Temp2 Sensors
  stat = Mux0Temp2.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp2 init failed with return of 0x%.2X: %s\r\n", Mux0Temp2.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp2.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp2 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp2.error.ret_val);

  // Read Temp on Mux0Temp2
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp2.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp2 error, stat=%u\r\n", stat);
  temp = Mux0Temp2.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp2 %6.4f F\r\n", temp);

  // ------------------------------
  // Init Mux0Temp1 Sensors
  stat = Mux0Temp1.init(TMP275_CFG_RES12);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp1 init failed with return of 0x%.2X: %s\r\n", Mux0Temp1.error.ret_val);

  // leave the pointer set to read temperature
  stat = Mux0Temp1.pointerWrite(TMP275_TEMP_REG_PTR);
  if (SUCCESS != stat) Serial.printf(" Mux0Temp1 ptr write failed with return of 0x%.2X: %s\r\n", Mux0Temp1.error.ret_val);

  // Read Temp on Mux0Temp1
  rawtemp=0;
  temp=0.0;
  stat = Mux0Temp1.register16Read (&rawtemp);
  if (stat != SUCCESS) Serial.printf("Mux0Temp1 error, stat=%u\r\n", stat);
  temp = Mux0Temp1.raw12_to_f(rawtemp);
  Serial.printf ("Mux0Temp1 %6.4f F\r\n", temp);  



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



