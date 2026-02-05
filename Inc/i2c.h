/*
 * i2c.h
 *
 *  Created on: Feb 4, 2026
 *      Author: John Moyer
 */

#ifndef I2C_H_
#define I2C_H_

void initI2C();

void i2c_start();

int i2c_write_byte(uint8_t byte);

void i2c_stop();

#endif /* I2C_H_ */
