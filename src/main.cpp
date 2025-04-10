#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs_flash.h"
#include <ESPmDNS.h>
#include <Arduino.h>

#include "GameManager/GameManager.hpp"
#include "WebServerTask/WebServerTask.hpp"
#include "ScoreSenderTask/ScoreSenderTask.hpp"
#include "StorageManager/StorageManager.hpp"
#include "Logger/Logger.hpp"                       

void setup()
{
  Serial.begin(115200);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    nvs_flash_erase();
    ret = nvs_flash_init();
  }

  if(!StorageManager::getInstance().init()){
    Logger::log(LoggerType::STORAGE, F("SPIFFS-Initialisierung fehlgeschlagen"));
  } 
  else 
  {
    Logger::log(LoggerType::STORAGE, F("SPIFFS erfolgreich initialisiert"));
  }

  WiFiManager wifiManager;

  wifiManager.autoConnect("ESP32_ConfigAP");

  if (WiFi.status() == WL_CONNECTED) 
  {
    wifiManager.stopConfigPortal();
    Logger::log(LoggerType::WIFI, F("WiFi verbunden; ConfigPortal beendet"));
  }

  delay(1000);

  if (!MDNS.begin("tischtennis")) 
  {
    Logger::log(LoggerType::WEBSERVER, F("mDNS-Start fehlgeschlagen"));
  } 
  else if (MDNS.addService("http", "tcp", 80)) 
  {
    Logger::log(LoggerType::WEBSERVER, F("mDNS-Start erfolgreich"));
  } 
  else
  {
    Logger::log(LoggerType::WEBSERVER, F("Fehler beim Starten von mDNS"));
  }

  xTaskCreatePinnedToCore
  (
    WebServerTask::startTask,
    "WebServerTask",
    8192,
    nullptr,
    5,
    nullptr,
    1
  );

  xTaskCreatePinnedToCore([](void *param){ GameManager::getInstance().taskLoop(); },
    "GameManagerTask",
    4096,
    nullptr,
    5,
    nullptr,
    1
  );

  xTaskCreatePinnedToCore
  (
    ScoreSenderTask::startTask,
    "ScoreSenderTask",
    4096,
    nullptr,
    5,
    nullptr,
    1
  );
}

void loop()
{
  vTaskDelay(10 / portTICK_PERIOD_MS);
}
