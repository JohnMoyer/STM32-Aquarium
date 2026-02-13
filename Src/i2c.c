/*
 * i2c.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "i2c.h"

#define I2C1_BASE 		0x40005400
#define RCC_BASE 		0x40021000
#define GPIOB_BASE 		0x40010C00

#define GPIOB_CRL 		(GPIOB_BASE + 0x00)
#define RCC_APB1ENR 	(RCC_BASE + 0x1C)
#define RCC_APB2ENR 	(RCC_BASE + 0x18)

#define I2C_CR1 		(I2C1_BASE + 0x00)
#define I2C_CR2 		(I2C1_BASE + 0x04)
#define I2C_CCR 		(I2C1_BASE + 0x1C)
#define I2C_TRISE 		(I2C1_BASE + 0x20)
#define I2C_DR 			(I2C1_BASE + 0x10)
#define I2C_SR1 		(I2C1_BASE + 0x14)
#define I2C_SR2			(I2C1_BASE + 0x18)

extern void toggleLed(void);
extern void delayBusyMS(uint32_t N);

void initI2C() {
    volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
    volatile uint32_t* rcc_apb1enr = (uint32_t*) RCC_APB1ENR;

    *rcc_apb2enr |= (1 << 3);   // Enable GPIOB
    *rcc_apb1enr |= (1 << 21);  // Enable I2C1

    // Configure PB6/PB7 AF open-drain 50MHz
    volatile uint32_t* gpiob_crl = (uint32_t*) GPIOB_CRL;
    *gpiob_crl &= ~(0xFF << 24);
    *gpiob_crl |= (0xFF << 24);

    volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;

    // Disable I2C before config
    *i2c_cr1 &= ~(1 << 0);

    // Software reset
    *i2c_cr1 |= (1 << 15);
    for(volatile int i = 0; i < 1000; i++);
    *i2c_cr1 &= ~(1 << 15);

    // Set peripheral clock frequency 36 MHz
    volatile uint32_t* i2c_cr2 = (uint32_t*) I2C_CR2;
    *i2c_cr2 = 36;

    // CCR for 100kHz with 36MHz clock: 36MHz / (2 * 100kHz) = 180
    volatile uint32_t* i2c_ccr = (uint32_t*) I2C_CCR;
    *i2c_ccr = 180;

    // TRISE: (36MHz * 1000ns / 1000000000) + 1 = 37
    volatile uint32_t* i2c_trise = (uint32_t*) I2C_TRISE;
    *i2c_trise = 37;

    // Enable I2C
    *i2c_cr1 |= (1 << 0);
}

static int i2c_wait_flag_timeout(volatile uint32_t* reg, uint32_t flag, uint32_t timeout) {
    while(!((*reg) & flag)) {
        if(timeout-- == 0) {
            toggleLed();  // Blink LED on timeout
            delayBusyMS(50);
            return -1;  // Timeout
        }
    }
    return 0;  // Success
}

void i2c_start() {
    volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    //Start bit
    *i2c_cr1 |= (1 << 8);
    //Wait for start bit
    i2c_wait_flag_timeout(i2c_sr1, (1 << 0), 100000);
}

//Param address: i2c device address (0x27 for lcd)
//Param direction: 0: w, 1: r
int i2c_send_address(uint8_t address, uint8_t direction) {
    volatile uint32_t* i2c_dr = (uint32_t*) I2C_DR;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;
    volatile uint32_t* i2c_sr2 = (uint32_t*) I2C_SR2;

    //Data register set to address, set bit 0 to direction
    *i2c_dr = (address << 1) | direction;

    int result = i2c_wait_flag_timeout(i2c_sr1, (0b1 << 1), 100000);
    if(result < 0) return -1;  // Timeout or NACK

    // Clear ADDR
    (void)*i2c_sr1;
    (void)*i2c_sr2;

    // Check for Acknowledgment failure
    if(*i2c_sr1 & (1 << 10)) return -1;  // AF bit set = NACK

    return 0;  // Success
}

void i2c_write_data(uint8_t data) {
    volatile uint32_t* i2c_dr = (uint32_t*) I2C_DR;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    i2c_wait_flag_timeout(i2c_sr1, (1 << 7), 100000);  // Wait for TxE
    *i2c_dr = data;
}

void i2c_stop() {
    volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    i2c_wait_flag_timeout(i2c_sr1, (1 << 2), 100000);  // Wait for BTF
    *i2c_cr1 |= (1 << 9);  // STOP
}
