# MAX7219 Dynamic Notice Board with ESP8266

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## Overview

This project implements a dynamic notice board using the MAX7219 LED matrix display and ESP8266 (NodeMCU) microcontroller. The notice board can be updated dynamically through a web interface, allowing users to change the displayed message, scroll speed, and brightness level.

## Table of Contents

- [Hardware](#hardware)
- [Web Interface](#web-interface)
- [Setup](#setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Hardware used

# BASE MODEL (ESP8266) 

| Device                           | Description                                      |
| -------------------------------- | -------------------------------------------------|
| MCU                              | Nodemcu ESP8266 (4MB FLASH) V 1.1                |
| Display unit                     | MAX719 :: FC-16 8*8 matrix (24 units)            |
| Power unit                       | LM2596 / 4R7 Based DC-DC Buck convertor 12.5 - 5v|

# MAIN MODEL (ESP-32)

## Web Interface

The notice board can be accessed and configured through a web interface. Users can update the message, adjust the scroll speed, and set the brightness level using a simple form.

![Web Interface Screenshot](/docs/web_interface.png)
<!-- Add additional screenshots or illustrations if needed -->

## Setup

1. **Hardware Connection:**
   - Connect the MAX7219 LED matrix display to the ESP8266 microcontroller.
   - <!-- Add specific hardware connection details -->

2. **Software Setup:**
   - Install the required libraries by including them in the Arduino IDE.
   - Configure the Wi-Fi credentials in the `setup` function.
   - Upload the code to the ESP8266 microcontroller.

## Usage

1. Power on the ESP8266 microcontroller.
2. Connect to the Wi-Fi network.
3. Open a web browser and navigate to the IP address of the ESP8266.
4. Update the message, scroll speed, and brightness level using the web interface.

## Contributing

Contributions are welcome! If you have ideas, suggestions, or bug reports, please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
