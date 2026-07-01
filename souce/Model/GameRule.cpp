//  GameRules.cpp  –  Game Logic cho Cờ Chùi (Nine Men's Morris)
//  Thành viên 2 – Game Logic Specialist
//  Branch: feature/model
//
//  Dùng chung dữ liệu nền tảng từ common.h (sở hữu bởi TV1):
//    - enum Player        { EMPTY, PLAYER_A, PLAYER_B }
//    - enum GameState      { MENU, PHASE_1_PLACING, PHASE_2_MOVING,
//                             FLYING_MODE, MILL_STRIKE, GAME_OVER }
//    - ADJACENCY_LIST      vector<vector<int>> 24 phần tử
//    - MILL_LINES[16][3]   16 bộ ba tạo Mill

#include "GameRules.h"

static const int MILLS_COUNT = 16;

//  HÀM NỘI BỘ (PRIVATE HELPERS)

// Trả về true nếu pos1 và pos2 có cạnh nối trực tiếp (tra ADJACENCY_LIST)
bool GameRules::areAdjacent(int pos1, int pos2) {
    if (pos1 < 0 || pos1 >= BOARD_SIZE) return false;
    for (int neighbor : ADJACENCY_LIST[pos1]) {
        if (neighbor == pos2) return true;
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


//  PHASE 1 – KIỂM TRA ĐẶT QUÂN

MoveResult GameRules::isValidPlacement(const int board[BOARD_SIZE], int pos) {
    if (pos < 0 || pos >= BOARD_SIZE)
        return MoveResult::INVALID_OUT_OF_RANGE;

    if (board[pos] != EMPTY)
        return MoveResult::INVALID_OCCUPIED;

    return MoveResult::VALID;
}


//  PHASE 2 / FLYING_MODE – KIỂM TRA DI CHUYỂN QUÂN

MoveResult GameRules::isValidMove(const int board[BOARD_SIZE],
                                   int from, int to, int player,
                                   bool isFlying) {
    if (from < 0 || from >= BOARD_SIZE ||
        to   < 0 || to   >= BOARD_SIZE)
        return MoveResult::INVALID_OUT_OF_RANGE;

    if (board[from] == EMPTY)
        return MoveResult::INVALID_NO_PIECE;

    if (board[from] != player)
        return MoveResult::INVALID_WRONG_PLAYER;

    if (board[to] != EMPTY)
        return MoveResult::INVALID_OCCUPIED;

    // FLYING_MODE (chỉ còn 3 quân): được di chuyển tới bất kỳ ô trống
    // nào, bỏ qua điều kiện liền kề. Ngược lại (PHASE_2_MOVING) bắt
    // buộc liền kề theo ADJACENCY_LIST.
    if (!isFlying && !areAdjacent(from, to))
        return MoveResult::INVALID_NOT_ADJACENT;

    return MoveResult::VALID;
}


//  KIỂM TRA MILL (HÀNG 3)

bool GameRules::checkMill(const int board[BOARD_SIZE], int pos, int player) {
    for (int i = 0; i < MILLS_COUNT; ++i) {
        int a = MILL_LINES[i][0];
        int b = MILL_LINES[i][1];
        int c = MILL_LINES[i][2];

        if (pos == a || pos == b || pos == c) {
            if (isMillTriple(board, a, b, c, player))
                return true;
        }
    }
    return false;
}


//  KIỂM TRA ĂN QUÂN (REMOVE PIECE) – dùng cho state MILL_STRIKE

bool GameRules::canRemovePiece(const int board[BOARD_SIZE],
                                int pos, int opponent) {
    if (board[pos] != opponent)
        return false;

    bool inMill = checkMill(board, pos, opponent);
    if (!inMill)
        return true;

    // Ngoại lệ: nếu TẤT CẢ quân đối thủ đều trong Mill → được ăn
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == opponent && !checkMill(board, i, opponent)) {
            return false;
        }
    }
    return true;
}


//  KIỂM TRA ĐIỀU KIỆN THUA

bool GameRules::isLoser(const int board[BOARD_SIZE],
                         int player,
                         int piecesOnBoard,
                         int piecesInHand,
                         GameState state) {

    // PHASE_2_MOVING hoặc FLYING_MODE mà còn dưới 3 quân → thua
    if ((state == PHASE_2_MOVING || state == FLYING_MODE) &&
        piecesOnBoard < 3)
        return true;

    // Hết quân hoàn toàn → thua
    if (piecesOnBoard == 0 && piecesInHand == 0)
        return true;

    // Không còn nước đi hợp lệ nào → thua
    if (state == PHASE_2_MOVING || state == FLYING_MODE) {
        bool flying = (state == FLYING_MODE);
        auto moves = getAllValidMoves(board, player, flying);
        if (moves.empty())
            return true;
    }

    return false;
}


//  CÁC HÀM TIỆN ÍCH (UTILITY)

const std::vector<int>& GameRules::getAdjacentPositions(int pos) {
    return ADJACENCY_LIST[pos];
}

std::vector<std::pair<int,int>> GameRules::getAllValidMoves(
    const int board[BOARD_SIZE], int player, bool isFlying) {

    std::vector<std::pair<int,int>> moves;

    if (isFlying) {
        // FLYING_MODE: mỗi quân của player có thể tới BẤT KỲ ô trống nào
        for (int from = 0; from < BOARD_SIZE; ++from) {
            if (board[from] != player) continue;
            for (int to = 0; to < BOARD_SIZE; ++to) {
                if (board[to] == EMPTY)
                    moves.push_back({from, to});
            }
        }
    } else {
        // PHASE_2_MOVING: chỉ tới ô liền kề theo ADJACENCY_LIST
        for (int from = 0; from < BOARD_SIZE; ++from) {
            if (board[from] != player) continue;
            for (int to : ADJACENCY_LIST[from]) {
                if (board[to] == EMPTY)
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

int GameRules::countPieces(const int board[BOARD_SIZE], int player) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i] == player) ++count;
    }
    return count;
}


//  ALIAS — khớp API mà GameEngine.cpp (TV1) đang gọi trực tiếp.
//  Xem giải thích chi tiết trong GameRules.h.

bool GameRules::isValidMove(const int board[BOARD_SIZE],
                             int from, int to, GameState state, int player) {

    // Phase 1 (Đặt quân): from == -1 báo hiệu "đây không phải di
    // chuyển, mà là đặt quân mới" — chuyển sang isValidPlacement.
    if (state == PHASE_1_PLACING || from == -1) {
        return isValidPlacement(board, to) == MoveResult::VALID;
    }

    // Flying mode: bỏ qua điều kiện liền kề
    bool flying = (state == FLYING_MODE);
    return isValidMove(board, from, to, player, flying) == MoveResult::VALID;
}

bool GameRules::isValidMove(const int board[BOARD_SIZE],
                             int from, int to, GameState state) {
    // Overload không có player tường minh: chỉ an toàn khi from
    // hợp lệ và có quân (Phase 2 / Flying) — suy luận player từ
    // chính ô nguồn. KHÔNG dùng overload này ở Phase 1 (from == -1),
    // vì lúc đó board[from] không tồn tại / vô nghĩa.
    if (from < 0 || from >= BOARD_SIZE) {
        // from == -1 (đặt quân) mà gọi nhầm overload này thì không
        // thể suy ra player → coi như không hợp lệ để báo lỗi sớm
        // thay vì đọc ngoài mảng.
        return false;
    }

    int player = board[from];
    return isValidMove(board, from, to, state, player);
}

bool GameRules::isMillCreated(const int board[BOARD_SIZE], int pos, int player) {
    return checkMill(board, pos, player);
}

bool GameRules::isGameOver(const int board[BOARD_SIZE], int player) {
    int piecesOnBoard = countPieces(board, player);

    // Suy luận đã qua Phase 1 hay chưa: nếu tổng quân cả 2 bên
    // trên bàn đạt 18 (9+9) thì coi như Placing đã xong, không còn
    // quân nào "trong tay" (piecesInHand = 0). Nếu chưa, coi như
    // vẫn đang Phase 1 (piecesInHand > 0) → chưa thể thua kiểu
    // "hết quân" hay "bị vây" theo luật Phase 2.
    int totalOnBoard = countPieces(board, PLAYER_A) + countPieces(board, PLAYER_B);
    bool placingDone = (totalOnBoard >= PIECES_PER_PLAYER * 2 - /*đã ăn bớt*/0)
                        || (totalOnBoard >= 18);

    if (!placingDone) {
        // Vẫn còn trong Phase 1 theo suy luận → dùng isLoser với
        // piecesInHand ước lượng còn lại để không báo thua nhầm.
        int piecesInHand = PIECES_PER_PLAYER - piecesOnBoard;
        if (piecesInHand < 0) piecesInHand = 0;
        return isLoser(board, player, piecesOnBoard, piecesInHand,
                       PHASE_1_PLACING);
    }

    // Đã qua Phase 1: piecesInHand = 0. Xác định FLYING_MODE nếu
    // đúng còn 3 quân, ngược lại PHASE_2_MOVING.
    GameState state = (piecesOnBoard == 3) ? FLYING_MODE : PHASE_2_MOVING;
    return isLoser(board, player, piecesOnBoard, 0, state);
}