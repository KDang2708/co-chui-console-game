#ifndef AI_EASYMED_H
#define AI_EASYMED_H

#include "../Model/GameRules.h"
#include <random>
#include <vector>

struct AIMove {
    int from;
    int to;
    bool valid;

    AIMove();
    AIMove(int moveFrom, int moveTo);
};

class AIEasyMed {
public:
    explicit AIEasyMed(unsigned int seed = std::random_device{}());

    int chooseEasyPlacement(const int board[BOARD_SIZE]);
    AIMove chooseEasyMove(const int board[BOARD_SIZE], int player);

    int chooseMediumPlacement(const int board[BOARD_SIZE], int player);
    AIMove chooseMediumMove(const int board[BOARD_SIZE], int player);

    int choosePieceToRemove(const int board[BOARD_SIZE], int opponent);

private:
    std::mt19937 rng;

    int opponentOf(int player) const;
    int randomIndex(int size);
    int chooseRandomPosition(const std::vector<int>& positions);
    AIMove chooseRandomMove(const std::vector<std::pair<int, int>>& moves);

    std::vector<int> getValidPlacements(const int board[BOARD_SIZE]) const;
    bool placementCreatesMill(const int board[BOARD_SIZE], int pos, int player) const;
    bool moveCreatesMill(const int board[BOARD_SIZE], const std::pair<int, int>& move, int player) const;

    int findWinningPlacement(const int board[BOARD_SIZE], int player) const;
    int findBlockingPlacement(const int board[BOARD_SIZE], int player) const;
    AIMove findWinningMove(const int board[BOARD_SIZE], int player) const;
    AIMove findBlockingMove(const int board[BOARD_SIZE], int player) const;
};

#endif
