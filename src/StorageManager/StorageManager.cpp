#include "StorageManager/StorageManager.hpp"
#include <SPIFFS.h>
#include "Logger/Logger.hpp"
#include <Arduino.h>

#define GAME_HISTORY_FILE "/gamehistory.txt"

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

StorageManager::StorageManager() {}

bool StorageManager::init() {
    if (!SPIFFS.begin(true)) {
        Logger::log(LoggerType::STORAGE, F("SPIFFS mount failed"));
        return false;
    }
    Logger::log(LoggerType::STORAGE, F("SPIFFS mounted successfully"));
    return true;
}

bool StorageManager::saveGameResult(const GameResult &result) {
    if (result.player1.empty() || result.player2.empty()) {
        Logger::log(LoggerType::STORAGE, F("Invalid player names - aborting save"));
        return false;
    }
    if (result.score1 < 0 || result.score2 < 0) {
        Logger::log(LoggerType::STORAGE, F("Invalid scores - aborting save"));
        return false;
    }

    File file = SPIFFS.open(GAME_HISTORY_FILE, FILE_APPEND);
    if (!file) {
        Logger::log(LoggerType::STORAGE, F("Failed to open game history file for appending"));
        return false;
    }

    String line = String(result.player1.c_str()) + "," + String(result.score1) + "," +
                  String(result.player2.c_str()) + "," + String(result.score2) + "\n";
    if (file.print(line) <= 0) {
        Logger::log(LoggerType::STORAGE, F("Failed to write game result to file"));
        file.close();
        return false;
    }
    file.close();
    Logger::log(LoggerType::STORAGE, F("Game result saved successfully"));
    return true;
}

std::vector<GameResult> StorageManager::loadGameHistory() {
    std::vector<GameResult> history;
    File file = SPIFFS.open(GAME_HISTORY_FILE, FILE_READ);
    if (!file) {
        Logger::log(LoggerType::STORAGE, F("Failed to open game history file for reading"));
        return history;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.length() == 0) continue;

        int firstComma = line.indexOf(',');
        int secondComma = line.indexOf(',', firstComma + 1);
        int thirdComma = line.indexOf(',', secondComma + 1);

        if (firstComma == -1 || secondComma == -1 || thirdComma == -1) {
            Logger::log(LoggerType::STORAGE, F("Malformed line in game history file"));
            continue;
        }

        String p1 = line.substring(0, firstComma);
        String s1 = line.substring(firstComma + 1, secondComma);
        String p2 = line.substring(secondComma + 1, thirdComma);
        String s2 = line.substring(thirdComma + 1);

        if (p1.isEmpty() || p2.isEmpty() || s1.length() == 0 || s2.length() == 0) {
            Logger::log(LoggerType::STORAGE, F("Invalid line content - skipping entry"));
            continue;
        }

        int score1 = s1.toInt();
        int score2 = s2.toInt();

        if (score1 < 0 || score2 < 0) {
            Logger::log(LoggerType::STORAGE, F("Negative score values found - skipping entry"));
            continue;
        }

        GameResult result;
        result.player1 = std::string(p1.c_str());
        result.score1 = score1;
        result.player2 = std::string(p2.c_str());
        result.score2 = score2;

        history.push_back(result);
    }
    file.close();
    Logger::log(LoggerType::STORAGE, F("Game history loaded successfully"));
    return history;
}
