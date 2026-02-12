/*
 	1. Send: RESET pulse
	2. Sensor responds: PRESENCE pulse (just says "I'm here")
	3. Send: 0xCC byte (8 bits sent one at a time as HIGH/LOW pulses)
	4. Send: 0x44 byte (command to start temperature conversion)
	5. Wait 750ms while sensor measures temperature
	6. Send: RESET pulse
	7. Send: 0xCC byte
	8. Send: 0xBE byte (command to read temperature)
	9. Sensor sends back: 9 bytes of data (72 bits total!)
   	   - Each bit is transmitted as a timed HIGH or LOW pulse
   	   - Read each bit using IDR
   	   - Combine bits into bytes
   	   - Bytes 0-1 contain the temperature
 */

#include "regaddr.h"
#include "delay.h"
#include "i2c.h"
#include "lcd.h"
#include <stdint.h>

#define GPIOB ((gpio_t*) GPIOB_BASE)

void ds_pin_high(void) {
    GPIOB->ODR |= (0b1 << 12);
}

void ds_pin_low(void) {
    GPIOB->ODR &= ~(0b1 << 12);
}

uint8_t ds_pin_read(void) {
    return ((GPIOB->IDR >> 12) & 0b1);
}

void ds18b20_init(void) {
    volatile uint32_t* rcc_apb2enr = (uint32_t*) RCC_APB2ENR;
    *rcc_apb2enr |= (0b1 << 3);

    GPIOB->CRH &= ~(0b1111 << 16);
    GPIOB->CRH |= (0b0111 << 16);

    ds_pin_high();
    delayStkBusyMS(10);  // Let it stabilize
}

uint32_t ds18b20_reset(void) {
    uint8_t presence;

    ds_pin_low();
    delayStkBusyUS(480);  // Reset pulse (min 480μs)

    ds_pin_high();
    delayStkBusyUS(70);   // Wait for presence pulse

    presence = !ds_pin_read();  // Sensor pulls LOW if present

    delayStkBusyUS(410);  // Complete the reset cycle

    return presence;
}

void ds18b20_write_bit(uint8_t bit) {
    if (bit) {
        // Write 1
        ds_pin_low();
        delayStkBusyUS(6);   // Short LOW pulse
        ds_pin_high();
        delayStkBusyUS(64);   // Rest of time slot
    } else {
        // Write 0
        ds_pin_low();
        delayStkBusyUS(60);   // Long LOW pulse
        ds_pin_high();
        delayStkBusyUS(10);    // Recovery
    }
}

uint8_t ds18b20_read_bit(void) {
    uint8_t bit = 0;

    ds_pin_low();
    delayStkBusyUS(1);    // Initiate read slot

    ds_pin_high();
    delayStkBusyUS(10);   // Wait for sensor to drive line

    bit = ds_pin_read();  // Sample the line

    delayStkBusyUS(49);   // Complete the time slot (total 60μs)

    return bit;
}

void ds18b20_write_byte(uint8_t byte) {
    for(uint8_t i = 0; i < 8; i++) {
        ds18b20_write_bit((byte >> i) & 0x01);
    }
}

uint8_t ds18b20_read_byte(void) {
    uint8_t byte = 0;

    for(uint8_t i = 0; i < 8; i++) {
        if(ds18b20_read_bit()) {
            byte |= (1 << i);
        }
    }

    delayStkBusyUS(10);

    return byte;
}

void ds18b20_start_conversion(void) {
    if(!ds18b20_reset()) {
        return;  // No sensor present
    }

    ds18b20_write_byte(0xCC);  // Skip ROM
    ds18b20_write_byte(0x44);  // Convert T

    delayStkBusyMS(750);  // Wait for conversion
}

float ds18b20_read_temperature(void) {
    if(!ds18b20_reset()) {
        return -999.0;  // Error: sensor not found
    }

    ds18b20_write_byte(0xCC);  // Skip ROM
    ds18b20_write_byte(0xBE);  // Read Scratchpad

    uint8_t temp_lsb = ds18b20_read_byte();
    uint8_t temp_msb = ds18b20_read_byte();

    int16_t raw_temp = (temp_msb << 8) | temp_lsb;

    float temp = raw_temp / 16.0;
    return temp;
}
