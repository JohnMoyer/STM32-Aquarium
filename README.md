# STM32 Bare-Metal Aquarium Controller

A fully custom embedded system for automated fish tank management, built from scratch without HAL libraries.

## Features
- **Automated Lighting Control**: MOSFET-based 12V LED switching with scheduled on/off times
- **Temperature Monitoring**: Real-time water temperature reading via DS18B20 sensor
- **LCD Display**: Live display of time and temperature on I2C LCD1602
- **RTC Integration**: DS3231 real-time clock for persistent timekeeping

## Technical Implementation

### Hardware
- **Microcontroller**: STM32F103C8T6 (ARM Cortex-M3, 72MHz)
- **Sensors**: DS18B20 waterproof temperature probe
- **Display**: LCD1602 with I2C backpack (PCF8574)
- **Power Control**: IRLZ44N logic-level N-channel MOSFET
- **Timekeeping**: DS3231 RTC module with battery backup

### Software Architecture
**Bare-metal C implementation** with custom peripheral drivers:

#### Communication Protocols
- **I2C (Hardware)**: Multi-device bus for LCD and RTC communication
  - Implemented master mode with proper start/stop conditions
  - Address arbitration and ACK/NACK handling
  
- **1-Wire Protocol (Software)**: Bit-banged communication for DS18B20
  - Microsecond-precision timing using SysTick
  - Reset/presence detection, bit read/write operations

#### Key Technical Challenges Solved
1. **Precise Timing**: Implemented SysTick-based microsecond delays for 1-Wire protocol (critical for reliable sensor communication)
2. **MOSFET Switching**: Designed low-side switching circuit with proper gate pull-down to eliminate floating gate issues
3. **Multi-device I2C**: Managed shared I2C bus with proper addressing and timing
4. **Memory-Mapped I/O**: Direct register manipulation without abstraction layers

### Code Structure
```
/Core
  /Src
    main.c           # Main control loop
    i2c.c            # Hardware I2C driver
    lcd.c            # LCD1602 display driver
    ds18b20.c        # 1-Wire temperature sensor driver
    ds3231.c         # RTC driver
    lights.c         # MOSFET control
    delay.c          # SysTick-based timing
```

## Skills Demonstrated
- **Low-level embedded programming**: Direct hardware register manipulation
- **Communication protocols**: I2C, 1-Wire, GPIO
- **Real-time systems**: Interrupt handling, precise timing
- **Circuit design**: MOSFET switching, pull-up/pull-down resistors
- **Debugging**: Logic analyzer, multimeter, oscilloscope techniques
- **Version control**: Git for embedded systems development


## Future Enhancements
- PWM-based LED dimming
- pH sensor integration
- Remote monitoring via UART/Bluetooth
- Feeding schedule automation

## Resources
- [STM32F103 Reference Manual](link)
- [DS18B20 Datasheet](link)
- [I2C Protocol Specification](link)
```
