#pragma once
#include "../Model/Common.h"

class GameEngine {
private:
    int board[24];          // Mảng lưu trạng thái bàn cờ hiện tại
    GameState currentState; // Trạng thái hiện tại của game
    Player currentTurn;     // Lượt của ai (PLAYER_A hay PLAYER_B)
    int piecesPlacedA;      // Số quân Player A đã đặt ở Phase 1 (Tối đa 8)
    int piecesPlacedB;      // Số quân Player B đã đặt ở Phase 1 (Tối đa 8)
    int gameMode;           // 1: Người vs Người, 2: Người vs Bot

public:
    GameEngine();
    void initGame();        // Khởi tạo lại các giá trị ban đầu
    void run();             // Vòng lặp chính (Game Loop)
    
private:
    void handleMenu();
    void handlePhase1();    // Xử lý Giai đoạn Đặt quân
    void handlePhase2();    // Xử lý Giai đoạn Di chuyển quân
    void handleMillStrike(); // Xử lý khi có người được ăn quân
    void switchTurn();      // Đổi lượt chơi
};