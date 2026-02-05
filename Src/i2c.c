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

#define GPIOB_CRL 	(GPIOB_BASE + 0x00)

#define RCC_APB1ENR (RCC_BASE + 0x1C)
#define RCC_APB2ENR (RCC_BASE + 0x18)
//GPIOB EN bit 3
#define I2C_CR1 	(I2C1_BASE + 0x00)
#define I2C_CR2 	(I2C1_BASE + 0x04)
#define I2C_CCR 	(I2C1_BASE + 0x1C)
#define I2C_TRISE 	(I2C1_BASE + 0x20)
#define I2C_DR 		(I2C1_BASE + 0x10)
#define I2C_SR1 	(I2C1_BASE + 0x14)
#define I2C_SR2 	(I2C1_BASE + 0x18)

#define PB67_MASK 	(0b11111111 << 24)


void initI2C() {
	volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
	volatile uint32_t* rcc_apb1enr = (uint32_t*) RCC_APB1ENR;

	//enable gpiob
	*rcc_apb2enr |= (0b1 << 3);
	//enable i2c
	*rcc_apb1enr |= (0b1 << 21);

	volatile uint32_t* gpiob_crl = (uint32_t*) GPIOB_CRL;
	*gpiob_crl &= ~PB67_MASK;    // Clear PB6/PB7
	*gpiob_crl |=  PB67_MASK;    // Set PB6/PB7 to AF open-drain 50 MHz

	volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
	//Reset I2C control register 1
	*i2c_cr1 |= (0b1 << 15);   // Set SWRST
	for(volatile int i = 0; i < 100; i++);  // Small delay
	*i2c_cr1 &= ~(0b1 << 15);  // Clear SWRST


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

static void i2c_wait_flag(volatile uint32_t* reg, uint32_t flag) {
    while(!(*reg & flag));
}

void i2c_start() {
    volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    *i2c_cr1 |= (1 << 8);  // START
    i2c_wait_flag(i2c_sr1, (1 << 0));  // Wait for SB
}

int i2c_send_address(uint8_t address, uint8_t direction) {
    volatile uint32_t* i2c_dr = (uint32_t*) I2C_DR;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;
    volatile uint32_t* i2c_sr2 = (uint32_t*) I2C_SR2;

    *i2c_dr = (address << 1) | direction;  // Send address with R/W bit
    i2c_wait_flag(i2c_sr1, (1 << 1));      // Wait for ADDR

    // Clear ADDR flag by reading SR1 then SR2
    (void)*i2c_sr1;
    (void)*i2c_sr2;

    return 0;  // Success
}

void i2c_write_data(uint8_t data) {
    volatile uint32_t* i2c_dr = (uint32_t*) I2C_DR;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    i2c_wait_flag(i2c_sr1, (1 << 7));  // Wait for TxE
    *i2c_dr = data;
}

void i2c_stop() {
    volatile uint32_t* i2c_cr1 = (uint32_t*) I2C_CR1;
    volatile uint32_t* i2c_sr1 = (uint32_t*) I2C_SR1;

    i2c_wait_flag(i2c_sr1, (1 << 2));  // Wait for BTF
    *i2c_cr1 |= (1 << 9);  // STOP
}
