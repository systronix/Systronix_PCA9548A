/*
 * Systronix_PCA9548A.cpp
 *
 *  Created on: 2017 Feb 06
 *	  Author: BAB
 */

#include <Systronix_PCA9548A.h>	

/**---------------------------< CONSTRUCTOR >----------------------------------

	@brief  Instantiates a new PCA9548A class to use the given base address
	base is clipped to min or max if necessary

-----------------------------------------------------------------------------*/

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


// default constructor assumes lowest base address
Systronix_PCA9548A::Systronix_PCA9548A()
	{
	_base = PCA9548A_SLAVE_ADDR_0;
	_base_clipped = false;	// since it's constant it must be OK
	error.total_error_count = true;				// clear the error counter
	}


// destructor
Systronix_PCA9548A::~Systronix_PCA9548A()
{
	// Anything to do here? Leave I2C as master? Set flag?
}

/**
	Variables publicly read-only, privately read/write
*/
// sensor is instantiated and responds to I2C at base address
boolean Systronix_PCA9548A::exists()
{
	return _exists;
}

// base address passed in constructor was out of range so clipped to value min <= base <= max
boolean Systronix_PCA9548A::base_clipped()
{
	return _base_clipped;
}

uint8_t Systronix_PCA9548A::base_address()
{
	return _base;
}


//---------------------------< B E G I N >--------------------------------------------------------------------
// One-time startup things here. Call this only at program start.
/*!
	@brief  Join the I2C bus as a master
*/

void Systronix_PCA9548A::begin(void)
	{
	Wire.begin();			// join I2C as master

	Wire.setDefaultTimeout(1000);	// 1000 usec = 1 msec

	}


/**---------------------------< I N I T >----------------------------------------------------------------------

Address the MUX and write to the control register.  
If successful, _exists = true;

TODO: if endTransmission times out its return should so indicate. See in i2c_t3 if it does and if not 
make that change? This might break code which assumes value can only be max of 4. In that case add 
a struct with more error information, have the library maintain it?

TODO How is this different from control write ???
**/

uint8_t Systronix_PCA9548A::init (uint8_t control)
	{
	_exists = true;							// anticipate success
	
	Wire.beginTransmission (_base);			// base address
	// no return to check

	error.ret_val = Wire.write (control);		// write to control reg
	// return: #bytes written = success, 0=fail
	if (1 != error.ret_val)
		{
		tally_errors (0);		// data length error
		// now if error, error.ret_val is 0
		// and no point in continuing
		// we don't know device exists if we can't communicate
		_exists = false;
		}
	else
		{
		// no error in buffer write so proceed
		error.ret_val = Wire.endTransmission();
		// return: 0=success, 1=data too long, 2=recv addr NACK, 3=recv data NACK, 4=other error	
	  	if (error.ret_val)
			{					
			// unsuccessful i2c transaction, get more detail
			error.ret_val = Wire.status();			// detailed error value enum 0..9
			// return = I2C_WAITING, I2C_SENDING, I2C_RECEIVING, I2C_TIMEOUT, I2C_ADDR_NAK, I2C_DATA_NAK, I2C_ARB_LOST, I2C_BUF_OVF, I2C_SLAVE_TX, I2C_SLAVE_RX;
			tally_errors (error.ret_val);							// increment the appropriate counter
			if ((I2C_ADDR_NAK==error.ret_val) || (I2C_DATA_NAK==error.ret_val))
				{
				_exists = false;		// addr or data NACK so device is not responding, assume it's not present
				}
			}
		else
			{
				// completely successful
				_control_reg = control;					// shadow copy to remember this setting
				return SUCCESS;
			}
		}
										
	return !SUCCESS;

	// TODO this could return with error.ret_val set to zero
	}


/**---------------------------< C O N T R O L W R I T E >----------------------------------------------------

Write to the 8-bit control register
returns 0 if no error, positive values for NAK errors

*/

uint8_t Systronix_PCA9548A::controlWrite (uint8_t control)
	{
	if (!_exists)							// exit immediately if device does not exist
		return ABSENT;

	Wire.beginTransmission (_base);			// base address
	error.ret_val = Wire.write (control);			// write control reg
	_control_reg = control;					// shadow copy to remember this setting

	if (1 != error.ret_val)
		{
		tally_errors (0);			// buffer write length error
		}
	else
		{
		// no error in buffer write
	  	error.ret_val = Wire.endTransmission();	
		// return: 0=success, 1=data too long, 2=recv addr NACK, 3=recv data NACK, 4=other error
		if (error.ret_val)
			{
			error.ret_val = Wire.status();			// to get error value
			tally_errors (error.ret_val);				// increment the appropriate counter
			if ((I2C_ADDR_NAK==error.ret_val) || (I2C_DATA_NAK==error.ret_val))
				{
				_exists = false;		// addr or data NACK so device is not responding, assume it's not present
				}
			}
		else
			{
				// completely successful
				_control_reg = control;					// shadow copy to remember this setting
				return SUCCESS;
			}
		}
	return !SUCCESS;
	// TODO this could return with error.ret_val set to zero
	}

/**--------------------------< C O N T R O L R E A D >------------------------------------------------------

  Read the 8-bit control register 

  return 0 if no error, FAIL if error (detail by using error.ret_val, ABSENT if slave had prior NAK
*/

uint8_t Systronix_PCA9548A::controlRead (uint8_t *data)
	{
	if (!_exists)								// exit immediately if device does not exist
		return ABSENT;

	if (1 != Wire.requestFrom(_base, 1, I2C_STOP))
		{
		error.ret_val = Wire.status();				// to get error value
		tally_errors (error.ret_val);					// increment the appropriate counter
		return FAIL;
		}

	*data = (uint8_t)Wire.read();
	return SUCCESS;
	}

//---------------------------< T A L L Y _ E R R O R S >------------------------------------------------------
//
// Here we tally errors.  This does not answer the 'what to do in the event of these errors' question; it just
// counts them.  If the device does not ack the address portion of a transaction or if we get a timeout error,
// exists is set to false.  We assume here that the timeout error is really an indication that the automatic
// reset feature of the i2c_t3 library failed to reset the device in which case, the device no longer 'exists'
// for whatever reason.
//
// Only call this method if there is some kind of error, since total is incremented on every entry here.
/** 
	TODO these error values don't match all the 0..9 status() enum e.g. timeout has a value of 3
	part of the problem is mashing up status() codes and Wire.write() 

	Detailed error value enum 0..9 returned from status()
		0..2 	I2C_WAITING, I2C_SENDING, I2C_RECEIVING, 	- not errors, but cycle in progress flags
		3 		I2C_TIMEOUT, 
		4-5		I2C_ADDR_NAK, I2C_DATA_NAK, 
		6		I2C_ARB_LOST, 
		7 		I2C_BUF_OVF, 
		8-9		I2C_SLAVE_TX, I2C_SLAVE_RX;
	0..2 can't happen is status() called after endTransmission
*/

void Systronix_PCA9548A::tally_errors (uint8_t err)
	{
	if (error.total_error_count < 0xFFFFFFFF) error.total_error_count++; 
	switch (err)
		{
		case 0:					// Wire.write failed to write all of the data to tx_buffer
			error.incomplete_write_count++;
			break;
		case 1:					// data too long from endTransmission() (rx/tx buffers are 259 bytes - slave addr + 2 cmd bytes + 256 data)
		case 8:					// buffer overflow from call to status() (read - transaction never started)
			error.data_len_error_count++;
			break;
		case 2:					// slave did not ack address (write)
		case 5:					// from call to status() (read)
			error.rcv_addr_nack_count++;
			break;
		case 3:					// slave did not ack data (write)
		case 6:					// from call to status() (read)
			error.rcv_data_nack_count++;
			break;
		case 4:					// arbitration lost (write) or timeout (read/write) or auto-reset failed
								// or "other" error from Wire.endTransmission
		case 7:					// arbitration lost from call to status() (read)
			error.other_error_count++;
			break;
		default:
			error.unknown_error_count++;
			break;
		}

	}
