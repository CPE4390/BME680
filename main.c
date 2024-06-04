
#include <xc.h>
#include "LCD.h"
#include "bme68x.h"
#include "pic18_i2c.h"

//SDA -> RD5
//SCL -> RD6
//CS -> 3.3V
//SDO -> GND
//VCC -> 3.3V
//GND -> GND
//pull-ups on board

struct bme68x_dev bme;
uint8_t dev_addr;
struct bme68x_conf conf;
struct bme68x_heatr_conf heatr_conf;
struct bme68x_data data;
uint32_t del_period;
uint8_t n_fields;

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    LCDInit();
    lprintf(0, "BME680");
    __delay_ms(2000);
    pic18_i2c_init();
    dev_addr = BME68X_I2C_ADDR_LOW;
    bme.read = pic18_i2c_read;
    bme.write = pic18_i2c_write;
    bme.intf = BME68X_I2C_INTF;
    bme.delay_us = pic18_delay_us;
    bme.intf_ptr = &dev_addr;
    bme.amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
    int8_t rslt;
    rslt = bme68x_init(&bme);
    if (rslt != BME68X_OK) {
        lprintf(1, "Init failed %d", rslt);
    } else {
        lprintf(1, "Chip ID %02x", bme.chip_id);
    }
    //while (1); //TODO - Remove once init is tested
    conf.filter = BME68X_FILTER_OFF;
    conf.odr = BME68X_ODR_NONE;
    conf.os_hum = BME68X_OS_16X;
    conf.os_pres = BME68X_OS_1X;
    conf.os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(&conf, &bme);
    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp = 300;
    heatr_conf.heatr_dur = 100;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme);

    while (1) {
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
        del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme) + (heatr_conf.heatr_dur * 1000);
        bme.delay_us(del_period, bme.intf_ptr);
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme);
        if (n_fields) {
#ifdef BME68X_USE_FPU
            lprintf(0, "%.2f, %.2f", data.temperature, data.pressure);
            lprintf(1, "%.2f,%.0f", data.humidity, data.gas_resistance);
#else
            lprintf(0, "%d, %lu", (data.temperature / 100),
                    (long unsigned int) data.pressure);
            lprintf(1, "%lu, %lu", (long unsigned int) (data.humidity / 1000), 
                    (long unsigned int) data.gas_resistance);
#endif
        }
    }
}


