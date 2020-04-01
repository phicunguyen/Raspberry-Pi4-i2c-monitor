/*
compile from Raspberry Pi4 shell:
    gcc -o i2c_monitor i2c_monitor.c -lwiringPi

run the code from shell:
    sudo ./i2c_monitor
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "i2c.h"


/* 
   SCL on gpio 23, which is Pin 4 for wiringPi
   SDA on gpio 24, which is Pin 5 for wiringPi
*/

#define SCL_PIN 4
#define SDA_PIN 5

static struct i2c_monitor_t i2c_mon;

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

int main(void) {
  if (wiringPiSetup () < 0) {
      fprintf (stderr, "wiringPi failed init: %s\n", strerror (errno));
      return 1;
  }

  /* setup isr rising interrupt for scl */
  if ( wiringPiISR (SCL_PIN, INT_EDGE_RISING, &sclInterrupt) < 0 ) {
      fprintf (stderr, "ISR not able to setup: %s\n", strerror (errno));
      return 1;
  }

  /* setup isr both edges interrupt for sda */
  if ( wiringPiISR (SDA_PIN, INT_EDGE_BOTH, &sdaInterrupt) < 0 ) {
      fprintf (stderr, "ISR not able to setup: %s\n", strerror (errno));
      return 1;
  }

  /* check if both scl and sda is hi */
  if (digitalRead(SCL_PIN) && digitalRead(SDA_PIN)) {
     printf("SCL: %d\n", digitalRead(SCL_PIN));
     printf("SDA: %d\n", digitalRead(SDA_PIN));
     i2cMonitorCreate();
     while ( 1 ) {
        delay(10); //delay 10ms
     }

  } else {
     printf("Failed: both scl and sda must be hi");
  }

  return 0;
}
