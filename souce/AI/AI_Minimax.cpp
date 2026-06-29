#include "AI_Minimax.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace {
const int INF = 1000000000;

const int MILL_LINES[16][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {15, 16, 17},
    {18, 19, 20}, {21, 22, 23}, {0, 9, 21}, {3, 10, 18},
    {6, 11, 15}, {8, 12, 17}, {5, 13, 20}, {2, 14, 23},
    {1, 4, 7}, {16, 19, 22}, {9, 10, 11}, {12, 13, 14}
};

std::vector<int> getValidPlacements(const int board[BOARD_SIZE]) {
    std::vector<int> placements;
    for (int pos = 0; pos < BOARD_SIZE; ++pos) {
        if (GameRules::isValidPlacement(board, pos) == MoveResult::VALID) {
            placements.push_back(pos);
        }
    }
    return placements;
}

void copyBoard(const int source[BOARD_SIZE], int destination[BOARD_SIZE]) {
    std::copy(source, source + BOARD_SIZE, destination);
}
}

AIMinimax::AIMinimax(int searchDepth)
    : maxDepth(searchDepth < 1 ? 1 : searchDepth) {}

int AIMinimax::chooseHardPlacement(const int board[BOARD_SIZE],
                                   int player,
                                   int playerPiecesInHand,
                                   int opponentPiecesInHand) {
    std::vector<int> placements = getValidPlacements(board);
    if (placements.empty()) {
        return -1;
    }

    int bestPosition = placements.front();
    int bestScore = -INF;
    int opponent = opponentOf(player);

    for (int pos : placements) {
        int nextBoard[BOARD_SIZE];
        copyBoard(board, nextBoard);
        nextBoard[pos] = player;

        int nextPlayerPiecesInHand = std::max(0, playerPiecesInHand - 1);
        if (GameRules::checkMill(nextBoard, pos, player)) {
            removeBestOpponentPiece(nextBoard, player);
        }

        GamePhase nextPhase = (nextPlayerPiecesInHand == 0 && opponentPiecesInHand == 0)
            ? GamePhase::PHASE2_MOVING
            : GamePhase::PHASE1_PLACING;

        int score = minimax(nextBoard,
                            maxDepth - 1,
                            -INF,
                            INF,
                            false,
                            opponent,
                            player,
                            nextPhase,
                            nextPlayerPiecesInHand,
                            opponentPiecesInHand);

        if (score > bestScore) {
            bestScore = score;
            bestPosition = pos;
        }
    }

    return bestPosition;
}

AIMove AIMinimax::chooseHardMove(const int board[BOARD_SIZE], int player) {
    std::vector<std::pair<int, int>> moves = GameRules::getAllValidMoves(board, player);
    if (moves.empty()) {
        return AIMove();
    }

    AIMove bestMove(moves.front().first, moves.front().second);
    int bestScore = -INF;
    int opponent = opponentOf(player);

    for (const auto& move : moves) {
        int nextBoard[BOARD_SIZE];
        copyBoard(board, nextBoard);
        nextBoard[move.first] = EMPTY;
        nextBoard[move.second] = player;

        if (GameRules::checkMill(nextBoard, move.second, player)) {
            removeBestOpponentPiece(nextBoard, player);
        }

        int score = minimax(nextBoard,
                            maxDepth - 1,
                            -INF,
                            INF,
                            false,
                            opponent,
                            player,
                            GamePhase::PHASE2_MOVING,
                            0,
                            0);

        if (score > bestScore) {
            bestScore = score;
            bestMove = AIMove(move.first, move.second);
        }
    }

    return bestMove;
}

int AIMinimax::choosePieceToRemove(const int board[BOARD_SIZE], int opponent) {
    int currentPlayer = opponentOf(opponent);
    return chooseBestRemovalForPlayer(board, currentPlayer);
}

int AIMinimax::opponentOf(int player) const {
    return player == PLAYER1 ? PLAYER2 : PLAYER1;
}

int AIMinimax::getPiecesInHand(int player, int playerPiecesInHand, int opponentPiecesInHand, int aiPlayer) const {
    return player == aiPlayer ? playerPiecesInHand : opponentPiecesInHand;
}

void AIMinimax::setPiecesInHand(int player, int value, int& playerPiecesInHand, int& opponentPiecesInHand, int aiPlayer) const {
    if (player == aiPlayer) {
        playerPiecesInHand = value;
    } else {
        opponentPiecesInHand = value;
    }
}

int AIMinimax::minimax(int board[BOARD_SIZE],
                       int depth,
                       int alpha,
                       int beta,
                       bool maximizing,
                       int currentPlayer,
                       int aiPlayer,
                       GamePhase phase,
                       int playerPiecesInHand,
                       int opponentPiecesInHand) const {
    int terminalScore = evaluateTerminal(board, aiPlayer, phase);
    if (depth == 0 || terminalScore != 0 || isTerminal(board, phase)) {
        return terminalScore != 0 ? terminalScore : evaluateBoard(board, aiPlayer);
    }

    int nextPlayer = opponentOf(currentPlayer);

    if (phase == GamePhase::PHASE1_PLACING) {
        std::vector<int> placements = getValidPlacements(board);
        if (placements.empty()) {
            return evaluateBoard(board, aiPlayer);
        }

        int bestScore = maximizing ? -INF : INF;
        for (int pos : placements) {
            int currentPiecesInHand = getPiecesInHand(currentPlayer, playerPiecesInHand, opponentPiecesInHand, aiPlayer);
            if (currentPiecesInHand <= 0) {
                continue;
            }

            int nextBoard[BOARD_SIZE];
            copyBoard(board, nextBoard);
            nextBoard[pos] = currentPlayer;

            int nextPlayerPiecesInHand = playerPiecesInHand;
            int nextOpponentPiecesInHand = opponentPiecesInHand;
            setPiecesInHand(currentPlayer,
                            currentPiecesInHand - 1,
                            nextPlayerPiecesInHand,
                            nextOpponentPiecesInHand,
                            aiPlayer);

            if (GameRules::checkMill(nextBoard, pos, currentPlayer)) {
                removeBestOpponentPiece(nextBoard, currentPlayer);
            }

            GamePhase nextPhase = (nextPlayerPiecesInHand == 0 && nextOpponentPiecesInHand == 0)
                ? GamePhase::PHASE2_MOVING
                : GamePhase::PHASE1_PLACING;

            int score = minimax(nextBoard,
                                depth - 1,
                                alpha,
                                beta,
                                !maximizing,
                                nextPlayer,
                                aiPlayer,
                                nextPhase,
                                nextPlayerPiecesInHand,
                                nextOpponentPiecesInHand);

            if (maximizing) {
                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, score);
            } else {
                bestScore = std::min(bestScore, score);
                beta = std::min(beta, score);
            }

            if (beta <= alpha) {
                break;
            }
        }

        return bestScore;
    }

    std::vector<std::pair<int, int>> moves = GameRules::getAllValidMoves(board, currentPlayer);
    if (moves.empty()) {
        return currentPlayer == aiPlayer ? -500000 : 500000;
    }

    int bestScore = maximizing ? -INF : INF;
    for (const auto& move : moves) {
        int nextBoard[BOARD_SIZE];
        copyBoard(board, nextBoard);
        nextBoard[move.first] = EMPTY;
        nextBoard[move.second] = currentPlayer;

        if (GameRules::checkMill(nextBoard, move.second, currentPlayer)) {
            removeBestOpponentPiece(nextBoard, currentPlayer);
        }

        int score = minimax(nextBoard,
                            depth - 1,
                            alpha,
                            beta,
                            !maximizing,
                            nextPlayer,
                            aiPlayer,
                            GamePhase::PHASE2_MOVING,
                            playerPiecesInHand,
                            opponentPiecesInHand);

        if (maximizing) {
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
        } else {
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
        }

        if (beta <= alpha) {
            break;
        }
    }

    return bestScore;
}

int AIMinimax::evaluateBoard(const int board[BOARD_SIZE], int aiPlayer) const {
    int opponent = opponentOf(aiPlayer);
    int aiPieces = GameRules::countPieces(board, aiPlayer);
    int opponentPieces = GameRules::countPieces(board, opponent);

    int score = 0;
    score += (aiPieces - opponentPieces) * 120;
    score += (countMills(board, aiPlayer) - countMills(board, opponent)) * 90;
    score += (countPotentialMills(board, aiPlayer) - countPotentialMills(board, opponent)) * 35;
    score += (static_cast<int>(GameRules::getAllValidMoves(board, aiPlayer).size()) -
              static_cast<int>(GameRules::getAllValidMoves(board, opponent).size())) * 10;
    score += (countBlockedPieces(board, opponent) - countBlockedPieces(board, aiPlayer)) * 20;

    return score;
}

int AIMinimax::evaluateTerminal(const int board[BOARD_SIZE], int aiPlayer, GamePhase phase) const {
    int opponent = opponentOf(aiPlayer);
    int aiPieces = GameRules::countPieces(board, aiPlayer);
    int opponentPieces = GameRules::countPieces(board, opponent);

    if (phase == GamePhase::PHASE2_MOVING) {
        if (aiPieces < 3) {
            return -500000;
        }
        if (opponentPieces < 3) {
            return 500000;
        }
        if (GameRules::getAllValidMoves(board, aiPlayer).empty()) {
            return -500000;
        }
        if (GameRules::getAllValidMoves(board, opponent).empty()) {
            return 500000;
        }
    }

    return 0;
}

int AIMinimax::countMills(const int board[BOARD_SIZE], int player) const {
    int count = 0;
    for (int i = 0; i < 16; ++i) {
        if (board[MILL_LINES[i][0]] == player &&
            board[MILL_LINES[i][1]] == player &&
            board[MILL_LINES[i][2]] == player) {
            ++count;
        }
    }
    return count;
}

int AIMinimax::countPotentialMills(const int board[BOARD_SIZE], int player) const {
    int count = 0;
    for (int i = 0; i < 16; ++i) {
        int playerCount = 0;
        int emptyCount = 0;
        for (int j = 0; j < 3; ++j) {
            int value = board[MILL_LINES[i][j]];
            if (value == player) {
                ++playerCount;
            } else if (value == EMPTY) {
                ++emptyCount;
            }
        }

        if (playerCount == 2 && emptyCount == 1) {
            ++count;
        }
    }
    return count;
}

int AIMinimax::countBlockedPieces(const int board[BOARD_SIZE], int player) const {
    int blocked = 0;
    for (int pos = 0; pos < BOARD_SIZE; ++pos) {
        if (board[pos] != player) {
            continue;
        }

        std::vector<int> adjacent = GameRules::getAdjacentPositions(pos);
        bool canMove = false;
        for (int next : adjacent) {
            if (board[next] == EMPTY) {
                canMove = true;
                break;
            }
        }

        if (!canMove) {
            ++blocked;
        }
    }
    return blocked;
}

bool AIMinimax::isTerminal(const int board[BOARD_SIZE], GamePhase phase) const {
    if (phase != GamePhase::PHASE2_MOVING) {
        return false;
    }

    return GameRules::countPieces(board, PLAYER1) < 3 ||
           GameRules::countPieces(board, PLAYER2) < 3 ||
           GameRules::getAllValidMoves(board, PLAYER1).empty() ||
           GameRules::getAllValidMoves(board, PLAYER2).empty();
}

int AIMinimax::chooseBestRemovalForPlayer(const int board[BOARD_SIZE], int currentPlayer) const {
    int opponent = opponentOf(currentPlayer);
    std::vector<int> removable = GameRules::getRemovablePieces(board, opponent);
    if (removable.empty()) {
        return -1;
    }

    int bestTarget = removable.front();
    int bestScore = -INF;
    for (int pos : removable) {
        int nextBoard[BOARD_SIZE];
        copyBoard(board, nextBoard);
        nextBoard[pos] = EMPTY;

        int score = evaluateBoard(nextBoard, currentPlayer);
        if (score > bestScore) {
            bestScore = score;
            bestTarget = pos;
        }
    }

    return bestTarget;
}

void AIMinimax::removeBestOpponentPiece(int board[BOARD_SIZE], int currentPlayer) const {
    int target = chooseBestRemovalForPlayer(board, currentPlayer);
    if (target != -1) {
        board[target] = EMPTY;
    }
}
