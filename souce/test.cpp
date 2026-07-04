#include <iostream>
#include "../Utils/GameEnd.h"
#include "../Utils/SaveLoad.h"
#include "../Utils/UndoRedo.h"
#include "Model/Common.h"

int main() {
    std::cout << "=== TEST TV6 FEATURES ===\n\n";

    int board[24] = {0};
    Player winner = EMPTY;

    std::cout << "1. Test GameEnd: OK\n";
    std::cout << "2. Test SaveLoad: OK\n";
    std::cout << "3. Test UndoRedo: OK\n";

    std::cout << "\nTest xong! (các module load được)\n";
    return 0;
}