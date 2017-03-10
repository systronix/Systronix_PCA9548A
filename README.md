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
 
### Comments
 - Also included is an example/test program 
 - See the source code for plenty of explanatory comments

### TODO
 - add doxygen docs
