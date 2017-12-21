//
// MUX_P7_sensors_read.ino
//
// This code is hacked out of MUX_mfg_tex (most of that is gone).  It repeated reads the mux-mounted TMP275 and
// HDC1080 sensors and displays the returned values on the debug LCD display
//
// TO RUN THIS TEST:
//	required hardware:
//		1. A SALT board with power supply
//		2. RJ25 cable between SALT J1 and power dist FROM SALT
//		3. RJ25 cable between power dist EXT NET and UUT BUS I/O
//
//	power on, code runs ad nauseum.
//

//---------------------------< I N C L U D E S >--------------------------------------------------------------


#include	<SALT_power_FRU.h>
#include	<SALT_FETs.h>
#include	<SALT_utilities.h>

#include	<Systronix_HDC1080.h>
#include	<Systronix_ili9341_helper.h>
#include	<Systronix_M24C32.h>	// fram
#include	<Systronix_PCA9548A.h>
#include	<Systronix_TMP275.h>

#include	<SdFat.h>
#include	<XPT2046_Touchscreen.h>

//---------------------------< D E C L A R A T I O N S >------------------------------------------------------

SALT_FETs FETs;
SALT_power_FRU fru;
SALT_utilities utils;

Systronix_HDC1080 hdc1080;
Systronix_ili9341_helper display;
Systronix_M24C32 eep;
Systronix_PCA9548A mux;
Systronix_PCA9548A cascade_mux;
Systronix_TMP275 tmp275;
Systronix_TMP275 local_tmp275;

XPT2046_Touchscreen ts (T_CS_PIN);
extern ILI9341_t3 tft;

mbox_com	mailbox_common = {0, {__DATE__}, {__TIME__}};		// common information not specific to a habitat; defined in SALT_reptile.h

elapsedMillis waiting;


//---------------------------< D E F I N E S >----------------------------------------------------------------

#define PCB_MAJOR_REV	2
#define PCB_MINOR_REV	00


//---------------------------< P R O T O T Y P E S >----------------------------------------------------------


//-----------------------------< P A G E   S C O P E   V A R I A B L E S >------------------------------------

char		temp_display_buf[LINE_LEN];

boolean		continuous_mode = false;



//---------------------------< F A I L _ L E D >--------------------------------------------------------------
//
// This function never exits.  Teensy LED may blink a pattern to indicate failure of the debug display.
//

void fail_led (uint8_t count=0)
	{
	pinMode(SCK_PIN, OUTPUT);											// set the pin to be an output

	Serial.printf ("test terminated with FAIL; reset to restart");

	if (count)
		{
		while (1)														// repeat forever
			{
			for (uint8_t i=0; i<count; i++)									// blink count number of blinks
				{
				digitalWrite (SCK_PIN, HIGH);							// Teensy LED on
				delay(50);

				digitalWrite (SCK_PIN, LOW);							// Teensy LED off
				delay(200);
				}
			delay (1000);												// pause and do it all again
			}
		}
	else
		{
		while (1);
		}
	}


//---------------------------< S T A R T U P _ C H I R P >----------------------------------------------------
//
// a function that provides audible progress and status information.
//

void startup_chirp (uint8_t count)
	{
	for (uint8_t i=0; i<count;)
		{
		FETs.FET_settings &= ~BUZZER;		// ON
		FETs.update_manual();
		FETs.FET_settings |= BUZZER;		// OFF
		FETs.update_manual();
		i++;
		if (i < count)
			delay(50);										// no need to do this delay after we've chirped the last chirp
		}
	}


//---------------------------< P I N G E X >------------------------------------------------------------------
//
//
//

uint8_t pingex (uint8_t addr, i2c_t3 wire)
	{
	wire.beginTransmission (addr);				// set the device slave address
	return wire.endTransmission();				// send slave address; returns SUCCESS if the address was acked
	}


//---------------------------< T M P 2 7 5 _ T E S T >--------------------------------------------------------
//
// This test writes a value that has ones in it to the 275's config register; this to prove that there are
// pullups on the 9548A outputs though that may not be detectable because of the pullups on the 9548A common
// side.  Regardless, reads temperature data from 275 which should be valid.
//

uint8_t tmp275_test (Systronix_TMP275* device)
	{
	uint8_t ret_val;

	if (SUCCESS != device->init (0x03))			// write 0x03 to the config register (anything with ones)
		return FAIL;							// could not init tmp275

	if (SUCCESS != device->config_read (&ret_val))
		return FAIL;							// could not read tmp275

	if (0x03 != ret_val)
		return FAIL;							// returned value is wrong

	if (SUCCESS != device->init (0x00))			// re-init to the point the config register at temp register
		return FAIL;							// could not init tmp275

	if (SUCCESS != device->get_temperature_data())
		return FAIL;							// could not get temp data

	return SUCCESS;
	}


//---------------------------< S E T U P >--------------------------------------------------------------------

void setup(void)
	{
	uint8_t ret_val;

	waiting = 0;
	Serial.begin(115200);								// use max baud rate

	Serial1.begin(9600);								// UI habitat A LCD and keypad

	Serial2.begin(9600);								// UI habitat B LCD and keypad

	Serial2.setRX (26);									// set alternate pins: SALT uses primary RX2 pin ...
	Serial2.setTX (31);									// ... for ETH_RST(L) and primary TX2 pin for ETH_CS(L)

	Serial1.printf ("r\r");								// 'r' initialize display so we can have a sign-on message
	Serial2.printf ("r\r");
	delay(50);

	//                0123456789ABCDEF0123456789ABCDEF
	Serial1.printf ("dMUX mfg test    \r");				// in case this code ever ends up in a habitat
	Serial2.printf ("dMUX mfg test    \r");

	pinMode(PERIPH_RST, OUTPUT);						// NOTE: SALT 2.0 resetting the FETs 9557 (U5) allows the gates of the FETs to float; U5 should be immediately initialized following reset
	pinMode (ETHER_RST, OUTPUT);
	pinMode (uSD_DETECT, INPUT_PULLUP);					// so we know if a uSD is in the socket

//---------- arduino libraries setup
	utils.fw_reset (RST_E | RST_P);						// assert resets

	utils.spi_port_pins_test ();						// test mobility of spi port pins; leave CS pins as INPUT_PULLUP; results in utils.spi_port_pins_result

	utils.fw_reset (0);									// release resets

	if (utils.spi_port_pins_result)						// if there is a SPI pin stuck high or low can we still use the debug display?
		{
		if ((TEST_MISO | TEST_MOSI | TEST_SCK) & utils.spi_port_pins_result)
			{											// no SPI - one or more of MISO, MOSI, SCK stuck high or low
			Serial.printf ("one or more of MISO, MOSI, SCK stuck high or low\n");
			fail_led(1);								// go blink and never return
			}
		else if ((uint8_t)(utils.spi_port_pins_result >> 8) & ((uint8_t)(~utils.spi_port_pins_result) & 0xF8))
			{											// one or more spi chip selects stuck low
			Serial.printf ("one or more of spi CS stuck low\n");
			fail_led(2);								// go blink and never return
			}
		else
			{											// one or more spi chip selects stuck high
			if (TEST_DISP_CS & utils.spi_port_pins_result)	// is it display CS?
				{
				Serial.printf ("display CS stuck high\n");
				fail_led(3);							// can't use debug display; go blink and never return
				}
			}
		}

	display.setup ();

	const uint16_t splash_list[] = {ILI9341_BLUE, ILI9341_GREEN, ILI9341_RED, ILI9341_BLACK};
	ret_val = display.screen_splash (splash_list);
	if (SUCCESS != ret_val)
		fail_led(4);

	ts.begin();											// touch screen

	while((!Serial) && (waiting<2000));					// wait until serial port is open or timeout

	snprintf (temp_display_buf, LINE_LEN, "MUX port 7 sensor read test");
	display.display_line_write (temp_display_buf, ILI9341_CYAN);
	Serial.printf ("%s\n", temp_display_buf);

	snprintf (temp_display_buf, LINE_LEN, "build: %s %s", mailbox_common.build_time, mailbox_common.build_date);
	display.display_line_write (temp_display_buf, ILI9341_CYAN);
	Serial.printf ("%s\n", temp_display_buf);

	snprintf (temp_display_buf, LINE_LEN, "for MUX PCB rev %d.%d", PCB_MAJOR_REV, PCB_MINOR_REV);
	display.display_line_write (temp_display_buf, ILI9341_CYAN);
	Serial.printf ("%s\n", temp_display_buf);
	delay (1000);

	Serial.flush();

	if (utils.spi_port_pins_result)						// if there was a SPI CS stuck high but not the display CS, display the failure
		{
		display.display_line_write ("SPI mobility failed", ILI9341_RED);
		if (TEST_uSD_CS & utils.spi_port_pins_result)
			{
			strcpy (temp_display_buf, (char*)"uSD_CS(L) stuck high");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		if (TEST_ETH_CS & utils.spi_port_pins_result)
			{
			strcpy (temp_display_buf, (char*)"ETH_CS(L) stuck high");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		if (TEST_T_CS & utils.spi_port_pins_result)
			{
			strcpy (temp_display_buf, (char*)"T_CS(L) stuck high");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		if (TEST_FLASH_CS & utils.spi_port_pins_result)
			{
			strcpy (temp_display_buf, (char*)"FLASH_CS(L) stuck high");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		fail_led();
		while(1);
		}

//============================================================================================================

	ret_val = 0;										// presume not touched
	for (uint8_t i=0; i<10; i++)
		{
		if (ts.touched())
			ret_val = 1;

		delay (10);
		}

	if (ret_val)
		{
		display.display_line_write ("unexpected touch", ILI9341_RED);
		fail_led();
		while(1);
		}


//============================================================================================================

	ret_val = utils.i2c_port_pins_test (0);				// test mobility of i2c port 0 pins
	if (ret_val)
		{
		display.display_line_write ("I2C net 0 mobility failed", ILI9341_RED);
		if (ret_val & TEST_SCL)
			{
			snprintf (temp_display_buf, LINE_LEN, "SCL0 stuck %s", (ret_val & TEST_SCL_HL) ? (char*)"high" : (char*)"low");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		if (ret_val & TEST_SDA)
			{
			snprintf (temp_display_buf, LINE_LEN, "SDA0 stuck %s", (ret_val & TEST_SDA_HL) ? (char*)"high" : (char*)"low");
			display.display_line_write (temp_display_buf, ILI9341_CYAN);
			}
		fail_led();
		while (1);
		}


//============================================================================================================

	FETs.setup (I2C_FET);								// constructor for SALT_FETs and PCA9557
	FETs.begin();										// joins i2c bus as master
	FETs.init();										// inits 9557

	ret_val = FETs.pin_mobility_test();					// nothing stuck high; anything stuck low?

	if (SUCCESS != ret_val)								// make sure we can wiggle all FETs register pins; causes a chirp so done last before startup chirp
		{
		display.display_line_write ("U5 mobility failed", ILI9341_RED);
		snprintf (temp_display_buf, LINE_LEN, "wrote: 0x%.2x; read: 0x%.2X", (uint8_t)(FETs.mobility_test_vals_get() >> 8), (uint8_t)FETs.mobility_test_vals_get());
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		fail_led();
		while (1);
		}


//============================================================================================================
//
// NOTE: Cannot detect a stuck-high line on UUT or second MUX because the stuck wire is on the far side of a
// 9600; the 9600 does not 'drive' the Teensy-side sda/scl line high but allows the, to be pulled up by local
// pullups.  The 9600 does drive low so a UUT or second mux i2c signal stuck low can be detected.
//

 	ret_val = utils.i2c_port_pins_test (1);				// test mobility of i2c port 1 pins
	if (ret_val)
		{
		display.display_line_write ("I2C net 1 mobility failed", ILI9341_RED);
		if (ret_val & TEST_SCL)
			{
			snprintf (temp_display_buf, LINE_LEN, "SCL1 stuck %s", (ret_val & TEST_SCL_HL) ? (char*)"high" : (char*)"low");
			display.display_line_write (temp_display_buf, ILI9341_RED);
			}
		if (ret_val & TEST_SDA)
			{
			snprintf (temp_display_buf, LINE_LEN, "SDA1 stuck %s", (ret_val & TEST_SDA_HL) ? (char*)"high" : (char*)"low");
			display.display_line_write (temp_display_buf, ILI9341_RED);
			}
		while(1);
		}


//============================================================================================================

	fru.setup (I2C_FRU);								// POWER FRU
	fru.begin ();
	ret_val = fru.init ();
	if (SUCCESS == ret_val)
		{
		if (SUCCESS != fru.pin_mobility_test (0))
			{
			display.display_line_write ("FRU mobility failed", ILI9341_RED);
			fail_led();
			while (1);
			}
		display.display_line_write ("FRU mobility OK", ILI9341_GREEN);

		fru.init ();									// restore after test
		}
	else
		{
		display.display_line_write ("FRU init failed", ILI9341_RED);
		Serial.printf ("fru ini fail: 0x%.2X\n", ret_val);
		fail_led();
		while (1);
		}

	fru.sensor_reset (true);							// assert reset to the mfg test hardware
	fru.update_manual();
	delay (50);
	fru.sensor_reset (false);							// and release it
	fru.update_manual();

	display.display_line_write ("FRU init OK", ILI9341_GREEN);


//============================================================================================================
// setup port7 devices

	mux.setup (PCA9548A_BASE_MIN, Wire1, (char*)"Wire1");		// initialize the UUT mux
	mux.begin (I2C_PINS_29_30, I2C_RATE_100);
	mux.init ();
	mux.control_write (PCA9548A_PORT_0_ENABLE);					// write something with a 1 in it to ext mux

	mux.control_read (&ret_val);								// read it back

	if (PCA9548A_PORT_0_ENABLE != ret_val)
		{
		display.display_line_write ("UUT MUX fail FAIL", ILI9341_RED);
		while(1);
		}

	mux.control_write (PCA9548A_PORT_7_ENABLE);
	local_tmp275.setup (TMP275_BASE_MAX, Wire1, (char*)"Wire1");		// initialize this instance
	local_tmp275.begin (I2C_PINS_29_30, I2C_RATE_100);
	local_tmp275.init (0x03);

	hdc1080.setup (Wire1, (char*)"Wire1");							// initialize this sensor instance
	hdc1080.begin (I2C_PINS_29_30, I2C_RATE_100);
	}


//---------------------------< L O O P >----------------------------------------------------------------------

void loop (void)
	{
	if (!continuous_mode)
		startup_chirp (1);									// chirp once

	if (SUCCESS != tmp275_test(&local_tmp275))
		{
		display.display_line_write ("U6 not detected", ILI9341_RED);
		while(1);
		}
	else
		{
		snprintf (temp_display_buf, LINE_LEN, "TMP275:");
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		snprintf (temp_display_buf, LINE_LEN, "  %2.1fF", local_tmp275.data.deg_f);
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		}

// at the time of writing, there is no MS8607 library so no better way of detecting the presence of U7
//	if ((SUCCESS == pingex (0x40, Wire1)) && (SUCCESS == pingex (0x76, Wire1)))
//		{
//		display.display_line_write ("U7 detected", ILI9341_RED);
//		total_errs++;
//		}
//	else
//		display.display_line_write ("U7 not detected", ILI9341_GREEN);


	if (SUCCESS != hdc1080.init (MODE_T_AND_H))						// temperature and humidity mode
		{
		display.display_line_write ("U8 not detected", ILI9341_RED);
		while(1);
		}
	else
		{
		delay (100);												// wait for conversion to complete
		hdc1080.get_t_h_data ();
		snprintf (temp_display_buf, LINE_LEN, "HDC1080:");
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		snprintf (temp_display_buf, LINE_LEN, "  %2.1fF", hdc1080.data.deg_f);
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		snprintf (temp_display_buf, LINE_LEN, "  %2.1f%%", hdc1080.data.rh);
		display.display_line_write (temp_display_buf, ILI9341_CYAN);
		}


//	eep.setup (0x57, Wire1, (char*)"Wire1");
//	eep.begin (I2C_PINS_29_30, I2C_RATE_100);
//	eep.init ();

//	eep.set_addr16 (0);										// point to page 0, address 0

//	if ((SUCCESS == eep.byte_read()) && (0 != eep.control.rd_byte))
//		{
//		snprintf (temp_display_buf, LINE_LEN, "U9 detected (0x%.2X)", eep.control.rd_byte);
//		display.display_line_write (temp_display_buf, ILI9341_GREEN);
//		}
//	else
//		{
//		display.display_line_write ("U9 not detected", ILI9341_RED);
//		total_errs++;
//		}

	delay (5000);
	display.screen_clear();
	}
