
#ifndef PIC18_I2C_H
#define	PIC18_I2C_H

#include "bme68x.h"

BME68X_INTF_RET_TYPE pic18_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
BME68X_INTF_RET_TYPE pic18_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void pic18_delay_us(uint32_t period, void *intf_ptr);
void pic18_i2c_init(void);

#endif	/* PIC18_I2C_H */

