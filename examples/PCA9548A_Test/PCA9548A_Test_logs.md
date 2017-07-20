# PCA9548A_Test Logs

# i2c_t3 Library

## Adafruit breakout

### 2017 Jul 20
This is really strange: no changes to the hardware or setup, just a restart.
Yet somehow the same test program is working better.
Currently over 3 x 10e9 good with two errors
2017Jul20: et:618000 Good:2,847,141,023  4607/sec  bad:2
That is an error rate of *702 e-12* which is just under 1 ppb.
This is pretty good but still an error every 85.8 hours
```
	PCA9548A Library Test Code at 0x70
	Build May 22 2017 - 14:27:11
	C:\Users\BAB\Documents\code\Arduino\libraries\Systronix_PCA9548A\examples\PCA9548A_Test\PCA9548A_Test.ino
	CPU is T_3.1/3.2
	F_CPU =48000000
	Using i2c_t3 I2C library for Teensy
	More than 32 bits? 0, 0x0
	UINT32_MAX=0xFFFFFFFF, UINT64_MAX=0xFFFFFFFFFFFFFFFF
	UINT64_MAX=0xFFFFFFFFFFFFFFFF, UINT32_MAX=0xFFFFFFFF
	UINT32_MAX=4294967295, UINT64_MAX=18446744073709551615
	Test of UINT64_MAX seems to pass
	 Attempt #1: Init control reg to 0x01 - OK!
	 Interval is 1 sec, Setup Complete!
	Send V/v to toggle verbose, r/R for Wire.resetBus()
	In quiet mode: detailed output every 10 minutes

	.control write to ch 2 of 0x04 failed with return of 0x05: I2C_ADDR_NAK
	et:613714  Good:2827396741  4607/sec  bad:1  resetBus: 0


	control write to ch 5 of 0x20 failed with return of 0x05: I2C_ADDR_NAK
	et:618000  Good:2847141023  4607/sec  bad:2  resetBus: 0
```

### Up to July 11
These are just the errors 31..47, the rest got overwritten in the logs. 
I need to make a quiet mode and only report errors., so logs don't fill.

Same May 22 build on same hardware as above. Here:
et:3987113  Good:18,367,749,554  4606/sec  bad:47  resetBus: 0
That is an error rate of *2.56 e-9* which falls short of 1 ppb (1 e-9)
This is an error every 23.6 hours
```
	..........'.......control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:2597117  Good:11963448894  4606/sec  bad:31  resetBus: 0

	Error: control read value=0x0F
	et:2615621  Good:12048702662  4606/sec  bad:32  resetBus: 0

	..........'..........'.......Error: control read value=0x9F
	et:2758947  Good:12709081254  4606/sec  bad:33  resetBus: 0

	..........'..........'..........'....Error: control read value=0x00
	et:2768914  Good:12755003908  4606/sec  bad:34  resetBus: 0
	......'..........'........control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:2768938  Good:12755112671  4606/sec  bad:35  resetBus: 0

	......control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:2771286  Good:12765930814  4606/sec  bad:36  resetBus: 0

	....control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:2861224  Good:13180398519  4606/sec  bad:37  resetBus: 0

	..........'..........'..........'..........'..control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:2875782  Good:13247476654  4606/sec  bad:38  resetBus: 0

	..........'..........'.......control write to ch 5 of 0x20 failed with return of 0x06: I2C_DATA_NAK
	et:3045567  Good:14029737944  4606/sec  bad:39  resetBus: 0

	.......control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:3132667  Good:14431044799  4606/sec  bad:40  resetBus: 0

	.........Error: control read value=0xFF
	et:3190629  Good:14698092493  4606/sec  bad:41  resetBus: 0

	..........'..........'..........'....control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:3276694  Good:15094626250  4606/sec  bad:42  resetBus: 0

	....control write to ch 3 of 0x08 failed with return of 0x06: I2C_DATA_NAK
	et:3291005  Good:15160558400  4606/sec  bad:43  resetBus: 0

	..........'..........'..........'..........'control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:3348340  Good:15424724843  4606/sec  bad:44  resetBus: 0

	..........'..........'..........'..........'..........'........control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:3663778  Good:16878075234  4606/sec  bad:45  resetBus: 0

	.........'..........'..........'control write to ch 4 of 0x10 failed with return of 0x06: I2C_DATA_NAK
	et:3915630  Good:18038427194  4606/sec  bad:46  resetBus: 0

	..........'..........'..........'..........'..........'...control write to ch 4 of 0x10 failed with return of 0x05: I2C_ADDR_NAK
	et:3987113  Good:18367749554  4606/sec  bad:47  resetBus: 0

```

### 2017 May 21
good is a long... so 2**32 = 4,294,967,296 max
So good overflowed at 258.5 hours, but et did not so suddenly the quotient good/time is really low!
Making this a 64-bit unsigned (long long) would allow for 2**32 times as many hours. So not likely to wrap.
```
	et:930600  Good:4292831601  4612/sec  bad:29  resetBus: 0
	...
	et:931200  Good:632033  0/sec  bad:29  resetBus: 0
```
### 2017 May 20
2.4 billion good
```
	et:1313400  Good:1763673743  1342/sec  bad:46  resetBus: 0
	...
	control write to ch 5 of 0x20 failed with return of 0x05: I2C_ADDR_NAK
	...
	.....control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK

	et:461945  Good:2130955408  4613/sec  bad:14  resetBus: 0

	..........'..........'.........control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK

	et:520409  Good:2400656279  4613/sec  bad:15  resetBus: 0
	.'.....control write to ch 0 of 0x01 failed with return of 0x06: I2C_DATA_NAK

	et:520415  Good:2400683607  4613/sec  bad:16  resetBus: 0

	.......control write to ch 5 of 0x20 failed with return of 0x05: I2C_ADDR_NAK

	et:529687  Good:2443454257  4613/sec  bad:17  resetBus: 0

	Error: control read value=0x1F

	et:534530  Good:2465792519  4613/sec  bad:18  resetBus: 0
	..........'
	..........'..........'..........'..........'..........'..........
	et:534600  Good:2466110567  4613/sec  bad:18  resetBus: 0

	control write to ch 3 of 0x08 failed with return of 0x06: I2C_DATA_NAK

	et:535800  Good:2471646359  4613/sec  bad:18  resetBus: 0
	.'
	.........'..........'..........'..........'..........'..........'
	.........control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT

	et:535869  Good:2471966240  4613/sec  bad:19  resetBus: 0

	et:537348  Good:2478787750  4613/sec  bad:20  resetBus: 0
	..'control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT

	et:537350  Good:2478794095  4613/sec  bad:21  resetBus: 0
	..........'
	..........'..........'..........'...control write to ch 1 of 0x02 failed with return of 0x05: I2C_ADDR_NAK

	et:537393  Good:2478987985  4613/sec  bad:22  resetBus: 0
	control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK

	et:537393  Good:2478988322  4613/sec  bad:23  resetBus: 0

	..........'.........control write to ch 7 of 0x80 failed with return of 0x06: I2C_DATA_NAK

	et:545959  Good:2518500960  4613/sec  bad:24  resetBus: 0
	.'..........'..........'..........'..........
	et:546000  Good:2518684576  4612/sec  bad:24  resetBus: 0


	

	.'
	.........'..........'..........'..........'..........'..........'
	.........control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT

	et:535869  Good:2471966240  4613/sec  bad:19  resetBus: 0

```
### 2017 May 09
1.5 billion good
```
	..........'control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:17050  Good:78646086  4613/sec  bad:1  resetBus: 0

	..........'..........'..........'........control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:260138  Good:1200041699  4613/sec  bad:2  resetBus: 0
	..'....control write to ch 0 of 0x01 failed with return of 0x04: I2C_TIMEOUT
	et:260144  Good:1200068099  4613/sec  bad:3  resetBus: 0


	..........'..........'..........'...control read got 0 bytes, not 1
	control read failed with return of 0x04: I2C_TIMEOUT
	et:277773  Good:1281388948  4613/sec  bad:4  resetBus: 0


	....control read got 0 bytes, not 1
	control read failed with return of 0x05: I2C_ADDR_NAK
	et:279005  Good:1287068467  4613/sec  bad:5  resetBus: 0

	et:291600  Good:1345165267  4613/sec  bad:5  resetBus: 0
	.........'.....control write to ch 2 of 0x04 failed with return of 0x06: I2C_DATA_NAK
	et:291615  Good:1345236919  4613/sec  bad:6  resetBus: 0

	et:338400  Good:1561063367  4613/sec  bad:6  resetBus: 0
```

### 2017 May 07
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