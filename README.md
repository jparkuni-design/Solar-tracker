# Solar Panel Tracker

Dual-axis solar tracking system built on an Arduino Nano — developed as an electrical engineering co-op portfolio project (University of Waterloo).

The panel actively follows the sun across two axes using four light sensors, while an onboard display shows live environmental and power data. It's a hands-on hardware + embedded software project combining sensor input, closed-loop actuation, and real-time monitoring.

## Table of Contents
- [Functional Components](#functional-components)
- [Support / Usage Components](#support--usage-components)
- [Setup & Usage](#setup--usage)
- [Conclusion](#conclusion)

## Functional Components

### 1. 20x4 I2C LCD (PCF8574 backpack)

**PCF8574 backpack**
Purpose: To reduce the number of pins used on the Arduino Nano.

**LCD display (20x4)**
Purpose: To live-display environmental/electrical data: Humidity, Temperature, Voltage, Current, Power, Energy.

Display layout:
```
H(%):##.##T(C):##.##
V(V):##.##I(A):##.##
P(W):##.##
E(Wh):##.##
```

![LCD wiring diagram](./assets/lcd_wiring_diagram.png)

### 2. Photoresistors (North, West, South, East)

Purpose: To compare voltages across the four NWSE sensors and use `analogRead()` to drive `anglex` and `angley` for both motors.

![Photoresistor wiring diagram](./assets/photoresistor_wiring_diagram.png)

### 3. DHT-11 Sensor

Purpose: To read temperature (°C) and humidity (%) and display it live on the LCD.

![DHT-11 wiring diagram](./assets/dht11_wiring_diagram.png)

### 4. Servo Motors x2 (ServoX & ServoY)

**ServoX** — Drives panel rotation across the X-axis (0–180°).
**ServoY** — Drives panel rotation across the Y-axis (0–180°).

![Servo wiring diagram](./assets/servo_wiring_diagram.png)

### 5. Solar Panel (100x100mm) + ACS712

**Solar Panel** — Converts sunlight into electricity via the photoelectric effect, lighting the red LED indicator and supplying raw data for the Nano to read and display.

**ACS712** — Measures the current the panel produces, so the program can calculate real power output (V × I) and confirm the tracker is improving energy capture.

https://drive.google.com/file/d/1aOH7hUzrwU2VTqaUWwQd1LtM1pcOUhdv/view?usp=drive_link

## Support / Usage Components

| Component | Purpose |
|---|---|
| Push Button | Toggles the LCD display on/off (and backlight), acting as the manual switch for showing/hiding live sensor data without power-cycling the board. |
| Green LED | Indicates on/off status for the LCD display. |
| Red LED | Indicates the solar panel is actively generating current (lights up proportional to current flow detected by the ACS712). |
| External Power Provider + 9V Battery | Supplies power to the Nano and servos independently of USB, allowing the tracker to run untethered/portable. |
| Breadboard (x2) | Solderless platform for prototyping the sensor, LED, and divider circuits before finalizing connections. |
| Jumper Wires | Carry connections between the Arduino, sensors, LCD, servos, and breadboard. |
| Zenga Blocks (2x) | Structural support for mounting the servo motors at the correct height/angle. |
| Main Base | Mounting platform holding all components together as a stable unit. |

## Setup & Usage

**Required libraries** (install via Arduino IDE Library Manager):
- `Wire` (built-in)
- `Servo` (built-in)
- `LiquidCrystal I2C` (Frank de Brabander, v1.1.2)
- `DHT sensor library` (Adafruit)
- `Adafruit Unified Sensor`
- `Adafruit BusIO`

**Board**: Arduino Nano (select "ATmega328P" or "ATmega328P (Old Bootloader)" depending on your board revision)

**Steps:**
1. Wire components according to the diagrams above.
2. Open `solar_tracker_main/solar_tracker_main.ino` in the Arduino IDE.
3. Select the correct board and COM port (the one labeled "(USB)" if multiple ports appear).
4. Upload.
5. Use the toggle switch to turn the LCD display on/off.

## Conclusion

This project demonstrates a fully functional dual-axis solar tracking system built on an Arduino Nano, combining sensor-driven actuation, real-time environmental monitoring, and live power measurement into a single embedded system. Throughout, I've learned further about both hardware and software, the importance of soldering and the danger of touching unsoldered components after executing the code. This lesson came from personal experience — while trying to reduce the number of pins needed for the LCD display, I found the I2C PCF8574 backpack, which lets the LCD run on just two data pins instead of six or more. The backpack I ordered arrived unsoldered, and while testing whether it would still work before soldering it, I touched one of the exposed pins the moment I uploaded code — it was extremely hot, and I got a shock from it. Even at a safe 5V expected, exposed unsoldered joints can carry enough current to genuinely hurt, and that experience pushed me to actually learn proper soldering rather than treat it as optional. The tracker reliably follows sunlight across two axes while displaying live humidity, temperature, voltage, current, and power data. Looking ahead, the next major improvement would be to retain all the sensor and power data being collected rather than only displaying it live, and to use that accumulated data — solar output trends, humidity, and temperature — to predict incoming rain and automatically shut down or park the tracker, protecting the servos and panel from weather exposure before damage can occur. Overall, the project reflects hands-on experience in embedded C++, sensor calibration, closed-loop control, and hardware debugging, and serves as a practical demonstration of applied electrical engineering skills.
