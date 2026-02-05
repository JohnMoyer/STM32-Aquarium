/*
 * delay.c
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#include <stdint.h>

#define DELAY_MS 555

void delayBusyMS(uint32_t N) {
	for(uint32_t i=0; i<N*DELAY_MS; i++);
}
