#include "Logger/Logger.hpp"

static const __FlashStringHelper* getLoggerPrefix(LoggerType type) {
    switch(type) {
        case LoggerType::HTTP:
            return F("HTTP-Logger");
        case LoggerType::GAME:
            return F("Game-Logger");
        case LoggerType::STORAGE:
            return F("Storage-Logger");
        case LoggerType::WEBSERVER:
            return F("WebServer-Logger");
        case LoggerType::WIFI:
            return F("WiFi-Logger");
        case LoggerType::GENERAL:
        default:
            return F("General-Logger");
    }
}

void Logger::log(LoggerType type, const __FlashStringHelper* message) {
    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(getLoggerPrefix(type));
    Serial.println(F("----------------------------------------"));                                                    
    Serial.println();
    Serial.println(message);
    Serial.println(); 
    Serial.println(F("========================================"));
    Serial.println();
}
