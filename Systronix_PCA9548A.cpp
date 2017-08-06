/*
 * Systronix_PCA9548A.cpp
 *
 *  Created on: 2017 Feb 06
 *	  Author: BAB
 */

/**---------------------------< REVISIONS >----------------------------------

2017 Jul 12			rewrite to make Wire1 aware; to conform to the 'standard' established in PCA9557

2017 May 22 bboyes	change to UINT64_MAX

2017 May 21 bboyes	Changed total_error_count and successful_count to 64 bit 
long long and check against ULLONG_MAX before incrementing

2017 May 05 bboyes	Changed default i2c_t3 timeout to 5000 usec, which is still
likely way too much. There's nothing that slow in the 9557 access.

2017 Mar 15	bboyes	Changed tally_transaction to properly record current Wire.status()
return values. Disabled simpleTest because it is not so simple and could return
with the config register changed in the middle of another loop which requires it
to be not changed.

-----------------------------------------------------------------------------*/

#include <Systronix_PCA9548A.h>	



/**---------------------------< CONSTRUCTOR >----------------------------------

	@brief  Instantiates a new PCA9548A class to use the given base address
	base is clipped to min or max if necessary

-----------------------------------------------------------------------------*/
/*
Systronix_PCA9548A::Systronix_PCA9548A(uint8_t base)
	{
	if (base < PCA9548A_SLAVE_ADDR_0) 
		{
		_base = PCA9548A_SLAVE_ADDR_0;
		_base_clipped = true;
		}
	else if (base > PCA9548A_SLAVE_ADDR_7) 
		{
		_base = PCA9548A_SLAVE_ADDR_7;
		_base_clipped = true;
		}
	else
		{
		_base = base;
		_base_clipped = false;
		}
	error.total_error_count = 0;				// clear the error counter
	}
*/

//---------------------------< D E F A U L T   C O N S R U C T O R >------------------------------------------
//
// default constructor assumes lowest base address
//

Systronix_PCA9548A::Systronix_PCA9548A()
	{
	_base = PCA9548A_SLAVE_ADDR_0;
	_base_clipped = false;						// since it's constant it must be OK
	error.total_error_count = 0;				// clear the error counter
	}


//---------------------------< D E S T R U C T O R >----------------------------------------------------------
//
// destructor
//

Systronix_PCA9548A::~Systronix_PCA9548A()
{
	// Anything to do here? Leave I2C as master? Set flag?
}


//---------------------------< B A S E _ C L I P P E D >------------------------------------------------------
//
// base address passed in constructor or setup() was out of range so clipped to value min <= base <= max
//

boolean Systronix_PCA9548A::base_clipped()
{
	return _base_clipped;
}


//---------------------------< B A S E _ G E T >--------------------------------------------------------------
//
// return the base address; will be the same as the address in constructor or setup() unless clipped in which
// case base address shall be PCA9548A_SLAVE_ADDR_0 or PCA9548A_SLAVE_ADDR_7
//

uint8_t Systronix_PCA9548A::base_get()
{
	return _base;
}


//---------------------------< S E T U P >--------------------------------------------------------------------
//
// TODO: merge with begin()? This function doesn't actually do anything, it just sets some private values. It's
// redundant and some params must be effectively specified again in begin (Wire net and pins are not independent).	what parameters are specified again? [wsk]
//

uint8_t Systronix_PCA9548A::setup (uint8_t base, i2c_t3 wire, char* name)
	{
	if ((PCA9548A_BASE_MIN > base) || (PCA9548A_BASE_MAX < base))
		{
		tally_transaction (SILLY_PROGRAMMER);
		return FAIL;
		}

	_base = base;
	_wire = wire;
	_wire_name = wire_name = name;		// protected and public
	return SUCCESS;
	}


//---------------------------< B E G I N >--------------------------------------------------------------------
//
//
//

void Systronix_PCA9548A::begin (i2c_pins pins, i2c_rate rate)
	{
	_wire.begin(I2C_MASTER, 0x00, pins, I2C_PULLUP_EXT, rate);	// join I2C as master
//	Serial.printf("9548A lib begin %s\r\n", _wire_name);
	_wire.setDefaultTimeout(200000); // 200ms
	}


//---------------------------< D E F A U L T   B E G I N >----------------------------------------------------
/**
	@ brief Void version of begin for backwards compatibility, assumes Wire net

*/

void Systronix_PCA9548A::begin (void)
	{
	_wire.begin();	// join I2C as master
	_wire.setDefaultTimeout(200000); // 200ms
	}


/**---------------------------< I N I T >---------------------------------------------------------------------

Address the MUX and write to the control register.  
If successful, _exists = true;

TODO: if endTransmission times out its return should so indicate. See in i2c_t3 if it does and if not 
make that change? This might break code which assumes value can only be max of 4. In that case add 
a struct with more error information, have the library maintain it?

TODO How is this different from control write ???
**/

uint8_t Systronix_PCA9548A::init (uint8_t control)
	{
	uint8_t ret_val;

	error.exists = true;					// so we can use control_write(); we'll find out later if device does not exist

//	Serial.printf("9548A lib init %s at base 0x%.2X\r\n", _wire_name, _base);
	ret_val = control_write (control);		// if successful this means we got two ACKs from slave device
	if (SUCCESS != ret_val)
		{
//		Serial.printf("9548A lib init %s at base 0x%.2X failed with %s (0x%.2X)\r\n", _wire_name, _base, status_text[error.error_val], error.error_val);
		error.exists = false;				// only place error.exists is set false
		return ABSENT;
		}

	return SUCCESS;
	}


//---------------------------< R E S E T _ B U S >------------------------------------------------------------
/**
	Invoke resetBus of whichever Wire net this class instance is using
	@return nothing
*/
void Systronix_PCA9548A::reset_bus (void)
{
	_wire.resetBus();
}


//---------------------------< R E S E T _ B U S _ C O U N T _ R E A D >--------------------------------------
/**
	Return the resetBusCount of whichever Wire net this class instance is using
	@return number of Wire net resets, clips at UINT32_MAX
*/
uint32_t Systronix_PCA9548A::reset_bus_count_read (void)
{
	return _wire.resetBusCountRead();
}


//---------------------------< T A L L Y _ T R A N S A C T I O N >--------------------------------------------
/**
Here we tally errors.  This does not answer the what-to-do-in-the-event-of-these-errors question; it just
counts them.

TODO: we should decide if the correct thing to do when slave does not ack, or arbitration is lost, or
timeout occurs, or auto reset fails (states 2, 5 and 4, 7 ??? state numbers may have changed since this
comment originally added) is to declare these addresses as non-existent.

We need to decide what to do when those conditions occur if we do not declare the device non-existent.
When a device is declared non-existent, what do we do then? (this last is more a question for the
application than this library).  The questions in this TODO apply equally to other i2c libraries that tally
these errors.

Don't set error.exists = false here! These errors are likely recoverable. bab & wsk 170612

This is the only place we set error.error_val()

TODO use i2c_t3 error or status enumeration here in the switch/case
*/

void Systronix_PCA9548A::tally_transaction (uint8_t value)
	{
	if (value && (error.total_error_count < UINT64_MAX))
		error.total_error_count++; 			// every time here incr total error count

	error.error_val = value;

	switch (value)
		{
		case SUCCESS:
			if (error.successful_count < UINT64_MAX)
				error.successful_count++;
			break;
		case 1:								// i2c_t3 and Wire: data too long from endTransmission() (rx/tx buffers are 259 bytes - slave addr + 2 cmd bytes + 256 data)
			error.data_len_error_count++;
			break;
#if defined I2C_T3_H
		case I2C_TIMEOUT:
			error.timeout_count++;			// 4 from i2c_t3; timeout from call to status() (read)
#else
		case 4:
			error.other_error_count++;		// i2c_t3 and Wire: from endTransmission() "other error"
#endif
			break;
		case 2:								// i2c_t3 and Wire: from endTransmission()
		case I2C_ADDR_NAK:					// 5 from i2c_t3
			error.rcv_addr_nack_count++;
			break;
		case 3:								// i2c_t3 and Wire: from endTransmission()
		case I2C_DATA_NAK:					// 6 from i2c_t3
			error.rcv_data_nack_count++;
			break;
		case I2C_ARB_LOST:					// 7 from i2c_t3; arbitration lost from call to status() (read)
			error.arbitration_lost_count++;
			break;
		case I2C_BUF_OVF:
			error.buffer_overflow_count++;
			break;
		case I2C_SLAVE_TX:
		case I2C_SLAVE_RX:
			error.other_error_count++;		// 9 & 10 from i2c_t3; these are not errors, I think
			break;
		case WR_INCOMPLETE:					// 11; Wire.write failed to write all of the data to tx_buffer
			error.incomplete_write_count++;
			break;
		case SILLY_PROGRAMMER:				// 12
			error.silly_programmer_error++;
			break;
		default:
			error.unknown_error_count++;
			break;
		}
	}


//---------------------------< C O N T R O L _ W R I T E >----------------------------------------------------
/**
Write to the 8-bit control register
returns 0 if no error, positive values for NAK errors

Prevent setting more than one bit?

*/

uint8_t Systronix_PCA9548A::control_write (uint8_t control)
	{
	uint8_t ret_val;

	if (!error.exists)							// exit immediately if device does not exist
		return ABSENT;

	_wire.beginTransmission (_base);			// base address

	ret_val = _wire.write (control);			// returns # of bytes written to i2c_t3 buffer
	if (1 != ret_val)
		{
		tally_transaction (WR_INCOMPLETE);		// set the error value
		return FAIL;
		}

	ret_val = _wire.endTransmission();			// returns 0 if successful
	if (SUCCESS != ret_val)
		{
		tally_transaction (ret_val);			// increment the appropriate counter
		return FAIL;							// calling function decides what to do with the error
		}

	_control_reg = control;						// shadow copy to remember this setting
	tally_transaction (SUCCESS);
	return SUCCESS;
	}


//---------------------------< C O N T R O L _ R E A D >------------------------------------------------------
/**

  Read the 8-bit control register 

  return 0 if no error, FAIL if error (detail by using error.error_val, ABSENT if slave had prior NAK
*/

uint8_t Systronix_PCA9548A::control_read (uint8_t *data_ptr)
	{
	uint8_t ret_val;

	if (!error.exists)						// exit immediately if device does not exist
		return ABSENT;

	if (1 != _wire.requestFrom (_base, 1, I2C_STOP))
		{
		ret_val = _wire.status();						// to get error value
		tally_transaction (ret_val);					// increment the appropriate counter
		return FAIL;
		}

	*data_ptr = _wire.readByte();

	tally_transaction (SUCCESS);
	return SUCCESS;
	}


/**---------------------------< testSimple >-----------------------------------
	@brief  Enable multiple MUX outputs and see if we can still access control reg

	Read the control reg. Then write test patterns to it, reading to verify.
	Patterns are 0xA5 and its complement 0x5A
	Then restore to the original value.

	Error counters and exists will be updated by the subroutines which this
	function calls.

	return SUCCESS if all was OK, !SUCCESS if any problem
	error struct has details of error(s) if any

	TODO finish this.
-----------------------------------------------------------------------------*/

uint8_t Systronix_PCA9548A::enableManyTest (void)
{
	uint8_t control_read_val = 0;
	uint8_t control_original = 0;
	uint8_t stat = 0;
	uint8_t test_write = 0xA5;		// can enable multiple mux outputs at once
	uint8_t bad_count = 0;

	Serial.printf("enableManyTest is not complete! Use not recommended!\r\n");

	stat = control_read(&control_original);
	if (SUCCESS != stat) return !SUCCESS;		// if we can't even read why continue?

	// write test pattern
	stat = control_write(test_write);
	if (SUCCESS != stat) bad_count++;

	stat = control_read(&control_read_val);
	if (SUCCESS != stat) bad_count++;
	if (test_write != control_read_val) 
	{
		bad_count++;
	}

	// write test pattern complement
	// dangerous, ~0x01 is 0xFE which enables 7 ports and any pullups on them,
	// could prevent slave driving low against such a large pullup load
	test_write = ~test_write;
	stat = control_write(test_write);
	if (SUCCESS != stat) return !SUCCESS;

	stat = control_read(&control_read_val);
	if (SUCCESS != stat) return !SUCCESS;
	if (test_write != control_read_val) return !SUCCESS;

	stat = control_write(control_original);
	if (SUCCESS != stat) return !SUCCESS;

	return SUCCESS;
}
