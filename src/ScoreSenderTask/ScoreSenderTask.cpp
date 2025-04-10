#include "ScoreSenderTask.hpp"
#include "GameManager/GameManager.hpp"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>

volatile bool ScoreSenderTask::isDisplayActive = false;

#define TARGET_ESP32_HOSTNAME "nixieclock"

void ScoreSenderTask::startTask(void *parameter) 
{
    HTTPClient http;
    while(true) 
    {
        if(GameManager::getInstance().isGameActive()){
            GameResult res = GameManager::getInstance().getCurrentGame();
            std::string score1Str = std::string(res.score1 < 10 ? "0" : "") + std::to_string(res.score1);
            std::string score2Str = std::string(res.score2 < 10 ? "0" : "") + std::to_string(res.score2);
            std::string customValue = score1Str + "00" + score2Str;
            
            std::string url = "http://" + std::string(TARGET_ESP32_HOSTNAME) 
                            + ".local/set/customdigits?value=" + customValue 
                            + "&active=" + (getDisplayActive() ? "1" : "0");

            http.begin(url.c_str());
            http.GET();
            http.end();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void ScoreSenderTask::setDisplayActive(bool active)
{
    isDisplayActive = active;
}

bool ScoreSenderTask::getDisplayActive()
{
    return isDisplayActive;
}
