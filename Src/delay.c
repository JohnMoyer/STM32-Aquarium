/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>
#include "delay.h"


//72MHz

void delayBusyMS(uint32_t N) {
	for(uint32_t i=0; i<N*DELAY_MS; i++);
}

void delayStkBusyUS(uint32_t N) {
    uint32_t reload = (8 * N) - 1;
    if (reload > 0xFFFFFF) return;

    STK->CTRL = 0;
    STK->LOAD = reload;
    STK->VAL = 0;
    STK->CTRL = 0b101;  // Full processor clock (72 MHz)

    while(!(STK->CTRL & (1 << 16)));

    STK->CTRL = 0;
}

void delayStkBusyMS(uint32_t N) {
    // Max safe delay per chunk: 16777215 / 72000 = ~233 ms
    while (N > 0) {
        uint32_t chunk = (N > 2097) ? 2097 : N;
        uint32_t reload = (8000 * chunk) - 1;

        STK->CTRL = 0;
        STK->LOAD = reload;
        STK->VAL = 0;
        STK->CTRL = 0b101;  // Full processor clock

        while(!(STK->CTRL & (1 << 16)));

        STK->CTRL = 0;

        N -= chunk;
    }
}
