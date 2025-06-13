import asyncio
from bleak import BleakScanner, BleakClient
import paho.mqtt.client as mqtt
import time

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "iotuser"  # change if using password auth
MQTT_PASS = "xxxxxxxx"
MQTT_TOPIC = "esp32/sensor"

UUID = "12345678-1234-1234-1234-123456789abc"
CHAR_UUID = "abcd"

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)  # Avoid deprecation warning
mqtt_client.username_pw_set(MQTT_USER, MQTT_PASS)
mqtt_client.connect(MQTT_BROKER, MQTT_PORT)

async def read_notify(address, name):
    async with BleakClient(address) as client:
        def handle_notify(_, data):
            value = data.decode()
            print(f"[{name}] {value}")
            mqtt_client.publish(MQTT_TOPIC, value)

        await client.start_notify(CHAR_UUID, handle_notify)
        await asyncio.sleep(99999)

async def main():
    print("[*] Scanning for BLE devices...")

    while True:
        devices = await BleakScanner.discover(timeout=5.0)
        targets = [d for d in devices if "ESP32_Node" in (d.name or "")]

        if targets:
            print(f"[+] Found {len(targets)} ESP32 node(s). Connecting...")
            break
        else:
            print("[!] No ESP32 nodes found. Retrying in 5 seconds...")
            await asyncio.sleep(5)

    tasks = [read_notify(d.address, d.name) for d in targets]
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())
