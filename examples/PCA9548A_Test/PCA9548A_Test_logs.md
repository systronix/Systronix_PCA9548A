# PCA9548A_Test Logs

# i2c_t3 Library

## Adafruit breakout
### 2017 Mar 15 overnight
```
	PCA9548A Library Test Code at 0x70
	Build Mar 15 2017 - 17:43:16
	C:\Users\BAB\Documents\code\Arduino\libraries\Systronix_PCA9548A\examples\PCA9548A_Test\PCA9548A_Test.ino
	CPU is T_3.1/3.2
	...
	et:60064  Good:276390081  4601/sec
	et:60065  Good:276394673  4601/sec
```
	Why is this test more successful? Ran for 16.7 hours, 276 million good messages, no errors. This is just writing and reading the MUX control register, all 8 1-output enabled patterns.
### 2017 Mar 15 afternoon
```
	et:4050  Good:18658081  4609/sec
	et:4051  Good:18662689  4609/sec
	control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:4052  Good:18666458  4609/sec  bad:1  resetBus: 0
	et:4053  Good:18669514  4608/sec  bad:1  resetBus: 1
```
This afternoon I would also see some control write failures, all to the Adafruit board, not to our custom board. I can't imagine why the difference.

### 2017 Mar 14 night:
```
	et:46959  Good:216784769  4616/sec
	et:46960  Good:216789441  4616/sec
	simple test in middle of write/read loop failed with return of 0x06: I2C_DATA_NAK
	et:46961  Good:216791693  4616/sec  bad:1  resetBus: 0
	et:46962  Good:216796365  4616/sec  bad:1  resetBus: 0
```
If this simpleTest doesn't restore the control reg then the main test program loop() will fail.
I need change simpleTest to try to restore after an error, not just return. But the question 
remains: why does failure usually occur in this simpleTest and not the outer loop? Is it 
because I enable more than one output? Currently there are 3.3K pullups only on the SDA outputs. 
With 0xA5, that means 4 total outputs are enabled, so the total pullup value is 825 ohms, way
too strong.

## Systronix MUX board
### 2017 Mar 15 overnight
```
	et:60275  Good:277361057  4601/sec
	et:60276  Good:277365649  4601/sec
```	
Same code running on Adafruit breakout. 277 million good cycles, no errors. No longer executing the read-modify-restore "simpleTest" in the middle of the main loop MUX test.
###2 017 Mar 14 night:
```
	et:48436  Good:223599873  4616/sec
```
This board has 4.7K pullups so the total pullup for control of 0xA5 is 1.2K which is still too strong
but not as bad as 825 ohms.

# PJRC new Wire library
See PJRC Forum [thread about this](https://forum.pjrc.com/threads/42664-I2C-hanging-with-SDA-and-SCL-both-high-ARB_LOST-amp-TIMEOUT-forever-status?p=136793&viewfull=1#post136793)