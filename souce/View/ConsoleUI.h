#pragma once

#include "../Model/GameRules.h"
#include <string>

class ConsoleUI {
public:
    void drawBoard(const int board[BOARD_SIZE]);
    int getPlayerInput(const int board[BOARD_SIZE], const std::string& prompt);
    void printMessage(const std::string& message);
};
