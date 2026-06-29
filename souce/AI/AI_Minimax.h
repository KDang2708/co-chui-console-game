#ifndef AI_MINIMAX_H
#define AI_MINIMAX_H

#include "AI_EasyMed.h"
#include "../Model/GameRules.h"

class AIMinimax {
public:
    explicit AIMinimax(int searchDepth = 4);

    int chooseHardPlacement(const int board[BOARD_SIZE],
                            int player,
                            int playerPiecesInHand,
                            int opponentPiecesInHand);

    AIMove chooseHardMove(const int board[BOARD_SIZE], int player);

    int choosePieceToRemove(const int board[BOARD_SIZE], int opponent);

private:
    int maxDepth;

    int opponentOf(int player) const;
    int getPiecesInHand(int player, int playerPiecesInHand, int opponentPiecesInHand, int aiPlayer) const;
    void setPiecesInHand(int player, int value, int& playerPiecesInHand, int& opponentPiecesInHand, int aiPlayer) const;

    int minimax(int board[BOARD_SIZE],
                int depth,
                int alpha,
                int beta,
                bool maximizing,
                int currentPlayer,
                int aiPlayer,
                GamePhase phase,
                int playerPiecesInHand,
                int opponentPiecesInHand) const;

    int evaluateBoard(const int board[BOARD_SIZE], int aiPlayer) const;
    int evaluateTerminal(const int board[BOARD_SIZE], int aiPlayer, GamePhase phase) const;

    int countMills(const int board[BOARD_SIZE], int player) const;
    int countPotentialMills(const int board[BOARD_SIZE], int player) const;
    int countBlockedPieces(const int board[BOARD_SIZE], int player) const;
    bool isTerminal(const int board[BOARD_SIZE], GamePhase phase) const;

    int chooseBestRemovalForPlayer(const int board[BOARD_SIZE], int currentPlayer) const;
    void removeBestOpponentPiece(int board[BOARD_SIZE], int currentPlayer) const;
};

#endif
