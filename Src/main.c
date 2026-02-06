//main.c
#include <stdint.h>
#include "i2c.h"
#include "lcd.h"

#define GPIOC_BASE 0x40011000
#define GPIOC_ODR  (GPIOC_BASE + 0x0C)

void initLed(void);
void delayBusyMS(uint32_t N);
void toggleLed(void);


#define LCD_BACKLIGHT 0x08
#define LCD_EN        0x04
#define LCD_RS        0x01


// i2c_write_byte and i2c_start/stop are from your i2c.c
int main(void)
{
    initLed();
    initI2C();
    lcd_init();

    lcd_clear();
    lcd_set_cursor(0, 0);  // Row 0, Col 0
    lcd_print("Temp here");

    lcd_set_cursor(1,0);
    lcd_print("Time here");

    while(1) {
        toggleLed();
        delayBusyMS(1000);
    }
}

