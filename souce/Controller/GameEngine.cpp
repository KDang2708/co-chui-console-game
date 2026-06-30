#include "GameEngine.h"
#include <iostream>

GameEngine::GameEngine() {
    initGame();
}

void GameEngine::initGame() {
    for(int i = 0; i < 24; i++) board[i] = EMPTY;
    currentState = MENU;
    currentTurn = PLAYER_A;
    piecesPlacedA = 0;
    piecesPlacedB = 0;
    gameMode = 1;
}

void GameEngine::run() {
    while (currentState != GAME_OVER) {
        switch (currentState) {
            case MENU:
                handleMenu();
                break;
            case PHASE_1_PLACING:
                handlePhase1();
                break;
            case PHASE_2_MOVING:
                handlePhase2();
                break;
            case MILL_STRIKE:
                handleMillStrike();
                break;
            default:
                break;
        }
    }
    std::cout << "Cam on ban da choi game!" << std::endl;
}

void GameEngine::handleMenu() {
    // 1. Gọi giao diện hiển thị Menu của TV3
    // 2. Nhận lựa chọn chế độ chơi (Người vs Người hoặc Người vs Bot)
    // 3. Chuyển trạng thái: currentState = PHASE_1_PLACING;
    currentState = PHASE_1_PLACING; // Demo nhảy thẳng vào game
}

void GameEngine::handlePhase1() {
    ConsoleUI ui;
    GameRules rules;
    
    ui.drawBoard(board); // Gọi TV3 vẽ bàn cờ
    std::cout << "Luot cua Player " << currentTurn << " (Giai doan dat quan)\n";
    
    // Nhận vị trí muốn đặt quân từ người chơi (TV3)
    int targetPos = ui.getPlayerInput(board, "Chon vi tri dat quan (0-23): ");
    
    // Gọi TV2 kiểm tra luật xem vị trí đó có trống không
    if (rules.isValidMove(board, -1, targetPos, currentState)) {
        board[targetPos] = currentTurn;
        
        // Tăng số lượng quân đã đặt
        if (currentTurn == PLAYER_A) piecesPlacedA++;
        else piecesPlacedB++;
        
        // Kiểm tra xem nước đặt này có tạo thành hàng 3 không (TV2)
        if (rules.isMillCreated(board, targetPos, currentTurn)) {
            currentState = MILL_STRIKE; // Chuyển sang trạng thái ăn quân
        } else {
            // Nếu cả 2 bên đặt đủ 8 quân thì chuyển sang Phase 2 di chuyển
            if (piecesPlacedA >= 8 && piecesPlacedB >= 8) {
                currentState = PHASE_2_MOVING;
            }
            switchTurn();
        }
    } else {
        ui.printMessage("Vi tri khong hop le! Vui long chon lai.");
    }
}

void GameEngine::handlePhase2() {
    ConsoleUI ui;
    GameRules rules;
    
    ui.drawBoard(board); // Vẽ bàn cờ
    std::cout << "Luot cua Player " << currentTurn << " (Giai doan di chuyen)\n";
    
    // Nhận vị trí quân cần di chuyển (from)
    int fromPos = ui.getPlayerInput(board, "Chon vi tri quan muon di chuyen (0-23): ");
    
    // Kiểm tra vị trí này có quân của người chơi hiện tại không
    if (board[fromPos] != currentTurn) {
        ui.printMessage("Vi tri nay khong co quan cua ban! Vui long chon lai.");
        return;
    }
    
    // Nhận vị trí đích (to)
    int toPos = ui.getPlayerInput(board, "Chon vi tri muon di toi (0-23): ");
    
    // Gọi TV2 kiểm tra luật xem nước đi có hợp lệ không
    if (rules.isValidMove(board, fromPos, toPos, currentState)) {
        board[fromPos] = EMPTY;
        board[toPos] = currentTurn;
        
        // Kiểm tra xem nước đi này có tạo thành hàng 3 không (TV2)
        if (rules.isMillCreated(board, toPos, currentTurn)) {
            currentState = MILL_STRIKE; // Chuyển sang trạng thái ăn quân
        } else {
            // Kiểm tra xem game kết thúc chưa
            if (rules.isGameOver(board, currentTurn)) {
                currentState = GAME_OVER;
            } else {
                switchTurn();
            }
        }
    } else {
        ui.printMessage("Nuoc di khong hop le! Vui long chon lai.");
    }
}

void GameEngine::handleMillStrike() {
    ConsoleUI ui;
    GameRules rules;
    
    std::cout << "Chuc mung! Ban tao duoc hang 3. Chon 1 quan doi thu de xoa: \n";
    int targetPiece = ui.getPlayerInput(board, "Nhap vi tri quan doi thu (0-23): ");
    
    // Thực hiện xóa quân (cần phối hợp thêm hàm check của TV2 để không xóa nhầm quân trong Mill của đối thủ)
    Player opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
    if (board[targetPiece] == opponent) {
        board[targetPiece] = EMPTY;
        
        // Kiểm tra xem game kết thúc chưa (TV6 viết hàm check)
        if (rules.isGameOver(board, opponent)) {
            currentState = GAME_OVER;
        } else {
            // Trả về trạng thái cũ dựa trên số lượng quân đã đặt
            if (piecesPlacedA < 8 || piecesPlacedB < 8) currentState = PHASE_1_PLACING;
            else currentState = PHASE_2_MOVING;
            
            switchTurn();
        }
    }
}

void GameEngine::switchTurn() {
    currentTurn = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
}