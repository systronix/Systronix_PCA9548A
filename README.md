# Systronix_PCA9548A
Arduino library for the PCA- (NXP) nd TCA- (Texas Instruments) 9458A I2C MUX.

## PCA9548A Octal MUX Key Features
This device lets you take one I2C bus and switch it to up to 8 other I2C branches
 - $.58 @reel of 3000
 - Allows different voltages on the input and output I2C buses
 - POR all outputs deselected
 - hardware reset input
 - supports Hot Insertion
 - 5V tolerant inputs
 - 0 to 400 kHz clock
  - 3 address bits for 8 possible device addresses
 - Adafruit has a [breakout](https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout/overview) which we have used
 - TI has the TCA prefix. NXP uses PCA. They should be interchangeable and there may be other vendors. [TCA9548A data sheet](http://www.ti.com/product/tca9548a) and [PCA95458A datasheet](www.nxp.com/documents/data_sheet/PCA9548A.pdf)

 ## PCA9548A Cautions or Issues
 - you can select more than out active output at a time
 - this device is not a buffer, so it does not isolate capacitance between input and active output(s).
 - beware total I2C net loading which is the sum of input and output nets, including capacitance, and pullups.

 ## Examples
 - PCA9548A_Test.ino beats on the MUX pretty hard, setting each channel in turn active, with a nondestructive read/modify/write/restore test by calling testSimple()
 in the middle of every channel test. Output can be verbose or quiet by sending v or q to the serial monitor. For this example to work properly you want I2C_AUTO_RETRY
 and the resetBus counter included in my [minorly-revised i2c_t3 library](https://github.com/systronix/i2c_t3)
 - Mux_and_Temps acesses TMP275 temp sensors on multiple PCA9548A channels. It is stressfull, running I2C at 100% duty cycle at 100 kHz. Output:
	et:337  Good:1542913  4605/sec
	simple test in middle of write/read loop failed with return of 0x05: I2C_ADDR_NAK
	et:338  Good:1546141  4601/sec  bad:1  busReset: 0
This shows a ADDR NAK error at 337 seconds, already 1,542,913 I2C messages.
 
### Comments
 - This MUX can get into a state when reset or a cycle interrupted where it can drive SDA low. 
 The only receovery is to use i2c_t3 wonderful resetBus, which can be invoked automatically if  I2C_AUTO_RETRY is #define in i2c_t3.h.
 I'm curious to see if the same holds true for our custom hardware as for the Adafruit breakout.
 - See the source code for plenty of explanatory comments

### TODO
 - add doxygen docs
