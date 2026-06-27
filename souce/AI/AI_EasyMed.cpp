#include "AI_EasyMed.h"

#include <algorithm>

AIMove::AIMove() : from(-1), to(-1), valid(false) {}

AIMove::AIMove(int moveFrom, int moveTo)
    : from(moveFrom), to(moveTo), valid(true) {}

AIEasyMed::AIEasyMed(unsigned int seed) : rng(seed) {}

int AIEasyMed::chooseEasyPlacement(const int board[BOARD_SIZE]) {
    return chooseRandomPosition(getValidPlacements(board));
}

AIMove AIEasyMed::chooseEasyMove(const int board[BOARD_SIZE], int player) {
    return chooseRandomMove(GameRules::getAllValidMoves(board, player));
}

int AIEasyMed::chooseMediumPlacement(const int board[BOARD_SIZE], int player) {
    int winningPos = findWinningPlacement(board, player);
    if (winningPos != -1) {
        return winningPos;
    }

    int blockingPos = findBlockingPlacement(board, player);
    if (blockingPos != -1) {
        return blockingPos;
    }

    return chooseEasyPlacement(board);
}

AIMove AIEasyMed::chooseMediumMove(const int board[BOARD_SIZE], int player) {
    AIMove winningMove = findWinningMove(board, player);
    if (winningMove.valid) {
        return winningMove;
    }

    AIMove blockingMove = findBlockingMove(board, player);
    if (blockingMove.valid) {
        return blockingMove;
    }

    return chooseEasyMove(board, player);
}

int AIEasyMed::choosePieceToRemove(const int board[BOARD_SIZE], int opponent) {
    return chooseRandomPosition(GameRules::getRemovablePieces(board, opponent));
}

int AIEasyMed::opponentOf(int player) const {
    return player == PLAYER1 ? PLAYER2 : PLAYER1;
}

int AIEasyMed::randomIndex(int size) {
    if (size <= 0) {
        return -1;
    }

    std::uniform_int_distribution<int> distribution(0, size - 1);
    return distribution(rng);
}

int AIEasyMed::chooseRandomPosition(const std::vector<int>& positions) {
    int index = randomIndex(static_cast<int>(positions.size()));
    return index == -1 ? -1 : positions[index];
}

AIMove AIEasyMed::chooseRandomMove(const std::vector<std::pair<int, int>>& moves) {
    int index = randomIndex(static_cast<int>(moves.size()));
    if (index == -1) {
        return AIMove();
    }

    return AIMove(moves[index].first, moves[index].second);
}

std::vector<int> AIEasyMed::getValidPlacements(const int board[BOARD_SIZE]) const {
    std::vector<int> positions;
    for (int pos = 0; pos < BOARD_SIZE; ++pos) {
        if (GameRules::isValidPlacement(board, pos) == MoveResult::VALID) {
            positions.push_back(pos);
        }
    }
    return positions;
}

bool AIEasyMed::placementCreatesMill(const int board[BOARD_SIZE], int pos, int player) const {
    if (GameRules::isValidPlacement(board, pos) != MoveResult::VALID) {
        return false;
    }

    int testBoard[BOARD_SIZE];
    std::copy(board, board + BOARD_SIZE, testBoard);
    testBoard[pos] = player;

    return GameRules::checkMill(testBoard, pos, player);
}

bool AIEasyMed::moveCreatesMill(const int board[BOARD_SIZE], const std::pair<int, int>& move, int player) const {
    if (GameRules::isValidMove(board, move.first, move.second, player) != MoveResult::VALID) {
        return false;
    }

    int testBoard[BOARD_SIZE];
    std::copy(board, board + BOARD_SIZE, testBoard);
    testBoard[move.first] = EMPTY;
    testBoard[move.second] = player;

    return GameRules::checkMill(testBoard, move.second, player);
}

int AIEasyMed::findWinningPlacement(const int board[BOARD_SIZE], int player) const {
    std::vector<int> placements = getValidPlacements(board);
    for (int pos : placements) {
        if (placementCreatesMill(board, pos, player)) {
            return pos;
        }
    }
    return -1;
}

int AIEasyMed::findBlockingPlacement(const int board[BOARD_SIZE], int player) const {
    return findWinningPlacement(board, opponentOf(player));
}

AIMove AIEasyMed::findWinningMove(const int board[BOARD_SIZE], int player) const {
    std::vector<std::pair<int, int>> moves = GameRules::getAllValidMoves(board, player);
    for (const auto& move : moves) {
        if (moveCreatesMill(board, move, player)) {
            return AIMove(move.first, move.second);
        }
    }
    return AIMove();
}

AIMove AIEasyMed::findBlockingMove(const int board[BOARD_SIZE], int player) const {
    int opponent = opponentOf(player);
    std::vector<std::pair<int, int>> opponentMoves = GameRules::getAllValidMoves(board, opponent);
    std::vector<int> dangerousTargets;

    for (const auto& opponentMove : opponentMoves) {
        if (moveCreatesMill(board, opponentMove, opponent)) {
            dangerousTargets.push_back(opponentMove.second);
        }
    }

    if (dangerousTargets.empty()) {
        return AIMove();
    }

    std::vector<std::pair<int, int>> botMoves = GameRules::getAllValidMoves(board, player);
    for (const auto& botMove : botMoves) {
        if (std::find(dangerousTargets.begin(), dangerousTargets.end(), botMove.second) != dangerousTargets.end()) {
            return AIMove(botMove.first, botMove.second);
        }
    }

    return AIMove();
}
