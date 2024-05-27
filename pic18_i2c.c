#include "pic18_i2c.h"
#include "xc.h"

BME68X_INTF_RET_TYPE pic18_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t slave_addr = *(uint8_t *)intf_ptr;
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN == 1);
    SSP2BUF = (uint8_t)(slave_addr << 1);
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    if (SSP2CON2bits.ACKSTAT == 1) {
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN == 1);
        return -1;
    }
    SSP2BUF = reg_addr;
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2CON2bits.RSEN = 1;
    while (SSP2CON2bits.RSEN == 1);
    SSP2BUF = (uint8_t)((slave_addr << 1) + 1); //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    while (len > 0) {
        SSP2CON2bits.RCEN = 1;
        while (!SSP2STATbits.BF);
        *reg_data = SSP2BUF;
        if (len > 1) {
            SSP2CON2bits.ACKDT = 0;
        } else {
            SSP2CON2bits.ACKDT = 1;
        }
        SSP2CON2bits.ACKEN = 1;
        while (SSP2CON2bits.ACKEN != 0);
        --len;
        ++reg_data;
    }
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN == 1);
    return BME68X_INTF_RET_SUCCESS;
}

BME68X_INTF_RET_TYPE pic18_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
    uint8_t slave_addr = *(uint8_t *)intf_ptr;
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN == 1);
    SSP2BUF = (uint8_t)(slave_addr << 1);
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    if (SSP2CON2bits.ACKSTAT == 1) {
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN == 1);
        return -1;
    }
    SSP2BUF = reg_addr;
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    while (len > 0) {
        SSP2BUF = *reg_data;
        while (SSP2STATbits.BF || SSP2STATbits.R_W);
        --len;
        ++reg_data;
    }
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN == 1);
    return BME68X_INTF_RET_SUCCESS;
}

#define _XTAL_FREQ  32000000

void pic18_delay_us(uint32_t period, void *intf_ptr) {
    while (period > 0) {
        __delay_us(1);
        --period;
    }
}

void pic18_i2c_init(void) {
    TRISD |= 0b01100000; //MMSP2 uses RD5 as SDA, RD6 as SCL, both set as inputs
    SSP2ADD = 19; //400kHz
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
}
