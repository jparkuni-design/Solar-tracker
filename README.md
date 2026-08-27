# Solar Tracker

Dual-axis solar tracking system built on an Arduino Nano — developed as an electrical engineering co-op portfolio project (University of Waterloo).

The panel actively follows the sun across two axes using four light sensors, while an onboard display shows live environmental and power data. It's a hands-on hardware + embedded software project combining sensor input, closed-loop actuation, and real-time monitoring.

## Features

- Dual-axis automatic sun tracking (X/Y servos driven by 4-photoresistor comparison)
- Live 20x4 LCD status display (humidity, temperature in °C/°F, panel voltage/current/power)
- Toggle-switch controlled display with a loading animation on activation
- Temperature & humidity monitoring (DHT11)
- Real-time solar panel voltage and current sensing
- Live energy total (Wh) calculated on-device and printed to the Serial monitor

## Hardware

| Component | Purpose |
|---|---|
| Arduino Nano | Main controller |
| 2x Servo motors | X/Y axis panel tracking |
| 4x Photoresistors (LDR) | Light sensing, arranged N/S/E/W |
| DHT11 | Temperature & humidity sensing |
| 20x4 I2C LCD (PCF8574 backpack) | Status display |
| Voltage divider (10kΩ/10kΩ) + ACS712 current sensor | Panel voltage/current measurement |
| Toggle switch + LED | Display control / status indicator |

### Pin Map

| Signal | Pin |
|---|---|
| X-axis servo | D10 |
| Y-axis servo | D9 |
| DHT11 data | D8 |
| Display toggle switch | D3 (`INPUT_PULLUP`) |
| Status LED | D2 |
| Photoresistors | A0–A3 |
| Panel voltage sense | A6 |
| Panel current sense (ACS712) | A7 |
| LCD (I2C) | A4 (SDA) / A5 (SCL) |

## How It Works

Four photoresistors arranged North/South/East/West feed a threshold-based comparison loop: when the light difference between opposing sensors exceeds a set threshold, the corresponding servo nudges the panel toward the brighter side. Panel voltage and current are sampled continuously through a resistor-divider + ACS712 sensor pair to compute real-time output power. Flipping the toggle switch brings up a short loading animation on the LCD, then displays live humidity, temperature, and power readings; flipping it off turns the backlight and status LED off to save power.

## Energy Calculation (Wh)

`panelPower` (from the voltage/current readings) is instantaneous — a snapshot of watts at one moment, not a running total. Energy is power multiplied by time, and since power keeps changing as the panel tracks and light conditions shift, the sketch integrates it in small slices instead of doing one multiplication:

Each pass through `loop()`, it reads the actual elapsed time since the last pass with `millis()` (rather than assuming a fixed delay — LCD writes and the loading animation make loop timing uneven), converts that to hours, multiplies by the current power reading, and adds that sliver of energy to a running `energyWh` total. Repeated thousands of times, this converges on the real energy produced even though power is never constant.

```cpp
unsigned long now=millis();
float hours=(now - prevTime) / 3600000.0;
energyWh += panelPower * hours;
prevTime=now;
```

The running total is printed to the Serial monitor once per second (independent of the accumulation rate, just to keep the output readable):

```cpp
if (now - printTime >= printEvery) {
  Serial.print("Energy: ");
  Serial.print(energyWh, 4);
  Serial.println(" Wh");
  printTime=now;
}
```

## Getting Started

1. Open `SolarTracker/SolarTracker.ino` in the Arduino IDE
2. Install the required libraries via Library Manager:
   - `LiquidCrystal I2C` (Frank de Brabander)
   - `DHT sensor library` (Adafruit) + `Adafruit Unified Sensor`
3. Select **Arduino Nano** as the board (old bootloader vs. ATmega328P depends on your clone — see Troubleshooting below)
4. Wire components per the pin map above and upload

### Troubleshooting

If you're using a Nano clone with a CH340 USB chip and the port keeps disconnecting during upload, reinstall the CH340 driver and make sure the standard **ATmega328P** bootloader is selected (not the old bootloader option).

## Status

**In progress.** Core tracking, display, and power sensing are working. The project is scoped to stay Arduino-IDE-only (no SD card or external data pipeline) — energy output is calculated on-device and printed live to the Serial monitor.
- Energy accumulation (Wh) via ACS712 current sensor + voltage divider, integrated over time with `millis()` and printed to Serial once per second

## Author

Jim (jparkuni-design) — Electrical Engineering, University of Waterloo
