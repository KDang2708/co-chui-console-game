#include "GameRules.h"
#include <algorithm>

// Board adjacency and mill definitions for Nine Men's Morris.
const int GameRules::ADJACENCY[][2] = {
    // Vòng ngoài (vòng số 1) – ngang
    {0, 1}, {1, 2},
    {3, 4}, {4, 5},
    {6, 7}, {7, 8},
    {15,16},{16,17},
    {18,19},{19,20},
    {21,22},{22,23},
    // Vòng ngoài – dọc
    {0, 9}, {9,21},
    {3,10},{10,18},
    {6,11},{11,15},
    {8,12},{12,17},
    {5,13},{13,20},
    {2,14},{14,23},
    // Nối giữa các vòng – cạnh giữa bàn
    {1, 4}, {4, 7},
    {9,10},{10,11},
    {12,13},{13,14},
    {16,19},{19,22}
};
const int GameRules::ADJACENCY_COUNT =
    sizeof(GameRules::ADJACENCY) / sizeof(GameRules::ADJACENCY[0]);

// Mill lines.
const int GameRules::MILLS[][3] = {
    // Hàng ngang – vòng ngoài
    { 0,  1,  2},
    { 3,  4,  5},
    { 6,  7,  8},
    {15, 16, 17},
    {18, 19, 20},
    {21, 22, 23},
    // Hàng dọc – vòng ngoài
    { 0,  9, 21},
    { 3, 10, 18},
    { 6, 11, 15},
    { 8, 12, 17},
    { 5, 13, 20},
    { 2, 14, 23},
    // Hàng ngang – vòng giữa (nối các vòng)
    { 1,  4,  7},
    {16, 19, 22},
    // Hàng dọc – vòng giữa
    { 9, 10, 11},
    {12, 13, 14}
};
const int GameRules::MILLS_COUNT =
    sizeof(GameRules::MILLS) / sizeof(GameRules::MILLS[0]);


// Internal helpers.
bool GameRules::areAdjacent(int pos1, int pos2) {
    for (int i = 0; i < ADJACENCY_COUNT; ++i) {
        if ((ADJACENCY[i][0] == pos1 && ADJACENCY[i][1] == pos2) ||
            (ADJACENCY[i][0] == pos2 && ADJACENCY[i][1] == pos1)) {
            return true;
        }
    }
    return false;
}

// Trả về true nếu bộ ba {a,b,c} đều là quân của player
bool GameRules::isMillTriple(const int board[BOARD_SIZE],
                              int a, int b, int c, int player) {
    return (board[a] == player &&
            board[b] == player &&
            board[c] == player);
}

MoveResult GameRules::isValidPlacement(const int board[BOARD_SIZE], int pos) {
    if (pos < 0 || pos >= BOARD_SIZE)
        return MoveResult::INVALID_OUT_OF_RANGE;

    if (board[pos] != EMPTY)
        return MoveResult::INVALID_OCCUPIED;

    return MoveResult::VALID;
}


MoveResult GameRules::isValidMove(const int board[BOARD_SIZE],
                                   int from, int to, int player) {
    if (from < 0 || from >= BOARD_SIZE ||
        to   < 0 || to   >= BOARD_SIZE)
        return MoveResult::INVALID_OUT_OF_RANGE;

    if (board[from] == EMPTY)
        return MoveResult::INVALID_NO_PIECE;

    if (board[from] != player)
        return MoveResult::INVALID_WRONG_PLAYER;

    if (board[to] != EMPTY)
        return MoveResult::INVALID_OCCUPIED;

    if (!areAdjacent(from, to))
        return MoveResult::INVALID_NOT_ADJACENT;

    return MoveResult::VALID;
}


bool GameRules::checkMill(const int board[BOARD_SIZE], int pos, int player) {
    for (int i = 0; i < MILLS_COUNT; ++i) {
        int a = MILLS[i][0];
        int b = MILLS[i][1];
        int c = MILLS[i][2];

        if (pos == a || pos == b || pos == c) {
            if (isMillTriple(board, a, b, c, player))
                return true;
        }
    }
    return false;
}

bool GameRules::canRemovePiece(const int board[BOARD_SIZE],
                                int pos, int opponent) {
    if (board[pos] != opponent)
        return false;

    bool inMill = checkMill(board, pos, opponent);
    if (!inMill)
        return true;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == opponent && !checkMill(board, i, opponent)) {
            return false;
        }
    }

    return true;
}


bool GameRules::isLoser(const int board[BOARD_SIZE],
                         int player,
                         int piecesOnBoard,
                         int piecesInHand,
                         GamePhase phase) {
    if (phase == GamePhase::PHASE2_MOVING && piecesOnBoard < 3)
        return true;

    if (piecesOnBoard == 0 && piecesInHand == 0)
        return true;

    if (phase == GamePhase::PHASE2_MOVING) {
        auto moves = getAllValidMoves(board, player);
        if (moves.empty())
            return true;
    }

    return false;
}

std::vector<int> GameRules::getAdjacentPositions(int pos) {
    std::vector<int> result;
    if (pos < 0 || pos >= BOARD_SIZE) return result;

    for (int i = 0; i < ADJACENCY_COUNT; ++i) {
        if (ADJACENCY[i][0] == pos) result.push_back(ADJACENCY[i][1]);
        if (ADJACENCY[i][1] == pos) result.push_back(ADJACENCY[i][0]);
    }
    return result;
}

std::vector<std::pair<int,int>> GameRules::getAllValidMoves(
    const int board[BOARD_SIZE], int player) {

    std::vector<std::pair<int,int>> moves;

    for (int from = 0; from < BOARD_SIZE; ++from) {
        if (board[from] != player) continue;

        auto neighbors = getAdjacentPositions(from);
        for (int to : neighbors) {
            if (board[to] == EMPTY) {
                moves.push_back({from, to});
            }
        }
    }
    return moves;
}

std::vector<int> GameRules::getRemovablePieces(
    const int board[BOARD_SIZE], int opponent) {

    std::vector<int> removable;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (canRemovePiece(board, i, opponent))
            removable.push_back(i);
    }
    return removable;
}

// Đếm số quân của player còn trên bàn
int GameRules::countPieces(const int board[BOARD_SIZE], int player) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == player) ++count;
    }
    return count;
}