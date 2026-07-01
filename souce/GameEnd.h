#pragma once
#include "Model/GameRules.h"

class GameEnd {
public:
    static bool isGameOver(const int board[BOARD_SIZE], 
                           Player currentPlayer,
                           int piecesPlacedA,
                           int piecesPlacedB,
                           GamePhase currentPhase,
                           Player& winner);
};