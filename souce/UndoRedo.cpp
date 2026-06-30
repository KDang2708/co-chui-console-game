#include "UndoRedo.h"
#include <iostream>

std::vector<std::tuple<std::array<int, BOARD_SIZE>, Player, GameState, int, int>> UndoRedo::history;
std::vector<std::tuple<std::array<int, BOARD_SIZE>, Player, GameState, int, int>> UndoRedo::redoStack;

void UndoRedo::saveState(const int board[BOARD_SIZE], Player currentTurn, GameState state, int piecesA, int piecesB) {
    std::array<int, BOARD_SIZE> boardCopy;
    for (int i = 0; i < BOARD_SIZE; i++) {
        boardCopy[i] = board[i];
    }
    history.emplace_back(boardCopy, currentTurn, state, piecesA, piecesB);
    redoStack.clear();
}

bool UndoRedo::undo(int board[BOARD_SIZE], Player& currentTurn, GameState& state, int& piecesA, int& piecesB) {
    if (history.empty()) {
        std::cout << "Khong con nuoc di de Undo!\n";
        return false;
    }

    auto [boardState, turn, gameState, pa, pb] = history.back();
    history.pop_back();

    std::array<int, BOARD_SIZE> currentBoard;
    for (int i = 0; i < BOARD_SIZE; i++) currentBoard[i] = board[i];
    redoStack.emplace_back(currentBoard, currentTurn, state, piecesA, piecesB);

    for (int i = 0; i < BOARD_SIZE; i++) board[i] = boardState[i];
    currentTurn = turn;
    state = gameState;
    piecesA = pa;
    piecesB = pb;

    std::cout << "✅ Đã Undo một nước đi.\n";
    return true;
}

bool UndoRedo::redo(int board[BOARD_SIZE], Player& currentTurn, GameState& state, int& piecesA, int& piecesB) {
    if (redoStack.empty()) {
        std::cout << "Khong con nuoc di de Redo!\n";
        return false;
    }

    auto [boardState, turn, gameState, pa, pb] = redoStack.back();
    redoStack.pop_back();

    std::array<int, BOARD_SIZE> currentBoard;
    for (int i = 0; i < BOARD_SIZE; i++) currentBoard[i] = board[i];
    history.emplace_back(currentBoard, currentTurn, state, piecesA, piecesB);

    for (int i = 0; i < BOARD_SIZE; i++) board[i] = boardState[i];
    currentTurn = turn;
    state = gameState;
    piecesA = pa;
    piecesB = pb;

    std::cout << "✅ Đã Redo một nước đi.\n";
    return true;
}

void UndoRedo::clearHistory() {
    history.clear();
    redoStack.clear();
}