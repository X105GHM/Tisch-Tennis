#include "WebServerTask.hpp"
#include "GameManager/GameManager.hpp"
#include "TierListManager/TierListManager.hpp"
#include "StorageManager/StorageManager.hpp"
#include "Logger/Logger.hpp"
#include "ScoreSenderTask/ScoreSenderTask.hpp"
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <string>
#include <optional>
#include <charconv>

AsyncWebServer server(80);

namespace {
    std::optional<int> parseIntParam(AsyncWebServerRequest *request, const char *param) {
        if (!request->hasParam(param)) return std::nullopt;
        std::string_view value = request->getParam(param)->value().c_str();
        int result;
        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
        if (ec != std::errc()) return std::nullopt;
        return result;
    }

    std::optional<std::string> getParamValue(AsyncWebServerRequest *request, const char *param) {
        if (!request->hasParam(param)) return std::nullopt;
        return std::string(request->getParam(param)->value().c_str());
    }
}

void WebServerTask::startTask(void *parameter) {
    if (!SPIFFS.begin(true)) {
        Logger::log(LoggerType::STORAGE, F("SPIFFS mount failed in WebServerTask"));
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.serveStatic("/script.js", SPIFFS, "/script.js");

    server.on("/setNames", HTTP_GET, [](AsyncWebServerRequest *request) {
        auto p1 = getParamValue(request, "player1");
        auto p2 = getParamValue(request, "player2");

        if (p1 && p2) {
            GameManager::getInstance().setPlayerNames(*p1, *p2);
            Logger::log(LoggerType::WEBSERVER, F("Player names set via /setNames"));
            request->send(200, "text/plain", "Player names set");
        } else {
            Logger::log(LoggerType::WEBSERVER, F("Missing parameters for /setNames"));
            request->send(400, "text/plain", "Missing player names");
        }
    });

    server.on("/updateScore", HTTP_GET, [](AsyncWebServerRequest *request) {
        auto id = parseIntParam(request, "id");
        auto inc = parseIntParam(request, "inc");

        if (!id || !inc || (*id != 1 && *id != 2)) {
            Logger::log(LoggerType::WEBSERVER, F("Invalid or missing parameters for /updateScore"));
            request->send(400, "text/plain", "Invalid or missing parameters");
            return;
        }

        GameResult current = GameManager::getInstance().getCurrentGame();
        if (*id == 1) {
            GameManager::getInstance().updateScore(current.player1, *inc);
        } else {
            GameManager::getInstance().updateScore(current.player2, *inc);
        }

        if (GameManager::getInstance().isGameFinished()) {
            GameManager::getInstance().finishGame();
            request->send(200, "application/json", R"({"status":"finished"})");
        } else {
            request->send(200, "text/plain", "Score updated");
        }
    });

    server.on("/getTierList", HTTP_GET, [](AsyncWebServerRequest *request) {
        std::string response = "[";
        const auto& tierList = TierListManager::getInstance().getTierList();
        if (tierList.empty()) {
            response = R"([{"name":"Hier gibt es noch nichts zu sehen","wins":0,"losses":0}])";
        } else {
            for (size_t i = 0; i < tierList.size(); ++i) {
                response += "{\"name\":\"" + tierList[i].first + "\",\"wins\":" +
                            std::to_string(tierList[i].second.wins) + ",\"losses\":" +
                            std::to_string(tierList[i].second.losses) + "}";
                if (i + 1 < tierList.size()) response += ",";
            }
            response += "]";
        }
        request->send(200, "application/json", response.c_str());
    });

    server.on("/getGameHistory", HTTP_GET, [](AsyncWebServerRequest *request) {
        std::string response = "[";
        const auto& history = StorageManager::getInstance().loadGameHistory();
        if (history.empty()) {
            response = R"([{"player1":"Hier gibt es noch nichts zu sehen","score1":0,"player2":"","score2":0}])";
        } else {
            for (size_t i = 0; i < history.size(); ++i) {
                response += "{\"player1\":\"" + history[i].player1 + "\",\"score1\":" +
                            std::to_string(history[i].score1) + ",\"player2\":\"" +
                            history[i].player2 + "\",\"score2\":" + std::to_string(history[i].score2) + "}";
                if (i + 1 < history.size()) response += ",";
            }
            response += "]";
        }
        request->send(200, "application/json", response.c_str());
    });

    server.on("/finishGame", HTTP_GET, [](AsyncWebServerRequest *request) {
        bool force = request->hasParam("force") && request->getParam("force")->value() == "true";
        bool success = GameManager::getInstance().finishGame(force);

        if (success) {
            GameResult current = GameManager::getInstance().getCurrentGame();
            std::string winner = (current.score1 > current.score2) ? current.player1 : current.player2;
            std::string response = "Game finished and recorded. Winner: " + winner;
            request->send(200, "application/json", R"({"status":"finished"})");
        } else {
            request->send(400, "text/plain", "Game not finished");
        }
    });

    server.on("/setDisplayActive", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("active")) {
            String activeParam = request->getParam("active")->value();

            ScoreSenderTask::setDisplayActive(activeParam == "1");
            Logger::log(LoggerType::WEBSERVER, F("Display active updated"));
            request->send(200, "text/plain", "Display active set");
        } else {
            request->send(400, "text/plain", "Missing parameter");
        }
    });

    server.begin();
    Logger::log(LoggerType::WEBSERVER, F("Web server started"));

    vTaskDelete(NULL); // Task sauber beenden
}
