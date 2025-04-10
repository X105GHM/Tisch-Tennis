#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <string>
#include <vector>
#include <mutex>

struct GameResult {
    std::string player1;
    int score1;
    std::string player2;
    int score2;
};
class GameManager {
public:
    static GameManager& getInstance();
    void setPlayerNames(const std::string &player1, const std::string &player2);
    void updateScore(const std::string &player, int inc);
    bool isGameFinished();
    bool finishGame(bool force = false);
    void taskLoop();
    GameResult getCurrentGame();
    void resetGame();
    bool isGameActive();
private:
    GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    std::string currentPlayer1;
    std::string currentPlayer2;
    int score1;
    int score2;
    bool gameActive;
    std::vector<GameResult> gameHistory;
    std::mutex mtx;
    bool checkWinningCondition(int s1, int s2);
};
#endif // GAME_MANAGER_HPP