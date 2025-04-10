#ifndef TIERLIST_MANAGER_HPP
#define TIERLIST_MANAGER_HPP
#include <string>
#include <map>
#include <mutex>
#include <vector>
#include <utility>
struct PlayerStats {
    int wins;
    int losses;
};
class TierListManager {
public:
    static TierListManager& getInstance();
    void addPlayer(const std::string &name);
    void updateStats(const std::string &winner, const std::string &loser);
    std::vector<std::pair<std::string, PlayerStats>> getTierList();
private:
    TierListManager();
    TierListManager(const TierListManager&) = delete;
    TierListManager& operator=(const TierListManager&) = delete;
    std::map<std::string, PlayerStats> playerStats;
    std::mutex mtx;
};
#endif // TIERLIST_MANAGER_HPP
