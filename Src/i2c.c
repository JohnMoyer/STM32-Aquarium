/*
 * i2c.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "i2c.h"

#define I2C1_BASE 0x40005400
//I2C1 enable bit 21
#define RCC_BASE 0x40021000
#define GPIOB_BASE 0x40010C00

#define GPIOB_CRL (GPIOB_BASE + 0x00)

#define RCC_APB1ENR (RCC_BASE + 0x1C)
#define RCC_APB2ENR (RCC_BASE + 0x18)
//GPIOB EN bit 3
#define I2C_CR1 I2C1_BASE
#define I2C_CR2 (I2C1_BASE + 0x04)
#define I2C_CCR (I2C1_BASE + 0x1C)
#define I2C_TRISE (I2C1_BASE + 0x20)
#define I2C_DR (I2C1_BASE + 0x10)
#define I2C_SR1 (I2C1_BASE + 0x14)
#define I2C_SR2 (I2C1_BASE + 0x18)

#define PB67_MASK (0b11111111 << 24)


void initI2C() {
	volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
	volatile uint32_t* rcc_apb1enr = (uint32_t*) RCC_APB1ENR;

	//enable gpiob
	*rcc_apb2enr |= (0b1 << 3);
	//enable i2c
	*rcc_apb1enr |= (0b1 << 21);

	volatile uint32_t* gpiob_crl = (uint32_t*) GPIOB_CRL;
	*gpiob_crl &= ~(0xFF << 24);    // Clear PB6/PB7
	*gpiob_crl |=  (0xFF << 24);    // Set PB6/PB7 to AF open-drain 50 MHz

	volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
	//Reset I2C control register 1
	*i2c_cr1 &= ~(0b1 << 15);
	*i2c_cr1 |= (0b1 << 15);


	volatile uint32_t* i2c_cr2 = (uint32_t*) I2C_CR2;
	//8 MHz
	*i2c_cr2 = 8;

	volatile uint32_t* i2c_ccr = (uint32_t*) I2C_CCR;
	*i2c_ccr = 40;
	volatile uint32_t* i2c_trise = (uint32_t*) I2C_TRISE;
	*i2c_trise = 9;




	//Enable peripherals for i2c cr1. Clock control must be done while off
	*i2c_cr1 |= (0b1 << 0);
}

static void i2c_wait_flag(uint32_t flag) {
    volatile uint32_t* i2c_sr1 = (uint32_t*)(I2C1_BASE + 0x14);
    while(!(*i2c_sr1 & flag));
}

// Generate START condition
void i2c_start() {
    volatile uint32_t* i2c_cr1 = (uint32_t*)(I2C1_BASE + 0x00);

    *i2c_cr1 |= (1 << 8);        // START
    i2c_wait_flag(1 << 0);       // Wait for SB (start bit)
}

// Send a byte on the I2C bus
int i2c_write_byte(uint8_t byte) {
    volatile uint32_t* i2c_dr  = (uint32_t*)(I2C1_BASE + 0x10);
    volatile uint32_t* i2c_sr1 = (uint32_t*)(I2C1_BASE + 0x14);

    *i2c_dr = byte;             // Put byte in data register
    i2c_wait_flag(1 << 7);      // Wait for BTF (byte transfer finished)

    // Optionally, check for NACK
    return (*i2c_sr1 & (1 << 10)) ? 1 : 0; // AF = acknowledge failure
}

// Generate STOP condition
void i2c_stop() {
    volatile uint32_t* i2c_cr1 = (uint32_t*)(I2C1_BASE + 0x00);
    *i2c_cr1 |= (1 << 9);  // STOP
}
