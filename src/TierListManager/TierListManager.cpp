#include "TierListManager.hpp"
#include "Logger/Logger.hpp"
#include <algorithm>

TierListManager& TierListManager::getInstance() {
    static TierListManager instance;
    return instance;
}

TierListManager::TierListManager() {}

void TierListManager::addPlayer(const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);
    if(playerStats.find(name) == playerStats.end()){
        playerStats[name] = {0,0};
        Logger::log(LoggerType::GAME, F("Player added to TierList"));
    }
}

void TierListManager::updateStats(const std::string &winner, const std::string &loser) {
    {
        std::lock_guard<std::mutex> lock(mtx);

        // Statt addPlayer(): direkt map-Zugriff mit insert
        if (playerStats.find(winner) == playerStats.end())
            playerStats[winner] = {0, 0};
        if (playerStats.find(loser) == playerStats.end())
            playerStats[loser] = {0, 0};

        playerStats[winner].wins++;
        playerStats[loser].losses++;
    }

    // Logging außerhalb der mutex-section
    // Logger::log(LoggerType::GAME, "TierList updated"); // Optional
}


std::vector<std::pair<std::string, PlayerStats>> TierListManager::getTierList() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::pair<std::string, PlayerStats>> tierList(playerStats.begin(), playerStats.end());
    std::sort(tierList.begin(), tierList.end(), [](const auto &a, const auto &b){
        return a.second.wins > b.second.wins;
    });
    Logger::log(LoggerType::GAME, F("TierList retrieved"));
    return tierList;
}
