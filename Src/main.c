//main.c
#include <stdint.h>
#include "i2c.h"

#define GPIOC_BASE 0x40011000
#define GPIOC_ODR  (GPIOC_BASE + 0x0C)

void initLed(void);
void delayBusyMS(uint32_t N);
void toggleLed(void);


#define LCD_BACKLIGHT 0x08
#define LCD_EN        0x04
#define LCD_RS        0x01


// i2c_write_byte and i2c_start/stop are from your i2c.c

int main() {
    initI2C();
    initLed();
    uint32_t address = 0;

    for(uint8_t addr = 0x20; addr < 0x40; addr++) {  // common LCD addresses
        i2c_start();
        int nack = i2c_write_byte(addr << 1);        // try writing to address
        i2c_stop();

        if(nack == 0) {
            // Device ACKed → this is the address!
        	if (addr != 0) {
                toggleLed();
                address = addr;
        	}
            break;
        }
        delayBusyMS(500); // short pause between addresses
    }

    i2c_start();
    i2c_write_byte(address << 1); // write mode
    i2c_stop();
}
