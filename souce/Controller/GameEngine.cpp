#include "GameEngine.h"
#include "../View/ConsoleUI.h"
#include "../Model/GameRules.h"
#include "../GameEnd.h"
#include <iostream>

GameEngine::GameEngine() {
    initGame();
}

void GameEngine::initGame() {
    for (int i = 0; i < BOARD_SIZE; i++) board[i] = EMPTY;
    currentState = PHASE_1_PLACING;
    currentTurn = PLAYER_A;
    piecesPlacedA = 0;
    piecesPlacedB = 0;
    gameMode = 1;
    winner = EMPTY;
    lastMessage = "Chon o de bat dau. Dat quan A va B lan luot.";
}

const int* GameEngine::getBoard() const {
    return board;
}

GameState GameEngine::getCurrentState() const {
    return currentState;
}

Player GameEngine::getCurrentTurn() const {
    return currentTurn;
}

Player GameEngine::getWinner() const {
    return winner;
}

int GameEngine::getPiecesPlacedA() const {
    return piecesPlacedA;
}

int GameEngine::getPiecesPlacedB() const {
    return piecesPlacedB;
}

const std::string& GameEngine::getLastMessage() const {
    return lastMessage;
}

void GameEngine::restart() {
    initGame();
}

void GameEngine::setMessage(const std::string& message) {
    lastMessage = message;
}

MoveResult GameEngine::placePiece(int pos) {
    if (currentState != PHASE_1_PLACING && currentState != MENU) {
        setMessage("Khong the dat quan o luc nay.");
        return MoveResult::INVALID_OUT_OF_RANGE;
    }

    MoveResult result = GameRules::isValidPlacement(board, pos);
    if (result != MoveResult::VALID) {
        setMessage("Vi tri khong hop le. Chon vi tri khac.");
        return result;
    }

    board[pos] = currentTurn;
    if (currentTurn == PLAYER_A) piecesPlacedA++;
    else piecesPlacedB++;

    if (GameRules::checkMill(board, pos, currentTurn)) {
        currentState = MILL_STRIKE;
        setMessage("Tao hang 3! Chon mot quan doi thu de xoa.");
        return result;
    }

    if (piecesPlacedA >= 8 && piecesPlacedB >= 8) {
        currentState = PHASE_2_MOVING;
    } else {
        currentState = PHASE_1_PLACING;
    }

    switchTurn();
    setMessage("Nuoc di thanh cong. Den luot nguoi choi " + std::string(currentTurn == PLAYER_A ? "A" : "B") + ".");
    return result;
}

MoveResult GameEngine::movePiece(int from, int to) {
    if (currentState != PHASE_2_MOVING) {
        setMessage("Khong the di chuyen o luc nay.");
        return MoveResult::INVALID_OUT_OF_RANGE;
    }

    MoveResult result = GameRules::isValidMove(board, from, to, currentTurn);
    if (result != MoveResult::VALID) {
        setMessage("Nuoc di khong hop le. Chon o den lien ke.");
        return result;
    }

    board[to] = currentTurn;
    board[from] = EMPTY;

    if (GameRules::checkMill(board, to, currentTurn)) {
        currentState = MILL_STRIKE;
        setMessage("Tao hang 3! Chon mot quan doi thu de xoa.");
        return result;
    }

    Player winnerCandidate = EMPTY;
    if (GameEnd::isGameOver(board, currentTurn, piecesPlacedA, piecesPlacedB, GamePhase::PHASE2_MOVING, winnerCandidate)) {
        currentState = GAME_OVER;
        winner = winnerCandidate;
        setMessage(std::string("Game ket thuc. Player ") + (winner == PLAYER_A ? "A" : "B") + " chien thang!");
        return result;
    }

    switchTurn();
    setMessage("Nuoc di thanh cong. Den luot nguoi choi " + std::string(currentTurn == PLAYER_A ? "A" : "B") + ".");
    return result;
}

MoveResult GameEngine::removePiece(int pos) {
    if (currentState != MILL_STRIKE) {
        setMessage("Khong the xoa quan o luc nay.");
        return MoveResult::INVALID_OUT_OF_RANGE;
    }

    Player opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
    if (board[pos] != opponent) {
        setMessage("O nay khong phai quan doi thu. Vui long chon lai.");
        return MoveResult::INVALID_WRONG_PLAYER;
    }

    if (!GameRules::canRemovePiece(board, pos, opponent)) {
        setMessage("Khong the xoa o nay. Chon mot quan doi thu hop le.");
        return MoveResult::INVALID_OCCUPIED;
    }

    board[pos] = EMPTY;
    GameState nextState = (piecesPlacedA < 8 || piecesPlacedB < 8) ? PHASE_1_PLACING : PHASE_2_MOVING;

    Player winnerCandidate = EMPTY;
    if (GameEnd::isGameOver(board, currentTurn, piecesPlacedA, piecesPlacedB,
                            nextState == PHASE_1_PLACING ? GamePhase::PHASE1_PLACING : GamePhase::PHASE2_MOVING,
                            winnerCandidate)) {
        currentState = GAME_OVER;
        winner = winnerCandidate;
        setMessage(std::string("Game ket thuc. Player ") + (winner == PLAYER_A ? "A" : "B") + " chien thang!");
        return MoveResult::VALID;
    }

    currentState = nextState;
    switchTurn();
    setMessage("Da xoa quan. Den luot nguoi choi " + std::string(currentTurn == PLAYER_A ? "A" : "B") + ".");
    return MoveResult::VALID;
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
    
    int targetPos = ui.getPlayerInput(board, "Chon vi tri dat quan (0-23): ");
    
    if (rules.isValidPlacement(board, targetPos) == MoveResult::VALID) {
        board[targetPos] = currentTurn;
        
        if (currentTurn == PLAYER_A) piecesPlacedA++;
        else piecesPlacedB++;
        
        if (GameRules::checkMill(board, targetPos, currentTurn)) {
            currentState = MILL_STRIKE;
        } else {
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
    
    int fromPos = ui.getPlayerInput(board, "Chon vi tri quan muon di chuyen (0-23): ");
    if (board[fromPos] != currentTurn) {
        ui.printMessage("Vi tri nay khong co quan cua ban! Vui long chon lai.");
        return;
    }
    
    int toPos = ui.getPlayerInput(board, "Chon vi tri muon di toi (0-23): ");
    
    if (rules.isValidMove(board, fromPos, toPos, currentTurn) == MoveResult::VALID) {
        board[fromPos] = EMPTY;
        board[toPos] = currentTurn;
        
        if (GameRules::checkMill(board, toPos, currentTurn)) {
            currentState = MILL_STRIKE;
        } else {
            Player winner = EMPTY;
            if (GameEnd::isGameOver(board, currentTurn, piecesPlacedA, piecesPlacedB, GamePhase::PHASE2_MOVING, winner)) {
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
    
    Player opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
    if (board[targetPiece] == opponent && rules.canRemovePiece(board, targetPiece, opponent)) {
        board[targetPiece] = EMPTY;
        
        GameState nextState = (piecesPlacedA < 8 || piecesPlacedB < 8) ? PHASE_1_PLACING : PHASE_2_MOVING;
        Player winner = EMPTY;
        if (GameEnd::isGameOver(board, currentTurn, piecesPlacedA, piecesPlacedB, nextState == PHASE_1_PLACING ? GamePhase::PHASE1_PLACING : GamePhase::PHASE2_MOVING, winner)) {
            currentState = GAME_OVER;
        } else {
            currentState = nextState;
            switchTurn();
        }
    } else {
        ui.printMessage("Khong the xoa o nay hoac o nay khong hop le. Vui long chon lai.");
    }
}

void GameEngine::switchTurn() {
    currentTurn = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
}