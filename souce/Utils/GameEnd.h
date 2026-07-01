#pragma once
#include "Model/Common.h"

class GameEnd {
public:
    static bool isGameOver(const int board[BOARD_SIZE], 
                           Player currentPlayer,
                           int piecesPlacedA,
                           int piecesPlacedB,
                           GamePhase currentPhase,
                           Player& winner);
};