#include "i2c.h"
/***************** defines go here ************************/

#define IsSdaHi(i2c)		(digitalRead(i2c->sda_pin)==GPIO_PIN_SET)
#define IsSdaLo(i2c)		(digitalRead(i2c->sda_pin)==GPIO_PIN_RESET)
#define IsSclHi(i2c)		(digitalRead(i2c->scl_pin)==GPIO_PIN_SET)
#define IsSclLo(i2c)		(digitalRead(i2c->scl_pin)==GPIO_PIN_RESET)

/***************** local functions go here ************************/
static void i2c_mon_sda(struct i2c_monitor_t *i2c, uint8_t pin);
static void i2c_mon_scl(struct i2c_monitor_t *i2c, uint8_t pin);

void i2c_log(struct i2c_monitor_t *i2c) {
    static uint8_t buffer[512];
    uint16_t n=0;
    int i;

    for (i = 0; i < i2c->i2c_buffer_idx; i++) {
        n += sprintf ((char *)&buffer[n], "%02X ", i2c->i2c_buffer[i]);
    }
    buffer[n++]	=	'\r';
    buffer[n]  	=	'\n';
    printf("%s\n", buffer);
}

/*
 * i2c monitor callback:
 *    monitor sda and scl when interrupt occurs
 */
static void i2c_monitor_callback(struct i2c_monitor_t *i2c, uint8_t GPIO_Pin){
    if ((i2c->sda_pin == GPIO_Pin) || (i2c->scl_pin == GPIO_Pin) ) {
	switch(i2c->i2c_state) {
	    case I2C_STATUS_IDLE:
		if(GPIO_Pin==i2c->sda_pin && IsSdaLo(i2c) && IsSclHi(i2c)) { // start condition when sda hi->lo
		    //if previous is not complete then it's an error
		    if(i2c->i2c_done) {
		        //should inform user i2c error detected.
			i2c->i2c_done=0;
	   	    }
			//continue decoding the i2c.
		    {
			//printf("i2c start\n");
			i2c->i2c_state = I2C_STATUS_START;
			i2c->i2c_bit_count = 0;
			i2c->i2c_data_bit = 0;
			i2c->i2c_done = 1;
	  	    }
		}
		break;
	    case I2C_STATUS_START:
	    case I2C_STATUS_RESTART:
	    case I2C_STATUS_DATA:
		if(GPIO_Pin==i2c->scl_pin)	{ // if scl rising edge detect then log the data.
		    if(i2c->i2c_bit_count >= 8) {
                        //printf("i2c done: %02X\n", i2c->i2c_data_bit);
		        i2c->i2c_buffer[i2c->i2c_buffer_idx++] = i2c->i2c_data_bit;
		        i2c->i2c_bit_count = 0;
		        i2c->i2c_data_bit = 0;
		    }
		    else {
		        i2c->i2c_data_bit = (i2c->i2c_data_bit << 1) + (IsSdaHi(i2c) ? 1 : 0);
		        i2c->i2c_bit_count++;
                        //printf("bit: %02X %d\n", i2c->i2c_data_bit, i2c->i2c_bit_count);
		    }
	        }
	        // if sda is 0 and scl is 1 //then restarted condition detected
	        else if(GPIO_Pin == i2c->sda_pin && IsSdaLo(i2c) && IsSclHi(i2c))  {
                    //printf("i2c repeated\n");
		    i2c->i2c_state = I2C_STATUS_RESTART;
		    i2c->i2c_bit_count 	= 0;
		    i2c->i2c_data_bit 	= 0;
		}
	        // if sda is hi and scl is lo then stopped condition detected
	        else if((GPIO_Pin == i2c->sda_pin) && IsSclHi(i2c) && IsSdaHi(i2c))	{
                    //printf("i2c stop: %d\n", i2c->i2c_buffer_idx);
		    //for demo purpose, just print from here.
	 	    //Next step is to send from other tasks
		    i2c_log(i2c);
		    i2c->i2c_done = 0;
		    i2c->i2c_state 	= I2C_STATUS_IDLE;
		    i2c->i2c_bit_count	= 0;
		    i2c->i2c_buffer_idx = 0;
		}
		break;
	}
    }
}

/* create an instance of i2c */
void i2c_mon_create(struct i2c_monitor_t *i2c) {

	i2c->sda_pin_set 			= i2c_mon_sda;
	i2c->scl_pin_set 			= i2c_mon_scl;
	i2c->i2c_mon_cb				= i2c_monitor_callback;
	i2c->i2c_buffer_idx			= 0;

	i2c->i2c_state				= I2C_STATUS_IDLE;
	i2c->i2c_bit_count			= 0;
	i2c->i2c_data_bit			= 0;
	i2c->i2c_done				= 0;
}

/* user sda pin configure */
static void i2c_mon_sda(struct i2c_monitor_t *i2c, uint8_t pin) {
	i2c->sda_pin = pin;
}

/* user scl pin configure */
static void i2c_mon_scl(struct i2c_monitor_t *i2c, uint8_t pin) {
	i2c->scl_pin = pin;
}
