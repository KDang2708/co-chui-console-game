#include "GameEnd.h"
#include "Model/GameRules.h"

bool GameEnd::isGameOver(const int board[BOARD_SIZE], 
                         Player currentPlayer,
                         int piecesPlacedA,
                         int piecesPlacedB,
                         GamePhase currentPhase,
                         Player& winner) 
{
    Player opponent = (currentPlayer == PLAYER_A) ? PLAYER_B : PLAYER_A;
    
    int oppPieces = GameRules::countPieces(board, opponent);

    // Điều kiện 1: Đối thủ còn dưới 3 quân
    if (oppPieces < 3) {
        winner = currentPlayer;
        return true;
    }

    // Điều kiện 2: Đối thủ bị chặn không còn nước đi
    if (currentPhase == GamePhase::PHASE2_MOVING) {
        auto moves = GameRules::getAllValidMoves(board, opponent);
        if (moves.empty()) {
            winner = currentPlayer;
            return true;
        }
    }

    winner = EMPTY;
    return false;
}