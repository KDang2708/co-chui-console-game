#include "ConsoleUI.h"
#include <iostream>
#include <limits>

static const char CELL_SYMBOLS[] = {'.', 'X', 'O'};

void ConsoleUI::drawBoard(const int board[BOARD_SIZE]) {
    std::cout << "\n--- BAN CO CO CHUI ---\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        char symbol = CELL_SYMBOLS[board[i]];
        std::cout << "[" << i << ":" << symbol << "] ";
        if ((i + 1) % 6 == 0) std::cout << "\n";
    }
    std::cout << "\n";
}

int ConsoleUI::getPlayerInput(const int board[BOARD_SIZE], const std::string& prompt) {
    int position = -1;
    while (true) {
        std::cout << prompt;
        if (!(std::cin >> position)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Nhap khong hop le. Vui long nhap so tu 0 den " << BOARD_SIZE - 1 << ".\n";
            continue;
        }
        if (position < 0 || position >= BOARD_SIZE) {
            std::cout << "Chi so nam ngoai pham vi. Vui long thu lai.\n";
            continue;
        }
        return position;
    }
}

void ConsoleUI::printMessage(const std::string& message) {
    std::cout << message << "\n";
}
