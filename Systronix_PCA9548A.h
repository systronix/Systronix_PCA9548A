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

/**---------------------------< REVISIONS >----------------------------------
2017 Jul 12			rewrite to make Wire1 aware; to conform to the 'standard' established in PCA9557
2017 May 21 bboyes	Changed total_error_count and successful_count to 64 bit 

2017 Mar 15	bboyes	error struct names and variables to properly match current 
Wire.status() return values. 

-----------------------------------------------------------------------------*/


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

//---------------------------< I N C L U D E S >--------------------------------------------------------------

#include <Arduino.h>
#include <Systronix_i2c_common.h>


//---------------------------< D E F I N E S >----------------------------------------------------------------

/** --------  Device Addressing --------
PCA9548A base address is 0x70 (B 1001 000x) where x is R/W

This the upper 7 bits of the I2C address byte, which is how Arduino wants it.
The lsb of the address byte is the R/W bit which is not an address bit.
-------------------------------------*/
#define		PCA9548A_SLAVE_ADDR_0		0x70
#define		PCA9548A_SLAVE_ADDR_1		PCA9548A_SLAVE_ADDR_0+1
#define		PCA9548A_SLAVE_ADDR_2		PCA9548A_SLAVE_ADDR_0+2
#define		PCA9548A_SLAVE_ADDR_3		PCA9548A_SLAVE_ADDR_0+3
#define		PCA9548A_SLAVE_ADDR_4		PCA9548A_SLAVE_ADDR_0+4
#define		PCA9548A_SLAVE_ADDR_5		PCA9548A_SLAVE_ADDR_0+5
#define		PCA9548A_SLAVE_ADDR_6		PCA9548A_SLAVE_ADDR_0+6
#define		PCA9548A_SLAVE_ADDR_7		PCA9548A_SLAVE_ADDR_0+7

#define		PCA9548A_BASE_MIN			PCA9548A_SLAVE_ADDR_0		// 7-bit address not including R/W bit
#define		PCA9548A_BASE_MAX 			PCA9548A_SLAVE_ADDR_7		// 7-bit address not including R/W bit

#define		WR_INCOMPLETE				11
#define		SILLY_PROGRAMMER			12


/** --------  Control Register --------
After the slave address, the master sends the control register value
which has bit 7..0, which if 1 enable port [7..0]

More than one port can be enabled at a time!
OR these bits to enable more than one.
*/

#define		PCA9548A_PORTS_DISABLE		0
#define		PCA9548A_PORT_0_ENABLE		1
#define		PCA9548A_PORT_1_ENABLE		(1<<1)
#define		PCA9548A_PORT_2_ENABLE		(1<<2)
#define		PCA9548A_PORT_3_ENABLE		(1<<3)
#define		PCA9548A_PORT_4_ENABLE		(1<<4)
#define		PCA9548A_PORT_5_ENABLE		(1<<5)
#define		PCA9548A_PORT_6_ENABLE		(1<<6)
#define		PCA9548A_PORT_7_ENABLE		(1<<7)



class Systronix_PCA9548A
{
	protected:
		uint8_t		_base;								// base address for this instance; four possible values
		uint8_t		_control_reg;						// copy of the control register value so we know which ports enabled
		void		tally_transaction (uint8_t);		// maintains the i2c_t3 error counters
		boolean		_base_clipped;

		char* 		_wire_name = (char*)"empty";
		i2c_t3		_wire = Wire;						// why is this assigned value = Wire? [bab]

	public:
		// Instance-specific properties
		/** Data for one instance of a PCA9548A temp sensor.
		12-bit mode is assumed 
		Error counters could be larger but then they waste more data in the typical case where they are zero.
		Errors peg at max value for the data type: they don't roll over.

		**/
		/**
		Array of the mux ports so we can easily index into the desired port X with uint8_t of value X
		*/
		const uint8_t port[8] =
		{
			PCA9548A_PORT_0_ENABLE,
			PCA9548A_PORT_1_ENABLE,
			PCA9548A_PORT_2_ENABLE,
			PCA9548A_PORT_3_ENABLE,
			PCA9548A_PORT_4_ENABLE,
			PCA9548A_PORT_5_ENABLE,
			PCA9548A_PORT_6_ENABLE,
			PCA9548A_PORT_7_ENABLE
		};		

		/**
		Array of Wire.status() extended return code strings, 11 as of 29Dec16 i2c_t3 release
		index into this with the value of status.
		There is an array of constant text: const status_text[11]
		char * makes the decl an array of char pointers, each pointing to constant text
		the first const means that array of char pointers can't change.
		We can access this with a const char * text_ptr which means point to char(s) which happen to be const
		Note each literal string has a null terminator added by C compiler.
		See NAP_UI_key_defs.h for similar

		TODO A problem is that SUCCESS returns 0 and gets put into error_val, so
		we can't tell the difference between SUCCESS and I2C_WAITING
		Since requestFrom is blocking, only "I2C message is over" status can occur.
		In Writing, with endTransmission, it is blocking, so only end of message errors can exist.
		*//*
#if defined I2C_T3_H 		
		const char * const status_text[13] =
		{
			"I2C_WAITING", 		// first four are not errors but status; first eleven taken from i2c_t3.h
			"I2C_SENDING", 
			"I2C_SEND_ADDR",
			"I2C_RECEIVING",
			"I2C_TIMEOUT", 		// start of 5 errors, status==4
			"I2C_ADDR_NAK", 
			"I2C_DATA_NAK",
			"I2C_ARB_LOST",
			"I2C_BUF_OVF",
			"I2C_SLAVE_TX", 	// slave status; not errors
			"I2C_SLAVE_RX",
			"WR_INCOMPLETE",
			"SILLY_PROGRAMMER"	// Doh. Slap forehead.
		};
#else
		// Wire.h returns from endTransmission
		// 0=success, 1=data too long, 2=recv addr NACK, 3=recv data NACK, 4=other error
		const char * const status_text[5] =
		{
			"Success",
			"Data length",
			"Receive addr NAK", 
			"Receive data NAK",
			"Other error"
		};		
#endif
*/
		/** error stucture
		Note that this can be written by a library user, so it could be cleared if desired as part of 
		some error recovery or logging operation. It could also be inadvertenly erased...

		successful_count overflowed at 258.5 hours. Making this a 64-bit unsigned (long long) allows
		for 2**32 times as many hours. So not likely to ever wrap wrap.
		*/

 		error_t		error;								// error struct typdefed in Systronix_i2c_common.h

		char*		wire_name;							// name of Wire, Wire1, etc in use

		boolean base_clipped();
		uint8_t base_get();

//		Systronix_PCA9548A (uint8_t base);				// constructor w/base passed in
		Systronix_PCA9548A ();							// default constructor
		~Systronix_PCA9548A();							// deconstructor

		uint8_t		setup (uint8_t base, i2c_t3 wire, char* name);
		void 		begin (i2c_pins pins, i2c_rate rate);
		void		begin (void);						// default begin
		uint8_t		init (uint8_t control=PCA9548A_PORTS_DISABLE);		// set operation mode, check device present and communicating

		void		reset_bus (void);
		uint32_t	reset_bus_count_read (void);
		uint8_t		control_write (uint8_t pointer);	//
		uint8_t		control_read (uint8_t *data);		// read 8-bit control register

		uint8_t		enableManyTest (void);				// TODO finish this 
		uint8_t		resetAll (void);					// reset device if possible, re-init, clear all errors

	private:

};

extern Systronix_PCA9548A pca9548a;

#endif /* SYSTRONIX_PCA9548A_h */
