# Raspberry-Pi4-i2c-monitor
The i2c monitor is using wiringPi interrupt edges trigger to monitor the i2c bus activites. 

Two pins on Raspberry Pi4 are configured as edges interrupt. GPIO23 is connected to SCL and GPIO24 is connected to SDA as an input.

The code below is using wiringPiISR to setup the interrupt callback.

    /* 
        SCL on gpio 23, which is Pin 4 for wiringPi
        SDA on gpio 24, which is Pin 5 for wiringPi
    */

    #define SCL_PIN 4
    #define SDA_PIN 5
    
        /*detecting rising edge on scl*/
        wiringPiISR (SCL_PIN, INT_EDGE_RISING, &sclInterrupt) < 0 ) {
        /*detecting both edges on sda*/
        wiringPiISR (SDA_PIN, INT_EDGE_BOTH, &sdaInterrupt) < 0 ) {
      
 I'm using the same i2c driver that I developed for stm32. You can look at the code in i2c.c
 
 First thing is create an i2c instance. 
     
     static struct i2c_monitor_t i2c_mon;
     
Second is passing the i2c_mon instance to the i2c_mon_create to intialized the inst; then set the scl and sda pin.

     void i2cMonitorCreate(void) {
         i2c_mon_create(&i2c_mon);
         i2c_mon.sda_pin_set(&i2c_mon, SDA_PIN);
         i2c_mon.scl_pin_set(&i2c_mon, SCL_PIN);
     }

     // called when scl edge goes from lo to hi 
     void sclInterrupt(void) {
         i2c_mon.i2c_mon_cb(&i2c_mon, SCL_PIN);
     }

     // called when sda edge changes from lo to hi or hi to lo
     // detect on both edge 
     void sdaInterrupt(void) {
         i2c_mon.i2c_mon_cb(&i2c_mon, SDA_PIN);
     }


The maxinum speed can achieve using this method is using 5Khz before losing data. I don't know how to improve the speed.
     
