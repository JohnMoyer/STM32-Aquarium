/*
 * ds18b20.h
 *
 *  Created on: Feb 11, 2026
 *      Author: John Moyer
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdint.h>

void ds_pin_high(void);

void ds_pin_low(void);

uint8_t ds_pin_read(void);

void ds18b20_init(void);

uint32_t ds18b20_reset(void);

void ds18b20_write_bit(uint8_t bit);

uint8_t ds18b20_read_bit(void);

void ds18b20_write_byte(uint8_t byte);

uint8_t ds18b20_read_byte(void);

void ds18b20_start_conversion(void);

float ds18b20_read_temperature(void);



#endif /* DS18B20_H_ */
