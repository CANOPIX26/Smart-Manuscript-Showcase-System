# Smart Manuscript Showcase Control System

An advanced, multi-layered environmental preservation system designed for high-density micro-climate buffering of historical manuscripts. This project utilizes a triple-circuit architecture to maintain a stable, inert environment and manage interactive visitor engagement.

## 🚀 System Architecture

The system is controlled by three distinct processing units (Arduino Mega, Arduino Uno, and ESP8266), all programmed in **C++** via the Arduino IDE.

### 1. Climate & Thermal Regulation (Arduino Mega)
Monitors and stabilizes the internal micro-climate using redundant sensors and Peltier effect cooling/heating.
* **Redundancy:** Uses dual **DHT11** sensors (Pins 6, 7) to calculate average Temperature and Humidity.
* **Thermal Logic:** * **Cooling Mode:** Activates via H-Bridge (Pins 10, 11, 53, 51) if average temp > 18.5°C.
    * **Heating Mode:** Activates via H-Bridge (Pins 49, 47) if average temp < 18.5°C.
* **Humidity:** Triggers an Argon Air Pump (Pin 2) if humidity drops below 30%.
* **Safety System:** Integrated Buzzers (Pins 3, 4) and Alert LED (Pin 22) trigger if values exceed critical preservation limits (16°C–24°C).

### 2. Atmospheric Integrity (Arduino Uno)
Ensures the "Low-Pressure Vacuum Encasement" remains sealed and chemically inert.
* **I2C Management:** Uses a **TCA9548A Multiplexer** (Address 0x70) to manage dual BMP280 pressure sensors and a status LCD.
* **Pressure Monitoring:** Triggers motor-driven pumps if the pressure exceeds a **2000 hPa delta**.
* **Gas Detection:** Monitors chemical levels via analog sensors (A0, A1). If levels exceed the **90 unit threshold**, the system flushes the chamber with Argon and activates a DC Fan (Pin 4).

### 3. Interactive Lighting & Web Dashboard (ESP8266)
Manages visitor proximity and provides a real-time local web interface.
* **Proximity Detection:** Uses an Ultrasonic sensor (D0, D1) to detect visitors within **50cm**.
* **Adaptive Lighting:** Uses an **LDR** (A0) to measure ambient light. When a person is detected, the **L298N driver** (D2-D7) fades the LEDs to a brightness level that is safe for ancient inks (calculated via PWM).
* **Web Server:** Hosts a real-time AJAX dashboard accessible via local IP, providing live sensor telemetry (Light, Distance, and LED % status).

---

## 🛠 Hardware Connections Reference

### ESP8266 (Interactive Lighting)
| Component | Pin | Role |
| :--- | :--- | :--- |
| Ultrasonic Trig/Echo | D0, D1 | Proximity Detection |
| LDR Sensor | A0 | Ambient Light Input |
| L298N EnA, EnB | D2, D7 | PWM LED Brightness |
| L298N In1 - In4 | D3, D4, D5, D6 | LED Path Control |

### Arduino Mega (Climate Control)
| Component | Pin | Function |
| :--- | :--- | :--- |
| DHT11 (1 & 2) | D6, D7 | Redundant Climate Input |
| H-Bridge (TEC1) | D10, D11, D53-D47 | Peltier Thermal Control |
| Argon Relay | D2 | Humidification Pump |
| Alert System | D3, D4, D22 | Buzzers & Warning LED |

### Arduino Uno (Atmospheric Control)
| Component | Pin | Function |
| :--- | :--- | :--- |
| Gas Sensors (1 & 2) | A0, A1 | Chemical Sensitivity |
| DC Fan | D4 | Air Circulation |
| H-Bridge (Pump) | D5, D6, D8-D11 | Vacuum/Argon Control |

---

## 📁 Repository Structure
* `/Firmware`: Specialized `.ino` sketches for Mega, Uno, and ESP8266.
* `/Circuits`: Fritzing schematics and wiring diagrams.
* `/Docs`: Technical research on historical manuscript conservation.

## 📜 License
This project is shared under the MIT License.
