/*
 * i2c_monitor.h
 *
 *  Created on: Nov 20, 2019
 *      Author: pnguyen
 */

#ifndef I2C_MONITOR_I2C_MONITOR_H_
#define I2C_MONITOR_I2C_MONITOR_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <wiringPi.h>


#define GPIO_PIN_SET            1
#define GPIO_PIN_RESET          0

//forward declaration
struct i2c_monitor_t;

/***************** defines go here ************************/
// I2C state machine
#define I2C_STATUS_IDLE			0
#define I2C_STATUS_START		1
#define I2C_STATUS_DATA 		2
#define I2C_STATUS_RESTART 		3

/***************** typedef define go here ************************/
typedef void (*I2C_pin)(struct i2c_monitor_t *i2c, uint8_t pin);
typedef void (*I2C_init)(struct i2c_monitor_t *i2c);
typedef void (*I2C_cb)(struct i2c_monitor_t *i2c, uint8_t pin);

/***************** structs go here ************************/
struct i2c_monitor_t {
	I2C_pin		sda_pin_set;		//user call this to set sda port/pin
	I2C_pin		scl_pin_set;		//user call this to set sda port/pin
	uint16_t 	scl_pin;		//copy of user  scl pin
	uint16_t   	sda_pin;		//copy of user sda pin
	uint8_t		i2c_buffer[512];	//store i2c data
	uint16_t	i2c_buffer_idx;		//count number of i2c data

	I2C_cb		i2c_mon_cb;		//i2c monitor call back from ext interrupt

	uint8_t 	i2c_state;		//i2c state: start, stop, repeat or i2c data
	uint8_t 	i2c_bit_count;		//count how many bits
	uint8_t 	i2c_data_bit;		//save data bit
	uint8_t 	i2c_done;		//is i2c done yet
};

/***************** global functions go here ************************/
void i2c_mon_create(struct i2c_monitor_t *i2c);

#endif /* I2C_MONITOR_I2C_MONITOR_H_ */
