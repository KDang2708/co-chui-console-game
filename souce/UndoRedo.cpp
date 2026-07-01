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

    auto previousState = history.back();
    history.pop_back();

    std::array<int, BOARD_SIZE> boardState = std::get<0>(previousState);
    Player turn = std::get<1>(previousState);
    GameState gameState = std::get<2>(previousState);
    int pa = std::get<3>(previousState);
    int pb = std::get<4>(previousState);

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

    auto nextState = redoStack.back();
    redoStack.pop_back();

    std::array<int, BOARD_SIZE> boardState = std::get<0>(nextState);
    Player turn = std::get<1>(nextState);
    GameState gameState = std::get<2>(nextState);
    int pa = std::get<3>(nextState);
    int pb = std::get<4>(nextState);

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