#ifndef GAMEEND_H
#define GAMEEND_H

#include "../Model/Common.h"

class GameEnd {
public:
    static bool isGameOver(const int board[24], int piecesA, int piecesB);
};

#endif