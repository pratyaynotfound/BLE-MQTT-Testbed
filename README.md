# ESP32 BLE Sensor Testbed with MQTT Gateway

This project demonstrates a wireless sensor network where **ESP32 nodes** equipped with DHT11 sensors broadcast temperature and humidity data over **BLE (Bluetooth Low Energy)**. A **Raspberry Pi** (or any BLE-capable Linux device) acts as a **BLE Central** and forwards the sensor data to an **MQTT broker**.

---

## Hardware Requirements

- 1 or more ESP32 development boards  
- DHT11 sensors (1 per ESP32)
- Raspberry Pi 5 (or any BLE-capable Linux machine)
- Internet/local MQTT broker access (e.g., Mosquitto)

---

## Project Structure

### `esp32_ble_dht.ino`

ESP32 firmware:
- Reads temperature & humidity from a DHT11 sensor
- Generates a unique device name using the MAC address
- Broadcasts sensor readings using BLE notifications

### `ble_mqtt_gateway.py`

Python script (for Raspberry Pi):
- Scans for ESP32 devices via BLE
- Subscribes to BLE notifications from multiple ESP32s
- Publishes the received data to an MQTT broker

---

## Wiring DHT11 to ESP32

| DHT11 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| DATA      | GPIO 4    |

---

## ESP32 Setup Instructions

1. Install the required libraries:
   - `DHT sensor library` by Adafruit
   - `NimBLE-Arduino` (for BLE)

2. Flash the `esp32_ble_dht.ino` sketch to your ESP32 boards.

3. Open Serial Monitor to confirm BLE name and MAC.

---

## Gateway Setup on Raspberry Pi

### 1. Install dependencies:

```bash
sudo apt update
sudo apt install python3-pip
pip3 install bleak paho-mqtt
```
### 2. Set up Mosquitto MQTT broker (optional):

```
sudo apt install mosquitto mosquitto-clients
sudo systemctl enable mosquitto

```
### 3. Run the BLE-MQTT script:
```
python3 ble_mqtt_gateway.py
```

## MQTT Output Format
Each BLE notification published to MQTT will look like:
```
[ESP32_Node_xxxx] NODE:ESP32_Node_xxxx TEMP:26 HUM:49
```
- `XXXX` is the last two MAC bytes.
- Temperature and humidity are integers.

## Notes
- Ensure BLE and DHT11 libraries are compatible with your board.
- If no ESP32 devices are found, the script will retry every 5 seconds.
- Modify MQTT_BROKER, MQTT_USER, and MQTT_PASS as needed.

## License
MIT License


