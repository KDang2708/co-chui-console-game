#include "SaveLoad.h"
#include <fstream>
#include <iostream>

bool SaveLoad::saveGame(const int board[BOARD_SIZE], Player currentTurn, 
                       GameState currentState, int piecesPlacedA, 
                       int piecesPlacedB, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << " Loi: Khong the luu file!\n";
        return false;
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        file << board[i] << " ";
    }
    file << "\n";
    file << static_cast<int>(currentTurn) << "\n";
    file << static_cast<int>(currentState) << "\n";
    file << piecesPlacedA << "\n";
    file << piecesPlacedB << "\n";

    std::cout << " Da luu game vao " << filename << std::endl;
    return true;
}

bool SaveLoad::loadGame(int board[BOARD_SIZE], Player& currentTurn, 
                       GameState& currentState, int& piecesPlacedA, 
                       int& piecesPlacedB, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << " Loi: Khong tim thay file save!\n";
        return false;
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        file >> board[i];
    }
    int temp;
    file >> temp; currentTurn = static_cast<Player>(temp);
    file >> temp; currentState = static_cast<GameState>(temp);
    file >> piecesPlacedA >> piecesPlacedB;

    std::cout << " Da tai game tu " << filename << std::endl;
    return true;
}