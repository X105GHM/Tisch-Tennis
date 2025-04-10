#include "MyWiFiManager.hpp"
#include <WiFi.h>
#include <WiFiManager.h>

MyWiFiManager& MyWiFiManager::getInstance() {
  static MyWiFiManager instance;
  return instance;
}

MyWiFiManager::MyWiFiManager() {}

void MyWiFiManager::begin() {
  WiFi.mode(WIFI_STA);
  ::WiFiManager wifiManager;
  wifiManager.autoConnect("ESP32_ConfigAP");
}
