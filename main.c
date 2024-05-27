
#include <xc.h>
#include "LCD.h"
#include "bme68x.h"
#include "pic18_i2c.h"

//RD5 = SDA
//RD6 = SCL
//pull-ups on board
//Connect ground of PIC board to ground of measured power source.

struct bme68x_dev bme;
uint8_t dev_addr;

void interfaceInit(void) {
    pic18_i2c_init();
    dev_addr = BME68X_I2C_ADDR_LOW;
    bme.read = pic18_i2c_read;
    bme.write = pic18_i2c_write;
    bme.intf = BME68X_I2C_INTF;
    bme.delay_us = pic18_delay_us;
    bme.intf_ptr = &dev_addr;
    bme.amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
}

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    LCDInit();
    lprintf(0, "BME680");
    bme68x_init(&bme);
    while (1) {
        __delay_ms(200);
    }
}


