//main.c
#include <stdint.h>
#include "i2c.h"
#include "lcd.h"
#include "delay.h"
#include "ds18b20.h"
#include "lights.h"
#include "regaddr.h"


void initLed(void);
void toggleLed(void);


#define LCD_BACKLIGHT 0x08
#define LCD_EN        0x04
#define LCD_RS        0x01

static void u32_to_str(uint32_t value, char *buffer)
{
    int i = 0;
    char temp[10];

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    // reverse
    for (int j = 0; j < i; j++) {
        buffer[j] = temp[i - j - 1];
    }

    buffer[i] = '\0';
}
static void float_to_string(float value, char* buffer) {
    int whole = (int)value;
    int decimal = (int)((value - whole) * 10);

    if(whole < 0) {
        *buffer++ = '-';
        whole = -whole;
    }

    if(whole >= 10) {
        *buffer++ = '0' + (whole / 10);
    }
    *buffer++ = '0' + (whole % 10);
    *buffer++ = '.';
    *buffer++ = '0' + decimal;
    *buffer = '\0';
}

//int main(void) {
//    initLed();
//    ds18b20_init();
//    initI2C();
//    lcd_init();
//    led_control_init();
//
//    initLed();
//    led_control_init();
//
//    for(;;) {
//        led_lights_on();
//        delayStkBusyMS(3000);
//
//        led_lights_off();
//        delayStkBusyMS(3000);
//    }



//    lcd_clear();
//    lcd_set_cursor(0, 0);
//    lcd_print("Sensor online");
//    delayStkBusyMS(1000);
//    lcd_clear();
//
//    for(;;) {
//        // Start conversion
//        ds18b20_start_conversion();
//
//        // Read temperature
//        float temp = ds18b20_read_temperature();
//
//        // Check for error
//        if(temp == -999.0) {
//        	lcd_clear();
//        	lcd_print("Read error!");
//        	for(;;);
//        }
//		// Display temperature
//		lcd_set_cursor(0, 0);
//		lcd_print("Temp: ");
//
//		int whole = (int)temp;
//		int decimal = (int)((temp - whole) * 10);
//
//		if(temp < 0) {
//			lcd_data('-');
//			whole = -whole;
//			decimal = -decimal;
//		}
//
//		lcd_data('0' + (whole / 10));
//		lcd_data('0' + (whole % 10));
//		lcd_data('.');
//		lcd_data('0' + (decimal % 10));
//		lcd_print(" C");
//		delayStkBusyMS(1000);
//	}
//}

int main(void) {
    initI2C();
    lcd_init();
    led_control_init();
    initLed();

    lcd_clear();
    lcd_print("Starting test");
    delayStkBusyMS(2000);

    while(1) {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Lights ON");
        led_lights_on();

        // Check if PA1 is actually HIGH
        volatile uint32_t* gpioa_idr = (uint32_t*)(GPIOA_BASE + 0x08);
        uint8_t pa1_state = (*gpioa_idr >> 1) & 0x1;

        lcd_set_cursor(1, 0);
        lcd_print("PA1:");
        lcd_data('0' + pa1_state);
        delayBusyMS(3000);
    }
}
