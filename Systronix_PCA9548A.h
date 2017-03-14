#ifndef SYSTRONIX_PCA9548A_h
#define SYSTRONIX_PCA9548A_h


/******************************************************************************/
/*!
	@file		Systronix_PCA9548A.h
	
	@author		B Boyes (Systronix Inc)
    @license	BSD (see license.txt)	
    @section	HISTORY

    v0.1	2017Mar07 bboyes Start based on Systronix_TMP275

    Philosophy

*/


/**---------------------------< TODO LIST >------------------------------------


-----------------------------------------------------------------------------*/

/** --------  Description ------------------------------------------------------

See the data sheet at www.nxp.com/documents/data_sheet/PCA9548A.pdf
or www.ti.com/lit/ds/symlink/pca9548a.pdf 
This lbrary based on data sheet rev E, 2015 Nov, document SBOS363E.

------------------------------------------------------------------------------*/

/** -------- Arduino Device and Version Testing and Issues ---------------------

This library assumes Arduino greater than 1.00, with Wire.read() and Wire.write()

This library was developed and tested on Teensy3 (ARM CortexM4) with I2C_T3 library.

------------------------------------------------------------------------------*/

#include <Arduino.h>

// Use Teensy improved I2C library
//#if defined (__MK20DX256__) || defined (__MK20DX128__) 	// Teensy 3.1 or 3.2 || Teensy 3.0
// from https://forum.pjrc.com/threads/42411-Communication-impossible-in-I2C-tennsy3-6?p=135630&viewfull=1#post135630
#if defined(KINETISK) || defined(KINETISL)	// Teensy 3.X and LC
#include <i2c_t3.h>		
#else
#include <Wire.h>	// for AVR I2C library
#endif

#if not defined SUCCESS
#define		SUCCESS	0		// best to test for !SUCCESS
#define		FAIL	0xFF
#define		ABSENT	0xFD
#endif

/** --------  Device Addressing --------
PCA9548A base address is 0x70 (B 1001 000x) where x is R/W

This the upper 7 bits of the I2C address byte, which is how Arduino wants it.
The lsb of the address byte is the R/W bit which is not an address bit.
-------------------------------------*/
#define PCA9548A_SLAVE_ADDR_0 0x70
#define PCA9548A_SLAVE_ADDR_1 PCA9548A_SLAVE_ADDR_0+1
#define PCA9548A_SLAVE_ADDR_2 PCA9548A_SLAVE_ADDR_0+2
#define PCA9548A_SLAVE_ADDR_3 PCA9548A_SLAVE_ADDR_0+3
#define PCA9548A_SLAVE_ADDR_4 PCA9548A_SLAVE_ADDR_0+4
#define PCA9548A_SLAVE_ADDR_5 PCA9548A_SLAVE_ADDR_0+5
#define PCA9548A_SLAVE_ADDR_6 PCA9548A_SLAVE_ADDR_0+6
#define PCA9548A_SLAVE_ADDR_7 PCA9548A_SLAVE_ADDR_0+7


/** --------  Control Register --------
After the slave address, the master sends the control register value
which has bit 7..0, which if 1 enable channel [7..0]

More than one channel can be enabled at a time!
OR these bits to enable more than one.
*/

#define PCA9548A_CHAN_0_ENABLE 1
#define PCA9548A_CHAN_1_ENABLE 1<<1
#define PCA9548A_CHAN_2_ENABLE 1<<2
#define PCA9548A_CHAN_3_ENABLE 1<<3
#define PCA9548A_CHAN_4_ENABLE 1<<4
#define PCA9548A_CHAN_5_ENABLE 1<<5
#define PCA9548A_CHAN_6_ENABLE 1<<6
#define PCA9548A_CHAN_7_ENABLE 1<<7



class Systronix_PCA9548A
{
	protected:
		uint8_t		_base;						// base address for this instance; four possible values
		uint8_t		_control_reg;				// copy of the control register value so we know which channels enabled
		void		tally_errors (uint8_t);		// maintains the i2c_t3 error counters
		boolean		_base_clipped;

	public:
		// Instance-specific properties
		/** Data for one instance of a PCA9548A temp sensor.
		12-bit mode is assumed 
		Error counters could be larger but then they waste more data in the typical case where they are zero.
		Errors peg at max value for the data type: they don't roll over.

		**/
		/**
		Array of the mux channels so we can easily index into the desired channel X with uint8_t of value X
		*/
		const uint8_t channel[8] =
		{
			PCA9548A_CHAN_0_ENABLE,
			PCA9548A_CHAN_1_ENABLE,
			PCA9548A_CHAN_2_ENABLE,
			PCA9548A_CHAN_3_ENABLE,
			PCA9548A_CHAN_4_ENABLE,
			PCA9548A_CHAN_5_ENABLE,
			PCA9548A_CHAN_6_ENABLE,
			PCA9548A_CHAN_7_ENABLE
		};		

		/**
		Array of Wire.status() extended return code strings
		index into this with the value of status
		See NAP_UI_key_defs.h
		*/
		const char * const status_text[11] =
		{
			"I2C_WAITING", 
			"I2C_SENDING", 
			"I2C_SEND_ADDR",
			"I2C_RECEIVING",
			"I2C_TIMEOUT", 
			"I2C_ADDR_NAK", 
			"I2C_DATA_NAK",
			"I2C_ARB_LOST",
			"I2C_BUF_OVF",
			"I2C_SLAVE_TX", 
			"I2C_SLAVE_RX"
		};

		/** error stucture
		Note that this can be written by a library user, so it could be cleared if desired as part of 
		some error recovery or logging operation. It could also be inadvertenly erased...
		*/
		struct
			{
			uint8_t		ret_val;						// i2c_t3 library return value from most recent transaction
			uint32_t	incomplete_write_count;			// Wire.write failed to write all of the data to tx_buffer
			uint32_t	data_len_error_count;			// data too long
			uint32_t	rcv_addr_nack_count;			// slave did not ack address
			uint32_t	rcv_data_nack_count;			// slave did not ack data
			uint32_t	other_error_count;				// arbitration lost or timeout
			uint32_t	unknown_error_count;
			uint32_t	total_error_count;				// quick check to see if any have happened
			uint32_t	successful_count;				// successful access cycle
			} error;

		boolean base_clipped();
		uint8_t base_address();

		Systronix_PCA9548A (uint8_t base);				// constructor w/base passed in
		Systronix_PCA9548A ();							// default constructor
		~Systronix_PCA9548A();							// deconstructor

		void		begin (void);
		uint8_t		init (uint8_t control_value);		// set operation mode, check device present and communicating

		uint8_t		controlWrite (uint8_t pointer);		// 
		uint8_t		controlRead (uint8_t *data);		// read 8-bit control register
		uint8_t		testSimple (void);					// simple nondestructive test of control read/write
		uint8_t		resetAll (void);					// reset device if possible, re-init, clear all errors

	private:

};

extern Systronix_PCA9548A pca9548a;

#endif /* SYSTRONIX_PCA9548A_h */
