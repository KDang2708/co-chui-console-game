#pragma once
#include "../Model/GameRules.h"
#include <string>

class GameEngine {
private:
    int board[BOARD_SIZE];          // Mảng lưu trạng thái bàn cờ hiện tại
    GameState currentState;         // Trạng thái hiện tại của game
    Player currentTurn;             // Lượt của ai (PLAYER_A hay PLAYER_B)
    int piecesPlacedA;              // Số quân Player A đã đặt ở Phase 1 (Tối đa 8)
    int piecesPlacedB;              // Số quân Player B đã đặt ở Phase 1 (Tối đa 8)
    int gameMode;                   // 1: Người vs Người, 2: Người vs Bot
    Player winner;                  // Người chiến thắng khi game kết thúc
    std::string lastMessage;        // Thông báo trạng thái mới nhất

public:
    GameEngine();
    void initGame();                 // Khởi tạo lại các giá trị ban đầu
    void run();                      // Vòng lặp chính (Game Loop)

    const int* getBoard() const;
    GameState getCurrentState() const;
    Player getCurrentTurn() const;
    Player getWinner() const;
    int getPiecesPlacedA() const;
    int getPiecesPlacedB() const;
    const std::string& getLastMessage() const;
    void restart();

    MoveResult placePiece(int pos);
    MoveResult movePiece(int from, int to);
    MoveResult removePiece(int pos);

private:
    void handleMenu();
    void handlePhase1();            // Xử lý Giai đoạn Đặt quân
    void handlePhase2();            // Xử lý Giai đoạn Di chuyển quân
    void handleMillStrike();        // Xử lý khi có người được ăn quân
    void switchTurn();              // Đổi lượt chơi
    void setMessage(const std::string& message);
};