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

## Circuit Design

### MOSFET Selection
- **Chosen:** IRLZ44N (logic-level N-channel MOSFET)
- **Rationale:** 
  - Logic-level (Vgs(th) < 3.3V) for direct GPIO control
  - 47A rating provides large safety margin for 150mA load
  - Low Rds(on) minimizes power dissipation
  - TO-220 package for easy breadboarding

### Gate Drive Network
- **100Ω series resistor:** Limits inrush current to gate capacitance
- **10kΩ pull-down:** Ensures gate discharges to 0V when GPIO is LOW,
  preventing floating gate and partial conduction

### I2C Pull-up Resistors
- **4.7kΩ pull-ups** on SDA/SCL (typically included on modules)
- Calculated for 100kHz standard-mode I2C:
  - tr(max) = 1000ns
  - Rp(min) = tr / (0.8473 × Cb) for typical bus capacitance

## Critical Timing Requirements

### 1-Wire Protocol (DS18B20)
Based on datasheet specifications:

| Operation | Min | Typical | Max | Implementation |
|-----------|-----|---------|-----|----------------|
| Reset pulse | 480μs | - | - | 500μs (margin) |
| Presence detect | - | - | 60μs | 80μs wait |
| Write 1 slot | 1μs | - | 15μs | 6μs LOW |
| Write 0 slot | 60μs | - | 120μs | 60μs LOW |
| Read slot | 1μs | - | 15μs | 2μs LOW |
| Sample time | - | ~15μs | - | 12μs after start |

### I2C Timing (100kHz Standard Mode)
- SCL frequency: 100kHz (10μs period)
- Setup time: 250ns minimum
- Hold time: 0ns minimum (extended in hardware)
```

---

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
