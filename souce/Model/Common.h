#pragma once
#include <vector>

static const int BOARD_SIZE = 24;
static const int PIECES_PER_PLAYER = 8;

enum Player {
    EMPTY = 0,
    PLAYER_A = 1,
    PLAYER_B = 2  // Có thể là Người chơi 2 hoặc Bot
};

// Định nghĩa các trạng thái của Vòng lặp Game 
enum GameState {
    MENU,
    PHASE_1_PLACING,  // Giai đoạn đặt quân (Mỗi bên 9 quân)
    PHASE_2_MOVING,   // Giai đoạn di chuyển quân sang ô kề
    FLYING_MODE,      // Giai đoạn "bay" (khi chỉ còn 3 quân - tùy chọn mở rộng)
    MILL_STRIKE,      // Trạng thái chờ ăn 1 quân của đối thủ
    GAME_OVER
};


// Định nghĩa danh sách kề (24 vị trí)
const std::vector<std::vector<int>> ADJACENCY_LIST = {
    {1, 9},           // 0: kết nối với 1 (cạnh) và 9 (tâm từ ngoài)
    {0, 2, 4},        // 1: kết nối với 0, 2 (cạnh) và 4 (tâm)
    {1, 14},          // 2: kết nối với 1 (cạnh) và 14 (tâm từ ngoài)
    {4, 10},          // 3
    {3, 5, 1},        // 4: tâm
    {4, 17},          // 5
    {7, 11},          // 6
    {6, 8, 13},       // 7: tâm
    {7, 19},          // 8
    {0, 10, 21},      // 9: kết nối tầng ngoài-giữa-trong
    {9, 11, 3},       // 10: tâm giữa
    {10, 6, 15},      // 11
    {13, 18},         // 12
    {12, 14, 7},      // 13: tâm
    {13, 2, 23},      // 14
    {16, 11},         // 15
    {15, 17, 22},     // 16: tâm trong
    {16, 5, 20},      // 17
    {19, 12},         // 18
    {18, 20, 8},      // 19: tâm trong
    {19, 17},         // 20
    {9, 22},          // 21
    {21, 23, 16},     // 22: tâm trong
    {22, 14}          // 23
};

// Định nghĩa 16 dòng để kiểm tra luật ăn quân (Mill)
const int MILL_LINES[16][3] = {
    // Các dòng ngang
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},           // Tầng ngoài
    {9, 10, 11}, {12, 13, 14}, {15, 16, 17},  // Tầng giữa
    {18, 19, 20}, {21, 22, 23},                // Tầng trong
    // Các dòng dọc
    {0, 9, 21}, {2, 14, 23}, {6, 11, 15}, {8, 19, 17},  // Dọc ngoài
    {3, 10, 18}, {5, 12, 20}, {1, 4, 13}, {7, 16, 22}   // Dọc giữa & trong
};