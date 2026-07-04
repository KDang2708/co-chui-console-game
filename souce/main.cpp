#include <iostream>
#include "souce/GameEnd.h"
#include "souce/SaveLoad.h"
#include "souce/UndoRedo.h"
#include "souce/Model/Common.h"   // Để lấy enum Player, GameState...

int main() {
    std::cout << "=== TEST TV6 MODULES ===\n\n";

    std::cout << "[1] GameEnd module: Included OK\n";
    std::cout << "[2] SaveLoad module: Included OK\n";
    std::cout << "[3] UndoRedo module: Included OK\n\n";

    std::cout << "TV6 test hoan thanh!\n";
    std::cout << "Neu khong co loi compile thi cac module da load duoc.\n";

    std::cin.get();
    return 0;
}