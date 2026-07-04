#ifndef UNDOREDO_H
#define UNDOREDO_H

#include "../Model/Common.h"
#include <vector>
#include <tuple>
#include <array>

class UndoRedo {
public:
    static void saveState(const int board[24], Player currentTurn, GameState state, int piecesA, int piecesB);
    static bool undo(int board[24], Player& currentTurn, GameState& state, int& piecesA, int& piecesB);
    static bool redo(int board[24], Player& currentTurn, GameState& state, int& piecesA, int& piecesB);
    static void clearHistory();
};

#endif