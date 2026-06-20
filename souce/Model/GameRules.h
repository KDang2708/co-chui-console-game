#pragma once
#include "Common.h"
#include <vector>
#include <utility>

// =============================================================
//  GameRules.h  –  Game Logic cho Cờ Chùi (Nine Men's Morris)
//  Thành viên 2 – Game Logic Specialist
//  Branch: feature/model
//
//  LƯU Ý: File này KHÔNG tự định nghĩa Player / GameState /
//  ADJACENCY_LIST / MILL_LINES nữa — toàn bộ lấy từ common.h
//  (TV1 sở hữu) để tránh trùng lặp / lỗi redefinition khi link.
// =============================================================

static const int BOARD_SIZE = 24;
static const int PIECES_PER_PLAYER = 9;

// Kết quả kiểm tra nước đi (riêng của GameRules, không đụng common.h)
enum class MoveResult {
    VALID,                  // Nước đi hợp lệ
    INVALID_OCCUPIED,       // Ô đích đã có quân
    INVALID_NOT_ADJACENT,   // Không liền kề (Phase 2, không phải Flying)
    INVALID_NO_PIECE,       // Không có quân tại ô nguồn
    INVALID_WRONG_PLAYER,   // Quân của người chơi khác
    INVALID_OUT_OF_RANGE    // Chỉ số ngoài phạm vi
};

class GameRules {
public:
    //  [PHASE 1] Kiểm tra đặt quân vào vị trí pos có hợp lệ không
    static MoveResult isValidPlacement(const int board[BOARD_SIZE], int pos);

    //  [PHASE 2 / FLYING_MODE] Kiểm tra di chuyển quân từ from → to
    //  isFlying = true → bỏ qua điều kiện liền kề (quân được "bay"
    //  tự do khi chỉ còn 3 quân, theo state FLYING_MODE trong common.h)
    static MoveResult isValidMove(const int board[BOARD_SIZE],
                                  int from, int to, int player,
                                  bool isFlying = false);

    //  Kiểm tra pos có tạo thành Mill (hàng 3) không
    static bool checkMill(const int board[BOARD_SIZE], int pos, int player);

    //  Kiểm tra ô pos của đối thủ có được phép ăn không
    //  (không ăn quân trong Mill, trừ khi toàn bộ quân đều trong Mill)
    static bool canRemovePiece(const int board[BOARD_SIZE],
                               int pos, int opponent);

    //  Kiểm tra điều kiện thua của player
    //  state: lấy trực tiếp từ enum GameState (common.h)
    static bool isLoser(const int board[BOARD_SIZE],
                        int player,
                        int piecesOnBoard,
                        int piecesInHand,
                        GameState state);

    //  Lấy tất cả vị trí liền kề của pos
    //  (Wrapper mỏng quanh ADJACENCY_LIST của common.h để các
    //   module khác không phải đụng trực tiếp vào vector lồng nhau)
    static const std::vector<int>& getAdjacentPositions(int pos);

    //  Lấy tất cả các nước đi hợp lệ của player (Phase 2 / Flying)
    static std::vector<std::pair<int,int>> getAllValidMoves(
        const int board[BOARD_SIZE], int player, bool isFlying = false);

    //  Lấy tất cả ô đối thủ có thể bị ăn
    static std::vector<int> getRemovablePieces(
        const int board[BOARD_SIZE], int opponent);

    //  Đếm số quân của player trên bàn cờ
    static int countPieces(const int board[BOARD_SIZE], int player);

    //  Alias cho isValidMove dùng trong GameEngine::handlePhase1/2.
    //  GameEngine truyền GameState thay vì player + isFlying, và
    //  dùng from = -1 để báo "đây là đặt quân" (Phase 1).
    //
    //  - state == PHASE_1_PLACING, from == -1
    //      → chuyển sang gọi isValidPlacement(board, to)
    //  - state == PHASE_2_MOVING
    //      → gọi isValidMove(board, from, to, player, isFlying=false)
    //  - state == FLYING_MODE
    //      → gọi isValidMove(board, from, to, player, isFlying=true)
    //
    //  player đi ở lượt này được suy ra từ chính board[from] khi
    //  from hợp lệ; khi from == -1 (đặt quân) cần biết người chơi
    //  hiện tại nên dùng overload có tham số player tường minh.
    static bool isValidMove(const int board[BOARD_SIZE],
                            int from, int to, GameState state, int player);

    // Overload tiện dụng: suy luận currentTurn từ board khi from != -1
    // (CHỈ dùng được ở Phase 2 / Flying, vì lúc đó from luôn có quân).
    // Ở Phase 1 (from == -1) bắt buộc dùng overload 5 tham số ở trên.
    static bool isValidMove(const int board[BOARD_SIZE],
                            int from, int to, GameState state);

    //  Alias cho checkMill — tên gọi khớp GameEngine::handlePhase1/2
    static bool isMillCreated(const int board[BOARD_SIZE], int pos, int player);

    //  Alias cho điều kiện kết thúc game — khớp chữ ký 2 tham số
    //  mà GameEngine đang gọi: rules.isGameOver(board, currentTurn)
    //
    //  Vì không có piecesInHand truyền vào, hàm tự suy luận:
    //    - Đếm quân player trên bàn (piecesOnBoard)
    //    - piecesInHand coi như 0 nếu tổng quân trên bàn của CẢ HAI
    //      bên đã đạt 18 (= 9+9, nghĩa là đã qua Phase 1)
    //    - Tự xác định state (PHASE_2_MOVING hay FLYING_MODE) dựa
    //      trên piecesOnBoard của player (== 3 → FLYING_MODE)
    //  Nếu cần chính xác tuyệt đối ở Phase 1, hãy gọi isLoser() gốc
    //  với đầy đủ piecesInHand thay vì dùng alias này.
    static bool isGameOver(const int board[BOARD_SIZE], int player);

private:
    static bool areAdjacent(int pos1, int pos2);
    static bool isMillTriple(const int board[BOARD_SIZE],
                             int a, int b, int c, int player);
};