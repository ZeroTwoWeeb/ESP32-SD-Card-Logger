# ESP32-SD-Card-Logger

A simple SD-Card Logger for the ESP-WROOM-32 with RTC, LCD and Ultrasonic-sensor.

## Circuit

Built on <a href="https://www.az-delivery.de/products/esp32-dev-kit-c-v4-unverlotet">ESP32-WROOM-32</a> from <a href="https://www.az-delivery.de/en">AZ-Delivery</a>.

<img src=./graphics/Circuit.jpg width=600>

## Parts

- Simple button
- 10k Potentiometer
- 10k Ohm resistor
- <a href="https://www.az-delivery.de/products/3er-set-hc-sr04-ultraschallmodule">Ultrasonic-Sensor</a> (HC-SR04)
- <a href="https://www.amazon.de/-/en/SUNFOUNDER-SD-Card-Module-Arduino/dp/B07R6HQGS1/ref=sr_1_4?crid=2JJTJ383MF7DM&keywords=sd-kartenmodul&qid=1670612869&sprefix=sd-card+module%2Caps%2C134&sr=8-4">SD-Card Module</a> (Standard size SD)
- <a href="https://www.az-delivery.de/products/16x2-lcd-blaues-display">LCD 16x2</a>
- <a href="https://www.az-delivery.de/products/ds3231-real-time-clock">Real-Time-Clock</a> (DS3231)
- <a href="https://www.az-delivery.de/products/esp32-dev-kit-c-v4-unverlotet">ESP32-WROOM-32</a>

## <p style="color:red;">Warning</p>

- The Pinout maybe diffenet on your ESP32-Board. Check the datasheet.

## TO-DO

- Log Battery Voltage
- Show Remaining / Used Storage and Voltage on LCD
- Integrate a menu system
- Integrate a button to switch between menus
- Show SD-Card errors on screen
- Log important events <p style="color:lightblue;">e.g. Interrupts, Errors, Storage Usage Information, Voltage, Unusual Values</p>
- Swap Ultrasonic-Sensor out for version US-100
