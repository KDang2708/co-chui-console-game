#ifndef SAVELOAD_H
#define SAVELOAD_H

#include "../Model/Common.h"
#include <string>

class SaveLoad {
public:
    static bool saveGame(const int board[24], Player currentTurn, GameState state, int piecesA, int piecesB, const std::string& filename);
    static bool loadGame(int board[24], Player& currentTurn, GameState& state, int& piecesA, int& piecesB, const std::string& filename);
};

#endif