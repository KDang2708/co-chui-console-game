#pragma once
#include "Model/Common.h"
#include <vector>
#include <array>
#include <tuple>

class UndoRedo {
private:
    static std::vector<std::tuple<std::array<int, BOARD_SIZE>, Player, GameState, int, int>> history;
    static std::vector<std::tuple<std::array<int, BOARD_SIZE>, Player, GameState, int, int>> redoStack;

public:
    static void saveState(const int board[BOARD_SIZE], Player currentTurn, GameState state, int piecesA, int piecesB);
    static bool undo(int board[BOARD_SIZE], Player& currentTurn, GameState& state, int& piecesA, int& piecesB);
    static bool redo(int board[BOARD_SIZE], Player& currentTurn, GameState& state, int& piecesA, int& piecesB);
    static void clearHistory();
};