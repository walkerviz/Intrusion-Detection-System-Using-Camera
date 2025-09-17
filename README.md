# Intrusion Detection System Using Camera
This project implements a camera-based intrusion detection system that provides real-time alerts. It's designed to monitor a specific area and notify users of any unauthorized entry through various warning methods.

## Features
* Real-time Intrusion Detection: Uses a camera to continuously monitor for motion or unauthorized presence.
* Multi-modal Warning System: Activates both a buzzer and an LED light for visual and audible alerts. The system is powered by a 12V source.
* Secure Communication: Leverages an ESP32 microcontroller and an Ethernet module for stable and reliable internet communication.
* Efficient Data Exchange: Utilizes the WebSocket protocol to enable fast and bi-directional message exchange with a server.

## Images
![image](https://github.com/user-attachments/assets/16a0e31d-5366-480a-83f8-146528103130)

## Hardware Requirements
* Microcontroller: ESP32
* Communication Module: Ethernet Module
* Warning Devices:
  * 12V Buzzer
  * 12V LED
* Power Supply: 12V Power Source
* Camera

## Software & Protocols
* Firmware: Developed for the ESP32 to handle sensor data and communication.
* Server-side Application: A server application to process data and send commands.
* Communication Protocol: WebSockets for real-time, low-latency communication between the ESP32 and the server.

## Getting Started
1. Hardware Setup
* Connect the ESP32 to the Ethernet module.
* Wire the 12V buzzer and LED to the appropriate pins on the ESP32, using a relay or a transistor to switch the 12V power supply.
* Connect your camera to the network.

2. Software Installation
Install the necessary ESP32 development environment (e.g., Arduino IDE with ESP32 board support).
Clone this repository:
```
Bash
git clone https://github.com/walkerviz/Intrusion-Detection-System-Using-Camera.git
```
Open the project in your IDE and configure the network settings (e.g., IP address of your server).

3. Operation
* The system operates by receiving motion detection signals from the camera via the server.
* Upon receiving an "intrusion" message, the ESP32 activates the buzzer and LED to issue a warning.
* Messages are sent and received using a WebSockets connection established between the ESP32 and the server.

## Contributing
This project is open to contributions. If you'd like to improve the code, add new features or fix bugs, please feel free to submit a pull request.
