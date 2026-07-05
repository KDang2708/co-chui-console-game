#ifndef GAMERULES_H
#define GAMERULES_H

#include <vector>
#include "Common.h"

// =============================================================
//  GameRules.h  –  Game Logic cho Cờ Chùi (Nine Men's Morris)
//  Game Logic Specialist
//  Branch: feature/model
// =============================================================

//  HẰNG SỐ & CẤU TRÚC DỮ LIỆU TOÀN CỤC

// Tổng số quân cờ mỗi người chơi
static const int PLAYER1 = PLAYER_A;
static const int PLAYER2 = PLAYER_B;

// Trạng thái game (dùng chung với GameEngine)
enum class GamePhase {
    PHASE1_PLACING,   // Giai đoạn 1: Đặt quân
    PHASE2_MOVING,    // Giai đoạn 2: Di chuyển quân
    REMOVING_PIECE,   // Đang ăn quân đối thủ (sau khi tạo Mill)
    GAME_OVER         // Kết thúc
};

// Kết quả kiểm tra nước đi
enum class MoveResult {
    VALID,            // Nước đi hợp lệ
    INVALID_OCCUPIED, // Ô đích đã có quân
    INVALID_NOT_ADJACENT, // Không liền kề (Phase 2)
    INVALID_NO_PIECE, // Không có quân tại ô nguồn
    INVALID_WRONG_PLAYER, // Quân của người chơi khác
    INVALID_OUT_OF_RANGE  // Chỉ số ngoài phạm vi
};

// ─────────────────────────────────────────────────────────────
//  CLASS GAMERULES
// ─────────────────────────────────────────────────────────────
class GameRules {
public:
    // ----------------------------------------------------------
    //  [PHASE 1] Kiểm tra đặt quân vào vị trí pos có hợp lệ không
    //  board : mảng trạng thái bàn cờ hiện tại
    //  pos   : chỉ số ô muốn đặt (0..23)
    //  Trả về MoveResult
    // ----------------------------------------------------------
    static MoveResult isValidPlacement(const int board[BOARD_SIZE], int pos);

    // ----------------------------------------------------------
    //  [PHASE 2] Kiểm tra di chuyển quân từ from → to
    //  board  : mảng trạng thái bàn cờ hiện tại
    //  from   : ô nguồn (phải có quân của player)
    //  to     : ô đích  (phải trống & liền kề from)
    //  player : PLAYER1 hoặc PLAYER2
    //  Trả về MoveResult
    // ----------------------------------------------------------
    static MoveResult isValidMove(const int board[BOARD_SIZE],
                                  int from, int to, int player);

    // ----------------------------------------------------------
    //  Kiểm tra pos có tạo thành Mill (hàng 3) không
    //  board  : bàn cờ SAU KHI đặt/di chuyển quân vào pos
    //  pos    : vị trí vừa đặt/di chuyển đến
    //  player : PLAYER1 hoặc PLAYER2
    //  Trả về true nếu tạo ít nhất 1 Mill
    // ----------------------------------------------------------
    static bool checkMill(const int board[BOARD_SIZE], int pos, int player);

    // ----------------------------------------------------------
    //  Kiểm tra ô pos của đối thủ có được phép ăn không
    //  Theo luật: không được ăn quân đang nằm trong Mill,
    //             TRỪ KHI toàn bộ quân đối thủ đều trong Mill
    //  board    : bàn cờ hiện tại
    //  pos      : ô muốn ăn
    //  opponent : PLAYER1 hoặc PLAYER2 (người bị ăn)
    //  Trả về true nếu được phép ăn
    // ----------------------------------------------------------
    static bool canRemovePiece(const int board[BOARD_SIZE],
                               int pos, int opponent);

    // ----------------------------------------------------------
    //  Kiểm tra điều kiện thua của player
    //  Thua khi: quân < 3  HOẶC  không còn nước đi hợp lệ
    //  board       : bàn cờ hiện tại
    //  player      : người chơi cần kiểm tra
    //  piecesOnBoard : số quân còn trên bàn
    //  piecesInHand  : số quân chưa đặt (Phase 1)
    //  phase       : giai đoạn hiện tại
    //  Trả về true nếu player thua
    // ----------------------------------------------------------
    static bool isLoser(const int board[BOARD_SIZE],
                        int player,
                        int piecesOnBoard,
                        int piecesInHand,
                        GamePhase phase);

    // ----------------------------------------------------------
    //  Lấy tất cả vị trí liền kề của pos (dùng cho AI & UI)
    // ----------------------------------------------------------
    static std::vector<int> getAdjacentPositions(int pos);

    // ----------------------------------------------------------
    //  Lấy tất cả các nước đi hợp lệ Phase 2 của player
    //  Trả về vector các cặp {from, to}
    // ----------------------------------------------------------
    static std::vector<std::pair<int,int>> getAllValidMoves(
        const int board[BOARD_SIZE], int player);

    // ----------------------------------------------------------
    //  Lấy tất cả ô đối thủ có thể bị ăn
    // ----------------------------------------------------------
    static std::vector<int> getRemovablePieces(
        const int board[BOARD_SIZE], int opponent);

    // ----------------------------------------------------------
    //  Đếm số quân của player trên bàn cờ
    // ----------------------------------------------------------
    static int countPieces(const int board[BOARD_SIZE], int player);

private:
    // ----------------------------------------------------------
    //  DỮ LIỆU BÀNG CẠNH – Danh sách kề (Adjacency List)
    //  Định nghĩa các cặp ô liền kề có đường nối trực tiếp
    //  trên bàn cờ Nine Men's Morris chuẩn
    //
    //  Sơ đồ đánh số ô:
    //
    //   0 ─────────── 1 ─────────── 2
    //   │             │             │
    //   │   3 ─────── 4 ─────── 5   │
    //   │   │         │         │   │
    //   │   │   6 ─── 7 ─── 8   │   │
    //   9 ─10 ──11         12 ──13 ─14
    //   │   │   15 ──16 ──17    │   │
    //   │   │         │         │   │
    //   │   18 ───── 19 ───── 20    │
    //   │             │             │
    //   21 ────────  22 ─────────── 23
    //
    // ----------------------------------------------------------
    static const int ADJACENCY[][2];
    static const int ADJACENCY_COUNT;

    // Tất cả các bộ ba tạo Mill
    static const int MILLS[][3];
    static const int MILLS_COUNT;

    // Hàm nội bộ: kiểm tra pos1 và pos2 có liền kề không
    static bool areAdjacent(int pos1, int pos2);

    // Hàm nội bộ: kiểm tra một bộ ba có phải Mill của player không
    static bool isMillTriple(const int board[BOARD_SIZE],
                             int a, int b, int c, int player);
};

#endif // GAMERULES_H