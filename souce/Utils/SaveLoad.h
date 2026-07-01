#pragma once
#include "Model/Common.h"
#include <string>

class SaveLoad {
public:
    static bool saveGame(const int board[BOARD_SIZE], 
                        Player currentTurn, 
                        GameState currentState,
                        int piecesPlacedA, 
                        int piecesPlacedB,
                        const std::string& filename = "savegame.txt");

    static bool loadGame(int board[BOARD_SIZE], 
                        Player& currentTurn, 
                        GameState& currentState,
                        int& piecesPlacedA, 
                        int& piecesPlacedB,
                        const std::string& filename = "savegame.txt");
};