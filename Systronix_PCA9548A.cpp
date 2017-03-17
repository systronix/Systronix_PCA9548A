/*
 * Systronix_PCA9548A.cpp
 *
 *  Created on: 2017 Feb 06
 *	  Author: BAB
 */

/**---------------------------< REVISIONS >----------------------------------

2017 Mar 15	bboyes	Changed tally_errors to properly record current Wire.status()
return values. Disabled simpleTest because it is not so simple and could return
with the config register changed in the middle of another loop which requires it
to be not changed.

-----------------------------------------------------------------------------*/

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
	error.total_error_count = 0;				// clear the error counter
	}


// destructor
Systronix_PCA9548A::~Systronix_PCA9548A()
{
	// Anything to do here? Leave I2C as master? Set flag?
}

/**
	Variables publicly read-only, privately read/write
*/

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
#if defined I2C_T3_H	
	Wire.setDefaultTimeout(10000);	// 1000 usec = 1 msec; 10,000 = 10 msec seems too long.
#endif

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
	
	Wire.beginTransmission (_base);			// base address
	// no return to check

	error.ret_val = Wire.write (control);		// write to control reg
	// return: #bytes written = success, 0=fail
	if (1 != error.ret_val)
		{
		tally_errors (0);		// data length error
		// now if error, error.ret_val is 0
		// and no point in continuing
		}
	else
		{
		// no error in buffer write so proceed
		error.ret_val = Wire.endTransmission();
		// return: 0=success, 1=data too long, 2=recv addr NACK, 3=recv data NACK, 4=other error	
	  	if (error.ret_val)
			{					
			// unsuccessful i2c transaction, get more detail
			// Serial.printf("init endTransmission failed with return of 0x%.2X\r\n", error.ret_val);
#if defined I2C_T3_H				
			error.ret_val = Wire.status();			// detailed status value enum 0..10
			// return = I2C_WAITING, I2C_SENDING, I2C_SEND_ADDR, I2C_RECEIVING, I2C_TIMEOUT, I2C_ADDR_NAK, I2C_DATA_NAK, I2C_ARB_LOST, I2C_BUF_OVF, I2C_SLAVE_TX, I2C_SLAVE_RX
			// Serial.printf("init endTransmission status value 0x%.2X\r\n", error.ret_val);
#endif			
			tally_errors (error.ret_val);							// increment the appropriate counter
			}
		else
			{
			// completely successful
			error.successful_count++;
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

Prevent setting more than one bit?

*/

uint8_t Systronix_PCA9548A::controlWrite (uint8_t control)
	{
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
#if defined I2C_T3_H					
			error.ret_val = Wire.status();			// to get error value
#endif			
			tally_errors (error.ret_val);				// increment the appropriate counter
			}
		else
			{
				// completely successful
				error.successful_count++;
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
	uint8_t read_count;

#if defined I2C_T3_H	
	read_count = Wire.requestFrom(_base, 1, I2C_STOP);
	if (read_count != 1)
#else
	read_count = Wire.requestFrom(_base, (uint8_t)1, (uint8_t)1); 		// Wire lib version
	if (read_count != 1 )		
#endif		
		{
		Serial.printf("control read got %u bytes, not 1\r\n", read_count);
#if defined I2C_T3_H	
		error.ret_val = Wire.status();				// to get error value
#else
		// PJRC Wire lib
		error.ret_val = 1;		// force value for data length error
#endif
		tally_errors (error.ret_val);					// increment the appropriate counter
		return FAIL;
		}

	error.successful_count++;
	*data = (uint8_t)Wire.read();
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

	stat = controlRead(&control_original);
	if (SUCCESS != stat) return !SUCCESS;		// if we can't even read why continue?

	// write test pattern
	stat = controlWrite(test_write);
	if (SUCCESS != stat) bad_count++;

	stat = controlRead(&control_read_val);
	if (SUCCESS != stat) bad_count++;
	if (test_write != control_read_val) 
	{
		bad_count++;
	}

	// write test pattern complement
	// dangerous, ~0x01 is 0xFE which enables 7 channels and any pullups on them,
	// could prevent slave driving low against such a large pullup load
	test_write = ~test_write;
	stat = controlWrite(test_write);
	if (SUCCESS != stat) return !SUCCESS;

	stat = controlRead(&control_read_val);
	if (SUCCESS != stat) return !SUCCESS;
	if (test_write != control_read_val) return !SUCCESS;

	stat = controlWrite(control_original);
	if (SUCCESS != stat) return !SUCCESS;

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

	From 29Dec16 release i2c_t3.h, now 11 status codes
	enum i2c_status   {I2C_WAITING,
	                   I2C_SENDING,
	                   I2C_SEND_ADDR,
	                   I2C_RECEIVING,
	                   I2C_TIMEOUT,
	                   I2C_ADDR_NAK,
	                   I2C_DATA_NAK,
	                   I2C_ARB_LOST,
	                   I2C_BUF_OVF,
	                   I2C_SLAVE_TX,
	                   I2C_SLAVE_RX};	

	Detailed status (not all are errors!) 11 values enum 0..10 returned from status()
		0..3 	I2C_WAITING, I2C_SENDING, I2C_RECEIVING, I2C_SEND_ADDR	- not errors, but cycle in progress when Teensy is master
		4 		I2C_TIMEOUT, 								- error
		5-6		I2C_ADDR_NAK, I2C_DATA_NAK, 				- errors
		7		I2C_ARB_LOST, 								- error, sort of (in multi-master mode)
		8 		I2C_BUF_OVF, 								- error
		9-10	I2C_SLAVE_TX, I2C_SLAVE_RX;					- not errors? Only apply to Teensy slave mode?
	0..2 can't happen if status() called after endTransmission since it blocks until cycle is complete

	If we are not using i2c_t3, then Wire errors are not so well-defined:
	0=success (we change this to mean incomplete write, 1=data too long, 2=recv addr NACK, 3=recv data NACK, 4=other error
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
			error.data_len_error_count++;
			break;
		case 4:					 
#if defined I2C_T3_H		
			error.timeout_count++;			// error 4 = i2c_t3 timeout
#else
			error.other_error_count++;		// error 4 = Wire "other error"
#endif
			break;
		case 2:
		case 5:
			error.rcv_addr_nack_count++;
			break;
		case 3:
		case 6:
			error.rcv_data_nack_count++;
			break;
		case 7:					// arbitration lost from call to status() (read)
			error.arbitration_lost_count++;
			break;
		case 8:
			error.buffer_overflow_count++;
			break;
		case 9:
		case 10:
			error.other_error_count++;		// i2c_t3 these are not errors, I think
			break;
		case 11:
		default:
			error.unknown_error_count++;
			break;
		}

	}
