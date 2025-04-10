#ifndef STORAGE_MANAGER_HPP
#define STORAGE_MANAGER_HPP

#include <vector>
#include <string>
#include "GameManager/GameManager.hpp"  // Erwartet, dass GameResult hier definiert ist

class StorageManager {
public:
    static StorageManager& getInstance();
    bool init();  
    bool saveGameResult(const GameResult &result);
    std::vector<GameResult> loadGameHistory();

private:
    StorageManager();    
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;
};

#endif // STORAGE_MANAGER_HPP
