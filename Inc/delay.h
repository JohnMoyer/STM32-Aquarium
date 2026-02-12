/*
 * delay.h
 *
 *  Created on: Feb 11, 2026
 *      Author: John Moyer
 */

#ifndef DELAY_H_
#define DELAY_H_

#define DELAY_MS 555

#define STK_BASE 0xE000E010

typedef struct {
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
}stk_t;

#define STK ((stk_t*) STK_BASE)

//72MHz

void delayBusyMS(uint32_t N);

void delayStkBusyUS(uint32_t N);

void delayStkBusyMS(uint32_t N);


#endif /* DELAY_H_ */
