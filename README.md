# ESP32-SD-Card-Logger

Logger for ESP32 with RTC, LCD and Ultrasonic-sensor.

## Circuit

Built on (<a href="https://www.az-delivery.de/products/esp32-dev-kit-c-v4-unverlotet">ESP32-wroom-32 from AZ-Delivery</a>)

![graphic of circuit](./graphics/Circuit.jpg)

## TO-DO

- detect voltage Drop(integrate Voltage-Sensor for Battery pack)
- Show SD Storage Usage on Screen
- Button to swap LCD Menu
- Code efficency/energy efficency
- Detect spikes/wrong values from Ultrasonic-Sensor
- document detection and creation whithout overwriting old Data
- Write all errors to error log
- write important events to log (interrupts, Errors, Storage usage information, Voltage, Suspect values)
- Chane Ultrasonic-Sensor to US-100
