/*
 * ledtest.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>

#define RCC_BASE 0x40021000
#define GPIOC_BASE 0x40011000

#define RCC_APB2ENR (RCC_BASE + 0x18)
#define GPIOC_CRH 	(GPIOC_BASE + 0x04)
#define GPIOC_ODR  (GPIOC_BASE + 0x0C)

void initLed() {
    volatile uint32_t* rcc = (uint32_t*) RCC_APB2ENR;
    uint8_t rccMask = 0b1 << 4;
    *rcc |= rccMask;

    volatile uint32_t* crh = (uint32_t*) GPIOC_CRH;
    *crh &= ~(0b1111 << 20);  // Clear all 4 bits (CNF13[1:0] + MODE13[1:0])
    *crh |= (0b0010 << 20);   // Set MODE13=0b10 (2MHz), CNF13=0b00 (push-pull)

	volatile uint32_t* odr = (uint32_t*) GPIOC_ODR;
	*odr &= ~(0b1 << 13);
}

//PC13 is board led

void toggleLed() {
	volatile uint32_t* odr = (uint32_t*) GPIOC_ODR;
	*odr ^= 0b1 << 13;
}
