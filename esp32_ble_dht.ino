#include <Arduino.h>
#include <NimBLEDevice.h>
#include "esp_system.h"
#include "esp_mac.h"
#include <DHT.h>

// DHT11 setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// BLE setup
NimBLEServer *bleServer;
NimBLECharacteristic *sensorCharacteristic;
std::string deviceName;

void setup() {
  Serial.begin(115200);

  // Start DHT sensor
  dht.begin();

  // Read BLE MAC address and generate a unique device name
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);  // Get BLE MAC, not Wi-Fi
  char nameBuf[32];
  sprintf(nameBuf, "ESP32_Node_%02X%02X", mac[4], mac[5]);
  deviceName = std::string(nameBuf);

  Serial.printf("Device name: %s\n", deviceName.c_str());
  Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Initialize BLE
  NimBLEDevice::init(deviceName);
  bleServer = NimBLEDevice::createServer();

  NimBLEService *sensorService = bleServer->createService("12345678-1234-1234-1234-123456789abc");

  sensorCharacteristic = sensorService->createCharacteristic(
    "abcd",
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );

  // Set initial dummy value
  std::string initialValue = "NODE:" + deviceName + " TEMP:-- HUM:--";
  sensorCharacteristic->setValue(initialValue);

  // Start BLE services
  sensorService->start();
  bleServer->getAdvertising()->start();

  Serial.println("BLE sensor started.");
}

void loop() {
  delay(2000);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Check for failed readings
  if (isnan(temp) || isnan(hum)) {
    Serial.println("[!] Failed to read from DHT sensor!");
    return;
  }

  std::string value = "NODE:" + deviceName +
                      " TEMP:" + std::to_string((int)temp) +
                      " HUM:" + std::to_string((int)hum);

  sensorCharacteristic->setValue(value);

  if (bleServer->getConnectedCount() > 0) {
    sensorCharacteristic->notify();
    Serial.println("✅ Notified: " + String(value.c_str()));
  } else {
    Serial.println("[ ] No central connected. Skipping notify.");

    //  Restart advertising when central disconnected
    bleServer->getAdvertising()->start();
    Serial.println("Advertising restarted.");
  }
}
