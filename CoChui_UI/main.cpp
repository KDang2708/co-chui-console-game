#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <cfloat>
#include <string>
#include <vector>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "../souce/Model/GameRules.h"
#include "../souce/Controller/GameEngine.h"
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

static const char* playerName(int player) {
    return player == PLAYER_A ? "A" : player == PLAYER_B ? "B" : "?";
}

static const char* pieceSymbol(int player) {
    return player == PLAYER_A ? "X" : player == PLAYER_B ? "O" : " ";
}

static const char* getStateLabel(GameState state) {
    switch (state) {
        case PHASE_1_PLACING: return "Giai doan 1: Dat quan";
        case PHASE_2_MOVING: return "Giai doan 2: Di chuyen";
        case MILL_STRIKE: return "Tao hang 3! Xoa quan doi thu";
        case GAME_OVER: return "Game ket thuc";
        default: return "Trang thai dang cho";
    }
}

static GamePhase toGamePhase(GameState state) {
    switch (state) {
        case PHASE_1_PLACING: return GamePhase::PHASE1_PLACING;
        case PHASE_2_MOVING: return GamePhase::PHASE2_MOVING;
        case MILL_STRIKE: return GamePhase::REMOVING_PIECE;
        default: return GamePhase::PHASE2_MOVING;
    }
}

static ImVec2 getNodePos(int row, int col, const ImVec2& origin, float spacing) {
    return ImVec2(origin.x + col * spacing, origin.y + row * spacing);
}

static int getBoardIndex(int row, int col) {
    return POSITION_INDEX[row][col];
}

static void resetGame(int board[BOARD_SIZE], int& currentTurn, GameState& currentState,
                      int& piecesPlacedA, int& piecesPlacedB, int& selectedSource,
                      Player& winner, std::string& message) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        board[i] = EMPTY;
    }
    currentTurn = PLAYER_A;
    currentState = PHASE_1_PLACING;
    piecesPlacedA = 0;
    piecesPlacedB = 0;
    selectedSource = -1;
    winner = EMPTY;
    message = "Chon o de bat dau. Dat quan A va B lan luot.";
}

int main(int, char**) {
    if (!glfwInit()) {
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 760, "DO AN CO CHUI - GIAO DIEN DEP", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 16.0f;
    style.FrameRounding = 10.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.10f, 0.18f, 0.96f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.24f, 0.38f, 0.95f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.36f, 0.60f, 0.95f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.22f, 0.38f, 0.95f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.40f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.08f, 0.16f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.16f, 0.28f, 0.92f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.26f, 0.42f, 0.95f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.32f, 0.52f, 1.00f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GameEngine engine;
    engine.restart();
    int selectedSource = -1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const int* board = engine.getBoard();
        GameState currentState = engine.getCurrentState();
        Player currentTurn = engine.getCurrentTurn();
        Player winner = engine.getWinner();
        std::string message = engine.getLastMessage();

        ImGui::SetNextWindowSize(ImVec2(640, 760), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Cờ Chùi", NULL,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::TextColored(ImVec4(0.70f, 0.92f, 1.00f, 1.00f), "CỜ CHÙI - NINE MEN'S MORRIS");
        ImGui::Text("Giao dien da lien ket voi logic GameRules va GameEnd.");
        ImGui::Separator();

        ImGui::Columns(2, "layout", true);

        ImGui::BeginChild("BoardArea", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 boardOrigin = ImGui::GetCursorScreenPos();
        float spacing = 70.0f;
        float nodeRadius = 18.0f;
        ImVec2 gridOffset = ImVec2(24.0f, 24.0f);

        ImVec2 backgroundMin = boardOrigin;
        ImVec2 backgroundMax = ImVec2(boardOrigin.x + spacing * 6 + 48.0f, boardOrigin.y + spacing * 6 + 48.0f);
        drawList->AddRectFilled(backgroundMin, backgroundMax, IM_COL32(12, 26, 46, 220), 18.0f);
        drawList->AddRect(backgroundMin, backgroundMax, IM_COL32(96, 165, 255, 160), 18.0f, 0, 3.0f);

        ImU32 lineColor = IM_COL32(140, 190, 255, 180);
        float lineThickness = 2.2f;
        ImVec2 boardOffset = ImVec2(boardOrigin.x + gridOffset.x, boardOrigin.y + gridOffset.y);
        for (int ring = 0; ring < 3; ++ring) {
            int offset = ring;
            int size = 6 - ring * 2;
            ImVec2 topLeft = getNodePos(offset, offset, boardOffset, spacing);
            ImVec2 bottomRight = getNodePos(offset + size, offset + size, boardOffset, spacing);
            drawList->AddRect(topLeft, bottomRight, lineColor, 0.0f, 0, lineThickness);
        }
        drawList->AddLine(getNodePos(0, 3, boardOffset, spacing), getNodePos(2, 3, boardOffset, spacing), lineColor, lineThickness);
        drawList->AddLine(getNodePos(4, 3, boardOffset, spacing), getNodePos(6, 3, boardOffset, spacing), lineColor, lineThickness);
        drawList->AddLine(getNodePos(3, 0, boardOffset, spacing), getNodePos(3, 2, boardOffset, spacing), lineColor, lineThickness);
        drawList->AddLine(getNodePos(3, 4, boardOffset, spacing), getNodePos(3, 6, boardOffset, spacing), lineColor, lineThickness);

        std::vector<int> removablePositions;
        if (currentState == MILL_STRIKE) {
            int opponent = (currentTurn == PLAYER_A) ? PLAYER_B : PLAYER_A;
            removablePositions = GameRules::getRemovablePieces(board, opponent == PLAYER_A ? PLAYER1 : PLAYER2);
        }

        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 7; ++col) {
                int index = getBoardIndex(row, col);
                if (index < 0) {
                    continue;
                }

                ImVec2 center = getNodePos(row, col, boardOffset, spacing);
                ImGui::PushID(index);
                ImGui::SetCursorScreenPos(ImVec2(center.x - nodeRadius, center.y - nodeRadius));
                ImGui::InvisibleButton("node", ImVec2(nodeRadius * 2.0f, nodeRadius * 2.0f));
                bool hovered = ImGui::IsItemHovered();
                bool pressed = ImGui::IsItemClicked();
                bool isSelected = (selectedSource == index);
                int value = board[index];
                bool isRemovable = std::find(removablePositions.begin(), removablePositions.end(), index) != removablePositions.end();

                ImU32 fillColor = IM_COL32(20, 36, 72, 220);
                if (value == PLAYER_A) fillColor = IM_COL32(232, 106, 82, 240);
                else if (value == PLAYER_B) fillColor = IM_COL32(104, 206, 132, 240);
                else if (hovered) fillColor = IM_COL32(255, 220, 120, 180);

                ImU32 borderColor = isSelected ? IM_COL32(255, 255, 255, 230) : IM_COL32(136, 206, 255, 180);
                if (isRemovable) borderColor = IM_COL32(255, 145, 80, 220);

                drawList->AddCircleFilled(center, nodeRadius, value == EMPTY ? IM_COL32(20, 40, 74, 220) : fillColor);
                drawList->AddCircle(center, nodeRadius + 2.0f, borderColor, 0, 2.5f);

                if (value != EMPTY) {
                    drawList->AddText(ImVec2(center.x - 6.0f, center.y - 8.0f), IM_COL32(255, 255, 255, 255), pieceSymbol(value));
                }

                if (pressed && currentState != GAME_OVER) {
                    if (currentState == PHASE_1_PLACING) {
                        MoveResult result = engine.placePiece(index);
                        if (result == MoveResult::VALID) {
                            selectedSource = -1;
                        }
                    } else if (currentState == PHASE_2_MOVING) {
                        if (selectedSource < 0) {
                            if (board[index] == currentTurn) {
                                selectedSource = index;
                                message = "Da chon quan. Chon o den can di.";
                            } else {
                                message = "Hay chon quan cua ban de di chuyen.";
                            }
                        } else if (index == selectedSource) {
                            selectedSource = -1;
                            message = "Bo chon. Chon quan hoac o den.";
                        } else {
                            MoveResult result = engine.movePiece(selectedSource, index);
                            if (result == MoveResult::VALID) {
                                selectedSource = -1;
                            }
                        }
                    } else if (currentState == MILL_STRIKE) {
                        MoveResult result = engine.removePiece(index);
                        if (result == MoveResult::VALID) {
                            selectedSource = -1;
                        }
                    }

                    board = engine.getBoard();
                    currentState = engine.getCurrentState();
                    currentTurn = engine.getCurrentTurn();
                    winner = engine.getWinner();
                    message = engine.getLastMessage();
                }

                ImGui::PopID();
            }
        }

        ImGui::Dummy(ImVec2(0.0f, spacing * 7 + 32.0f));
        ImGui::EndChild();

        ImGui::NextColumn();
        ImGui::BeginChild("InfoPane", ImVec2(0, 0), true);
        ImGui::Text("Trang thai: %s", getStateLabel(currentState));
        ImGui::Text("Luot hien tai: Player %s", playerName(currentTurn));
        ImGui::Text("Quan tren ban: A = %d, B = %d", GameRules::countPieces(board, PLAYER_A), GameRules::countPieces(board, PLAYER_B));
        ImGui::Text("Quan da dat: A = %d, B = %d", engine.getPiecesPlacedA(), engine.getPiecesPlacedB());
        ImGui::Separator();
        ImGui::TextWrapped("%s", message.c_str());
        ImGui::Separator();

        if (ImGui::Button("Khoi dong lai", ImVec2(-FLT_MIN, 42))) {
            engine.restart();
            selectedSource = -1;
            board = engine.getBoard();
            currentState = engine.getCurrentState();
            currentTurn = engine.getCurrentTurn();
            winner = engine.getWinner();
            message = engine.getLastMessage();
        }
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.65f, 0.86f, 0.96f, 1.00f), "Huong dan nhanh:");
        ImGui::BulletText("Giai doan 1: nhap o trong de dat quan.");
        ImGui::BulletText("Giai doan 2: chon quan cua ban, sau do chon o den.");
        ImGui::BulletText("Tao hang 3 de xoa mot quan doi thu.");
        ImGui::BulletText("Game ket thuc neu doi thu duoi 3 quan hoac khong con nuoc di.");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(150.0f/255.0f, 210.0f/255.0f, 255.0f/255.0f, 1.0f), "Nhan nut 'Khoi dong lai' de choi lai.");
        ImGui::EndChild();

        ImGui::Columns(1);
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.04f, 0.08f, 0.15f, 1.0f);
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
