# PCA9548A_Test Logs

# i2c_t3 Library

## Adafruit breakout
2017 May 07
869 million good in 52 hrs. Why is this rate so much higher
than the previous test: 247/sec?
```
	..........'control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	...
	et:188400  Good:869101526  4613/sec  bad:1  resetBus: 0
```

2017 May 05
Different message output to reduce log file size.
No changes to text procedure. 
Build with Arduino 1.8.2, TD 1.36

```
	..........'control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:17050  Good:78646086  4613/sec  bad:1  resetBus: 0
```

### 2017 May 05
594 hours (24.8 days). 486 million good. Why is messages per second so slow?
Stopping to load some revised version.
```
	et:1968739  Good:486398717  247/sec  bad:91  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:1968740  Good:486400970  247/sec  bad:92  resetBus: 1
	...
	et:1973468  Good:508196138  257/sec  bad:92  resetBus: 1
	control write to ch 3 of 0x08 failed with return of 0x06: I2C_DATA_NAK
	et:1973469  Good:508198480  257/sec  bad:93  resetBus: 1
	...
	et:2079859  Good:998636304  480/sec  bad:93  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:2079860  Good:998640301  480/sec  bad:94  resetBus: 1
	...
	
	et:2139892  Good:1275382429  596/sec  bad:94  resetBus: 1
```

### 2017 Apr 29
Six days since the last bad message. Now 2.8 billion I2C cycles
```
	et:1539952  Good:2804738254  1821/sec  bad:87  resetBus: 1
	control write to ch 1 of 0x02 failed with return of 0x04: I2C_TIMEOUT
	et:1539953  Good:2804740512  1821/sec  bad:88  resetBus: 1
```
### 2017 Apr 23
While working today, noticed new errors.
```
et:1128379  Good:907493435  804/sec  bad:86  resetBus: 1
control read got 0 bytes, not 1
control read failed with return of 0x05: I2C_ADDR_NAK
et:1128380  Good:907495742  804/sec  bad:87  resetBus: 1
```

```
et:1124654  Good:890324349  791/sec  bad:85  resetBus: 1
control write to ch 7 of 0x80 failed with return of 0x06: I2C_DATA_NAK
et:1124655  Good:890327355  791/sec  bad:86  resetBus: 1
```

311 hours, 881x10e6 good cycles
```
	et:1122780  Good:881685613  785/sec  bad:85  resetBus: 1
	et:1122781  Good:881690221  785/sec  bad:85  resetBus: 1
```	
### 2017 Apr 12 
This is 43 hours after a reset.
```
	et:156747  Good:722689166  4610/sec  bad:19  resetBus: 1
	et:156748  Good:722693774  4610/sec  bad:19  resetBus: 1
```
### 2017 Apr 04 
after running over 2 weeks, almost 1.1 billion good cycles
```
	et:1169822  Good:1096874442  937/sec  bad:28  resetBus: 0
	...

	et:1363479  Good:1989449786  1459/sec  bad:28  resetBus: 0
	et:1363480  Good:1989454394  1459/sec  bad:28  resetBus: 0
	control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:1363481  Good:1989457311  1459/sec  bad:29  resetBus: 0
	et:1363482  Good:1989461263  1459/sec  bad:29  resetBus: 0

```
### 2017 Mar 18 night
```
	// 2 errors in the first 16 minutes...
	et:893  Good:4104129  4606/sec
	control write to ch 3 of 0x08 failed with return of 0x06: I2C_DATA_NAK
	et:894  Good:4107271  4604/sec  bad:1  resetBus: 0

	et:922  Good:4235511  4603/sec  bad:1  resetBus: 0
	control write to ch 3 of 0x08 failed with return of 0x06: I2C_DATA_NAK
	et:923  Good:4238653  4602/sec  bad:2  resetBus: 0	

	// Then 64,000 seconds (18 hours) with zero errors...
	et:64707  Good:298277629  4609/sec  bad:2  resetBus: 0
	et:64708  Good:298282237  4609/sec  bad:2  resetBus: 0	
```
### 2017  Mar 18 day
```
	PCA9548A Library Test Code at 0x70
	Build Mar 17 2017 - 15:46:07
	C:\Users\BAB\Documents\code\Arduino\libraries\Systronix_PCA9548A\examples\PCA9548A_Test\PCA9548A_Test.ino

	et:86046  Good:396674369  4610/sec
	et:86047  Good:396678993  4610/sec
	control write to ch 3 of 8 failed with return of 0x06: I2C_DATA_NAK
	et:86048  Good:396681287  4610/sec  bad:1  resetBus: 1
	Error: control read value=0x27
	et:86049  Good:396683587  4610/sec  bad:1  resetBus: 1
	et:86050  Good:396688211  4610/sec  bad:1  resetBus: 1

	et:88941  Good:410015603  4610/sec  bad:1  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:88942  Good:410018760  4610/sec  bad:2  resetBus: 1

	et:89087  Good:410686312  4610/sec  bad:2  resetBus: 1
	control write to ch 6 of 64 failed with return of 0x05: I2C_ADDR_NAK
	et:89088  Good:410689300  4610/sec  bad:3  resetBus: 1

	et:89346  Good:411877972  4610/sec  bad:3  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK

	et:93492  Good:430988585  4609/sec  bad:4  resetBus: 1
	control write to ch 2 of 4 failed with return of 0x05: I2C_ADDR_NAK
	et:93493  Good:430992941  4609/sec  bad:5  resetBus: 1	
	et:89347  Good:411880793  4610/sec  bad:4  resetBus: 1

	et:93659  Good:431756141  4609/sec  bad:5  resetBus: 1
	control write to ch 6 of 64 failed with return of 0x06: I2C_DATA_NAK
	et:93660  Good:431758457  4609/sec  bad:6  resetBus: 1	

	et:94778  Good:436912393  4609/sec  bad:6  resetBus: 1
	control write to ch 5 of 32 failed with return of 0x06: I2C_DATA_NAK
	et:94779  Good:436916147  4609/sec  bad:7  resetBus: 1	

	et:96994  Good:447118849  4609/sec  bad:8  resetBus: 1
	control write to ch 7 of 128 failed with return of 0x05: I2C_ADDR_NAK
	et:96995  Good:447121151  4609/sec  bad:9  resetBus: 1
	control write to ch 7 of 128 failed with return of 0x06: I2C_DATA_NAK
	et:96996  Good:447123453  4609/sec  bad:10  resetBus: 1	

	et:97091  Good:447561389  4609/sec  bad:10  resetBus: 1
	control write to ch 4 of 16 failed with return of 0x05: I2C_ADDR_NAK
	et:97092  Good:447563701  4609/sec  bad:11  resetBus: 1

	et:97182  Good:447978581  4609/sec  bad:11  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:97183  Good:447980880  4609/sec  bad:12  resetBus: 1

	et:97725  Good:450479456  4609/sec  bad:12  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:97726  Good:450481767  4609/sec  bad:13  resetBus: 1

	et:97780  Good:450730695  4609/sec  bad:13  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:97781  Good:450733006  4609/sec  bad:14  resetBus: 1
	control write to ch 3 of 8 failed with return of 0x05: I2C_ADDR_NAK
	et:97782  Good:450737332  4609/sec  bad:15  resetBus: 1

	// TODO why didn't bad counter increment here?
	// The read was an OK I2C message but the value was wrong. Interesting.
	et:99210  Good:457318244  4609/sec  bad:15  resetBus: 1
	Error: control read value=0x00
	et:99211  Good:457320548  4609/sec  bad:15  resetBus: 1

	et:99225  Good:457385092  4609/sec  bad:15  resetBus: 1
	control write to ch 7 of 128 failed with return of 0x05: I2C_ADDR_NAK
	et:99226  Good:457389282  4609/sec  bad:16  resetBus: 1

	et:99384  Good:458115762  4609/sec  bad:16  resetBus: 1
	Error: control read value=0xFF
	et:99385  Good:458118060  4609/sec  bad:16  resetBus: 1	

	et:99435  Good:458348556  4609/sec  bad:16  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:99436  Good:458350867  4609/sec  bad:17  resetBus: 1	

	et:100845  Good:464846195  4609/sec  bad:17  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:100846  Good:464850794  4609/sec  bad:18  resetBus: 1	

	et:101394  Good:467374714  4609/sec  bad:18  resetBus: 1
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:101395  Good:467378453  4609/sec  bad:19  resetBus: 1	
```
### 2017 Mar 16 day
```
	et:79847  Good:367445969  4601/sec
	et:79848  Good:367450577  4601/sec
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:79849  Good:367454294  4601/sec  bad:1  resetBus: 0
	et:79850  Good:367457478  4601/sec  bad:1  resetBus: 0
	...
	after a reset
	'''
	et:213  Good:967105  4583/sec
	et:214  Good:971713  4583/sec
	control write of 5 failed with return of 0x04: I2C_TIMEOUT
	et:215  Good:975947  4581/sec  bad:1  resetBus: 0
	et:216  Good:978571  4572/sec  bad:1  resetBus: 0
	...
	et:2079  Good:9551899  4598/sec  bad:1  resetBus: 0
	et:2080  Good:9556491  4598/sec  bad:1  resetBus: 0
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:2081  Good:9558800  4597/sec  bad:2  resetBus: 0
	et:2082  Good:9563392  4597/sec  bad:2  resetBus: 0
	...
	et:2467  Good:11335120  4598/sec  bad:2  resetBus: 0
	et:2468  Good:11339712  4598/sec  bad:2  resetBus: 0
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:2469  Good:11342011  4597/sec  bad:3  resetBus: 0
	et:2470  Good:11346619  4597/sec  bad:3  resetBus: 0
	...
	et:18490  Good:85067995  4601/sec  bad:3  resetBus: 0
	et:18491  Good:85072603  4601/sec  bad:3  resetBus: 0
	control write of 7 failed with return of 0x06: I2C_DATA_NAK
	et:18492  Good:85075385  4601/sec  bad:4  resetBus: 0
	et:18493  Good:85079497  4601/sec  bad:4  resetBus: 0	
```
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
### 2017 Mar 18 night, same time as Adafruit breakout, same code
```
	et:64761  Good:298543985  4610/sec
	et:64762  Good:298548593  4610/sec
	et:64763  Good:298553201  4610/sec
```
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