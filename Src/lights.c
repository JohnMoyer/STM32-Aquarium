/*
 * lights.c
 *
 *  Created on: Feb 17, 2026
 *      Author: John Moyer
 */

#include "regaddr.h"
#include "lights.h"

#define GPIOA ((gpio_t*) GPIOA_BASE)

void led_control_init() {
    volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
    *rcc_apb2enr |= (0b1 << 2);

    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |= (0b0010 << 4);

    led_lights_off();

}

void led_lights_on() {
	GPIOA->ODR |= (0b1 << 1);
}

void led_lights_off() {
	GPIOA->ODR &= ~(0b1 << 1);
}

