#include "GameManager.hpp"
#include "TierListManager/TierListManager.hpp"
#include "StorageManager/StorageManager.hpp"
#include "Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <stdlib.h>

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

GameManager::GameManager() 
    : currentPlayer1("Player1"), currentPlayer2("Player2"), score1(0), score2(0), gameActive(false) {}

bool GameManager::checkWinningCondition(int s1, int s2) {
    return ((s1 >= 11 || s2 >= 11) && abs(s1 - s2) >= 2);
}

void GameManager::setPlayerNames(const std::string &player1, const std::string &player2) {
    std::lock_guard<std::mutex> lock(mtx);
    currentPlayer1 = player1;
    currentPlayer2 = player2;
    Logger::log(LoggerType::GAME, F("Player names updated"));
}

void GameManager::updateScore(const std::string &player, int inc) {
    std::lock_guard<std::mutex> lock(mtx);
    if(!gameActive && inc > 0) { 
        gameActive = true; 
        Logger::log(LoggerType::GAME, F("Game started"));
    }
    if(player == currentPlayer1) {
        score1 += inc;
        Logger::log(LoggerType::GAME, F("Score updated for Player1"));
    } else if(player == currentPlayer2) {
        score2 += inc;
        Logger::log(LoggerType::GAME, F("Score updated for Player2"));
    }
}

bool GameManager::isGameFinished() {
    std::lock_guard<std::mutex> lock(mtx);
    return checkWinningCondition(score1, score2);
}

bool GameManager::finishGame(bool force) {
    std::lock_guard<std::mutex> lock(mtx);

    if (!force && !checkWinningCondition(score1, score2)) {
        Logger::log(LoggerType::GAME, F("finishGame called, but winning condition not met and not forced"));
        Logger::log(LoggerType::GAME, force ? F("force=true erkannt") : F("force=false erkannt"));

        return false;
    }

    GameResult result{currentPlayer1, score1, currentPlayer2, score2};
    gameHistory.push_back(result);

    if(!StorageManager::getInstance().saveGameResult(result)) {
        Logger::log(LoggerType::GAME, F("Failed to save game result to storage"));
    } else {
        Logger::log(LoggerType::GAME, F("Game result saved successfully"));
    }
    
    if(score1 > score2) {
        TierListManager::getInstance().updateStats(currentPlayer1, currentPlayer2);
    } else {
        TierListManager::getInstance().updateStats(currentPlayer2, currentPlayer1);
    }
    Logger::log(LoggerType::GAME, F("Game finished. Resetting game state"));
    
    resetGame();

    return true;
}


GameResult GameManager::getCurrentGame() {
    std::lock_guard<std::mutex> lock(mtx);
    return GameResult{currentPlayer1, score1, currentPlayer2, score2};
}

void GameManager::resetGame() {
    score1 = 0;
    score2 = 0;
    gameActive = false;
    Logger::log(LoggerType::GAME, F("Game state reset"));
}

void GameManager::taskLoop() {
    while(true) {
        if(isGameFinished()){
            finishGame();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

bool GameManager::isGameActive() {
    std::lock_guard<std::mutex> lock(mtx);
    return gameActive;
}
