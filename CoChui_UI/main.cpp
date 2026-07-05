#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <cfloat>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GLFW/glfw3.h>
#include "../souce/Model/GameRules.h"
#include "../souce/Controller/GameEngine.h"
#include "../souce/AI/AI_EasyMed.h"
#include "../souce/AI/AI_Minimax.h"
#include "../souce/Model/Common.h"

static const int POSITION_INDEX[7][7] = {
    { 0, -1, -1,  1, -1, -1,  2},
    {-1,  3, -1,  4, -1,  5, -1},
    {-1, -1,  6,  7,  8, -1, -1},
    { 9, 10, 11, -1, 12, 13, 14},
    {-1, -1, 15, 16, 17, -1, -1},
    {-1, 18, -1, 19, -1, 20, -1},
    {21, -1, -1, 22, -1, -1, 23}
};

int gameMode = 0;       
int aiDifficulty = 0;   
bool showMainMenu = true; 

static const char* playerName(int player) { return player == PLAYER_A ? "Player A (Do)" : player == PLAYER_B ? "Player B (Xanh)" : "?"; }
static const char* pieceSymbol(int player) { return player == PLAYER_A ? "X" : player == PLAYER_B ? "O" : " "; }

static ImVec2 getNodePos(int row, int col, const ImVec2& origin, float spacing) {
    return ImVec2(origin.x + col * spacing, origin.y + row * spacing);
}

static int getBoardIndex(int row, int col) { return POSITION_INDEX[row][col]; }

// =========================================================================
// AI LOGIC
// =========================================================================
void performAIMove(GameEngine& engine) {
    static AIEasyMed mediumAI(static_cast<unsigned int>(time(nullptr)));
    static AIMinimax hardAI(5);
    const int* board = engine.getBoard();
    GameState state = engine.getCurrentState();
    int currentTurn = engine.getCurrentTurn();

    if (state == PHASE_1_PLACING) {
        int choice = -1;
        if (aiDifficulty == 0) {
            std::vector<int> empties;
            for (int i = 0; i < BOARD_SIZE; ++i) if (board[i] == EMPTY) empties.push_back(i);
            if (empties.empty()) return;
            choice = empties[rand() % empties.size()];
        } else if (aiDifficulty == 1) {
            choice = mediumAI.chooseMediumPlacement(board, PLAYER_B);
            if (choice == -1) {
                std::vector<int> empties;
                for (int i = 0; i < BOARD_SIZE; ++i) if (board[i] == EMPTY) empties.push_back(i);
                if (empties.empty()) return;
                choice = empties[rand() % empties.size()];
            }
        } else {
            int playerPiecesInHand = PIECES_PER_PLAYER - engine.getPiecesPlacedB();
            int opponentPiecesInHand = PIECES_PER_PLAYER - engine.getPiecesPlacedA();
            choice = hardAI.chooseHardPlacement(board, PLAYER_B, playerPiecesInHand, opponentPiecesInHand);
        }
        if (choice != -1) engine.placePiece(choice);
    } else if (state == PHASE_2_MOVING) {
        if (aiDifficulty == 0) {
            std::vector<std::pair<int, int>> validMoves;
            for (int i = 0; i < BOARD_SIZE; ++i) {
                if (board[i] == PLAYER_B) {
                    for (int adj : ADJACENCY_LIST[i]) {
                        if (board[adj] == EMPTY) validMoves.push_back({i, adj});
                    }
                }
            }
            if (validMoves.empty()) return;
            auto move = validMoves[rand() % validMoves.size()];
            engine.movePiece(move.first, move.second);
        } else if (aiDifficulty == 1) {
            AIMove move = mediumAI.chooseMediumMove(board, PLAYER_B);
            if (move.valid) {
                engine.movePiece(move.from, move.to);
            } else {
                std::vector<std::pair<int, int>> validMoves;
                for (int i = 0; i < BOARD_SIZE; ++i) {
                    if (board[i] == PLAYER_B) {
                        for (int adj : ADJACENCY_LIST[i]) {
                            if (board[adj] == EMPTY) validMoves.push_back({i, adj});
                        }
                    }
                }
                if (validMoves.empty()) return;
                auto fallback = validMoves[rand() % validMoves.size()];
                engine.movePiece(fallback.first, fallback.second);
            }
        } else {
            AIMove move = hardAI.chooseHardMove(board, PLAYER_B);
            if (move.valid) {
                engine.movePiece(move.from, move.to);
            }
        }
    } else if (state == MILL_STRIKE) {
        int opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
        std::vector<int> removable = GameRules::getRemovablePieces(board, opponent);
        if (removable.empty()) return;
        int choice = -1;
        if (aiDifficulty == 2) {
            choice = hardAI.choosePieceToRemove(board, opponent);
            if (choice == -1) {
                choice = removable[rand() % removable.size()];
            }
        } else {
            choice = removable[rand() % removable.size()];
        }
        engine.removePiece(choice);
    }
}

// =========================================================================
// HÀM VẼ HIỆU ỨNG ĐƯỜNG KẺ
// =========================================================================
void DrawCleanLine(ImDrawList* drawList, const ImVec2& p1, const ImVec2& p2, ImU32 color, float thickness) {
    ImU32 glowColor = (color & 0x00FFFFFF) | 0x30000000; 
    drawList->AddLine(p1, p2, glowColor, thickness * 2.0f);
    drawList->AddLine(p1, p2, color, thickness);
}

// =========================================================================
// MAIN
// =========================================================================
int main(int, char**) {
    srand(static_cast<unsigned int>(time(0))); 
    if (!glfwInit()) return 1;

    // THU HẸP CHIỀU RỘNG CỬA SỔ XUỐNG 600
    GLFWwindow* window = glfwCreateWindow(600, 780, "DO AN CO CHUI - NHOM 04", NULL, NULL);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 12.0f;
    style.ItemSpacing = ImVec2(10, 10);
    style.FramePadding = ImVec2(10, 8);
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.08f, 0.98f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.09f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.95f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.25f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.45f, 0.85f, 0.80f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.55f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.35f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.15f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.32f, 1.00f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GameEngine engine;
    engine.restart();
    int selectedSource = -1;
    float aiTimer = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float currentTime = ImGui::GetTime(); 
        const int* board = engine.getBoard();
        GameState currentState = engine.getCurrentState();
        Player currentTurn = engine.getCurrentTurn();
        Player winner = engine.getWinner();
        std::string message = engine.getLastMessage();

        // ==========================================
        // WINDOW 1: MENU CHÍNH 
        // ==========================================
        if (showMainMenu) {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(450, 420), ImGuiCond_Always);
            
            ImGui::Begin("MENU CHINH - NHOM 04", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            
            ImGui::Dummy(ImVec2(0, 10));
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "  CHAO MUNG DEN VOI GAME CO CHUI - NHOM 04");
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 20));

            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "  [ CAU HINH TRAN DAU ]");
            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Indent(20.0f);
            ImGui::RadioButton("PvP (Nguoi vs Nguoi)", &gameMode, 0);
            ImGui::Dummy(ImVec2(0, 5));
            ImGui::RadioButton("PvE (Nguoi vs AI)", &gameMode, 1);
            if (gameMode == 1) {
                ImGui::Indent(30.0f);
                ImGui::Dummy(ImVec2(0, 5));
                ImGui::RadioButton("AI De (Ngau nhien)", &aiDifficulty, 0);
                ImGui::Dummy(ImVec2(0, 5));
                ImGui::RadioButton("AI Trung Binh (An/Chan)", &aiDifficulty, 1);
                ImGui::Dummy(ImVec2(0, 5));
                ImGui::RadioButton("AI Kho (Minimax + Alpha-Beta)", &aiDifficulty, 2);
                ImGui::Unindent(30.0f);
            }
            ImGui::Unindent(20.0f);
            
            ImGui::Dummy(ImVec2(0, 30));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 15));

            if (ImGui::Button("BAT DAU TRAN DAU", ImVec2(-FLT_MIN, 50))) {
                engine.restart();
                selectedSource = -1;
                aiTimer = 0.0f;
                showMainMenu = false; 
            }

            ImGui::End();
        } 
        // ==========================================
        // WINDOW 2: BÀN CỜ CHÍNH (ĐÃ XÓA CỘT BÊN PHẢI)
        // ==========================================
        else {
            if (gameMode == 1 && currentTurn == PLAYER_B && currentState != GAME_OVER && winner == EMPTY) {
                aiTimer += io.DeltaTime;
                if (aiTimer > 0.8f) { 
                    performAIMove(engine);
                    aiTimer = 0.0f;
                    board = engine.getBoard();
                    currentState = engine.getCurrentState();
                    currentTurn = engine.getCurrentTurn();
                    winner = engine.getWinner();
                    message = engine.getLastMessage();
                }
            } else {
                aiTimer = 0.0f;
            }

            ImGui::SetNextWindowSize(ImVec2(600, 780), ImGuiCond_Always); // Thu nhỏ khung
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::Begin("CO CHUI - NHOM 04", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "DO AN CO CHUI - NHOM 04");
            ImGui::Separator();

            // VẼ BÀN CỜ (CHIẾM TRỌN BỀ NGANG CỬA SỔ)
            ImGui::BeginChild("BoardArea", ImVec2(0, 560), true, ImGuiWindowFlags_NoScrollbar);
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 boardOrigin = ImGui::GetCursorScreenPos();
            
            // Canh giữa bàn cờ
            float spacing = 75.0f;
            float nodeRadius = 16.0f; 
            ImVec2 gridOffset = ImVec2(40.0f, 30.0f); // Tinh chỉnh để cân giữa cửa sổ 600px

            ImVec2 backgroundMin = ImVec2(boardOrigin.x + 15, boardOrigin.y + 5);
            ImVec2 backgroundMax = ImVec2(boardOrigin.x + spacing * 6 + 65.0f, boardOrigin.y + spacing * 6 + 55.0f);
            drawList->AddRectFilledMultiColor(backgroundMin, backgroundMax, 
                IM_COL32(15, 20, 35, 255), IM_COL32(10, 15, 25, 255), 
                IM_COL32(10, 15, 25, 255), IM_COL32(20, 25, 45, 255));
            drawList->AddRect(backgroundMin, backgroundMax, IM_COL32(50, 150, 255, 80), 12.0f, 0, 1.5f);

            ImU32 gridColor = IM_COL32(0, 180, 255, 180);
            float lineThickness = 2.0f; 
            ImVec2 boardOffset = ImVec2(boardOrigin.x + gridOffset.x, boardOrigin.y + gridOffset.y);
            
            for (int ring = 0; ring < 3; ++ring) {
                int offset = ring; int size = 6 - ring * 2;
                ImVec2 topLeft = getNodePos(offset, offset, boardOffset, spacing);
                ImVec2 bottomRight = getNodePos(offset + size, offset + size, boardOffset, spacing);
                DrawCleanLine(drawList, topLeft, ImVec2(bottomRight.x, topLeft.y), gridColor, lineThickness);
                DrawCleanLine(drawList, ImVec2(bottomRight.x, topLeft.y), bottomRight, gridColor, lineThickness);
                DrawCleanLine(drawList, bottomRight, ImVec2(topLeft.x, bottomRight.y), gridColor, lineThickness);
                DrawCleanLine(drawList, ImVec2(topLeft.x, bottomRight.y), topLeft, gridColor, lineThickness);
            }
            DrawCleanLine(drawList, getNodePos(0, 3, boardOffset, spacing), getNodePos(2, 3, boardOffset, spacing), gridColor, lineThickness);
            DrawCleanLine(drawList, getNodePos(4, 3, boardOffset, spacing), getNodePos(6, 3, boardOffset, spacing), gridColor, lineThickness);
            DrawCleanLine(drawList, getNodePos(3, 0, boardOffset, spacing), getNodePos(3, 2, boardOffset, spacing), gridColor, lineThickness);
            DrawCleanLine(drawList, getNodePos(3, 4, boardOffset, spacing), getNodePos(3, 6, boardOffset, spacing), gridColor, lineThickness);

            std::vector<int> removablePositions;
            if (currentState == MILL_STRIKE) {
                int opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
                removablePositions = GameRules::getRemovablePieces(board, opponent == PLAYER_A ? PLAYER1 : PLAYER2);
            }

            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 7; ++col) {
                    int index = getBoardIndex(row, col);
                    if (index < 0) continue;

                    ImVec2 center = getNodePos(row, col, boardOffset, spacing);
                    ImGui::PushID(index);
                    ImGui::SetCursorScreenPos(ImVec2(center.x - nodeRadius, center.y - nodeRadius));
                    ImGui::InvisibleButton("node", ImVec2(nodeRadius * 2.0f, nodeRadius * 2.0f));
                    
                    bool hovered = ImGui::IsItemHovered();
                    bool pressed = ImGui::IsItemClicked();
                    bool isSelected = (selectedSource == index);
                    int value = board[index];
                    bool isRemovable = std::find(removablePositions.begin(), removablePositions.end(), index) != removablePositions.end();

                    float pulse = (sin(currentTime * 6.0f) + 1.0f) * 0.5f; 
                    
                    ImU32 baseColor = IM_COL32(35, 50, 75, 255); 
                    if (value == PLAYER_A) baseColor = IM_COL32(230, 40, 60, 255); 
                    else if (value == PLAYER_B) baseColor = IM_COL32(0, 200, 120, 255); 
                    else if (hovered) baseColor = IM_COL32(255, 200, 50, 200); 

                    if (isSelected || isRemovable) {
                        ImU32 haloColor = isRemovable ? IM_COL32(255, 100, 0, (int)(100 + 80*pulse)) : IM_COL32(255, 255, 255, (int)(80 + 80*pulse));
                        drawList->AddCircleFilled(center, nodeRadius + 4.0f + 1.0f*pulse, haloColor);
                    }

                    if (value == EMPTY && !hovered) {
                        drawList->AddCircleFilled(center, nodeRadius * 0.35f, baseColor);
                    } else {
                        drawList->AddCircleFilled(center, nodeRadius, IM_COL32(10, 10, 15, 200)); 
                        drawList->AddCircleFilled(center, nodeRadius - 1.5f, baseColor); 
                        drawList->AddCircleFilled(ImVec2(center.x, center.y - nodeRadius*0.3f), nodeRadius * 0.4f, IM_COL32(255, 255, 255, 50)); 
                        drawList->AddText(ImVec2(center.x - 4.5f, center.y - 7.0f), IM_COL32(25, 25, 25, 255), pieceSymbol(value));
                    }

                    bool isPlayerTurn = (currentTurn == PLAYER_A) || (gameMode == 0 && currentTurn == PLAYER_B);
                    if (pressed && currentState != GAME_OVER && isPlayerTurn) {
                        if (currentState == PHASE_1_PLACING) {
                            if (engine.placePiece(index) == MoveResult::VALID) selectedSource = -1;
                        } 
                        else if (currentState == PHASE_2_MOVING) {
                            if (selectedSource < 0) {
                                if (board[index] == currentTurn) selectedSource = index;
                            } else if (index == selectedSource) {
                                selectedSource = -1;
                            } else {
                                if (engine.movePiece(selectedSource, index) == MoveResult::VALID) selectedSource = -1;
                            }
                        } 
                        else if (currentState == MILL_STRIKE) {
                            if (engine.removePiece(index) == MoveResult::VALID) selectedSource = -1;
                        }
                    }
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
            
            // ==========================================
            // CÁC THÔNG TIN VÀ NÚT BẤM DƯỚI BÀN CỜ
            // ==========================================
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImVec4 turnColor = (currentTurn == PLAYER_A) ? ImVec4(1.0f, 0.2f, 0.3f, 1.0f) : ImVec4(0.0f, 0.9f, 0.6f, 1.0f);
            
            ImGui::TextWrapped(">>> %s", message.c_str());
            ImGui::Text("Luot hien tai:"); ImGui::SameLine();
            ImGui::TextColored(turnColor, "[ %s ]", playerName(currentTurn));
            
            if (gameMode == 1 && currentTurn == PLAYER_B && currentState != GAME_OVER) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "- AI dang tinh toan...");
            }
            
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            
            // Căn giữa 2 nút bấm
            float buttonWidth = 270.0f;
            float space = (ImGui::GetWindowWidth() - (buttonWidth * 2 + 10)) / 2;
            ImGui::SetCursorPosX(space);
            
            if (ImGui::Button(" LAM MOI TRAN DAU ", ImVec2(buttonWidth, 45))) {
                engine.restart();
                selectedSource = -1;
                aiTimer = 0.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button(" QUAY LAI MENU ", ImVec2(buttonWidth, 45))) {
                showMainMenu = true; 
            }

            // ==========================================
            // POPUP KẾT THÚC GAME
            // ==========================================
            if (currentState == GAME_OVER || winner != EMPTY) {
                ImGui::OpenPopup(" KET QUA ");
            }

            ImVec2 centerPopup = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(centerPopup, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.15f, 0.98f));
            if (ImGui::BeginPopupModal(" KET QUA ", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Dummy(ImVec2(0, 10));
                if (winner == PLAYER_A) {
                    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.4f, 1.0f), "  TUYET VOI! NGUOI CHOI DO (A) DA GIANH CHIEN THANG!  ");
                } else if (winner == PLAYER_B) {
                    if (gameMode == 1) ImGui::TextColored(ImVec4(0.4f, 0.4f, 1.0f, 1.0f), "  BAN DA BI AI DANH BAI. HAY PHUC THU!  ");
                    else ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.5f, 1.0f), "  TUYET VOI! NGUOI CHOI XANH (B) DA GIANH CHIEN THANG!  ");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "  TRAN DAU KET THUC VOI KET QUA HOA!  ");
                }
                ImGui::Dummy(ImVec2(0, 15));
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 10));

                if (ImGui::Button("CHOI LAI NGAY", ImVec2(350, 45))) {
                    engine.restart();
                    selectedSource = -1;
                    aiTimer = 0.0f;
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("QUAY LAI MENU", ImVec2(350, 45))) {
                    showMainMenu = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::PopStyleColor();

            ImGui::End();
        } 

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}