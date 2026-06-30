#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h> 

// Mang 2 chieu quan ly trang thai ban co
char board[7][7];
char currentTurn = 'X';

// Ham khoi tao ban co ban dau
void InitBoard() {
    for(int i = 0; i < 7; i++) {
        for(int j = 0; j < 7; j++) {
            board[i][j] = ' ';
        }
    }

    // Dat dau cham '.' de danh dau cac vi tri co the danh co
    board[0][0] = board[0][3] = board[0][6] = '.';
    board[1][1] = board[1][3] = board[1][5] = '.';
    board[2][2] = board[2][3] = board[2][4] = '.';
    board[3][0] = board[3][1] = board[3][2] = '.';
    board[3][4] = board[3][5] = board[3][6] = '.';
    board[4][2] = board[4][3] = board[4][4] = '.';
    board[5][1] = board[5][3] = board[5][5] = '.';
    board[6][0] = board[6][3] = board[6][6] = '.';
}

int main(int, char**) {
    // Khoi tao GLFW tao cua so
    if (!glfwInit()) return 1;
    GLFWwindow* window = glfwCreateWindow(520, 680, "DO AN CO CHUI - GIAO DIEN DEP", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Khoa FPS de may khong bi nong

    // Thiet lap ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    InitBoard();

    // Vong lap game
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- BAT DAU VE GIAO DIEN CHINH ---
        ImGui::Begin("BAN CO CHUI", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowSize(ImVec2(520, 680));
        ImGui::SetWindowPos(ImVec2(0, 0));

        ImGui::Text(" Luot choi hien tai: %c", currentTurn);
        ImGui::Separator();

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        
        float spacing = 60.0f; // Khoang cach o co
        float radius = 18.0f;  // Do to cua quan co

        // 1. VE NEN GRADIENT CHO BAN CO
        ImVec2 p_bg_start = ImVec2(p.x + 10.0f, p.y + 10.0f);
        ImVec2 p_bg_end = ImVec2(p.x + 470.0f, p.y + 470.0f); 
        
        // Setup mau cho 4 goc (Hieu ung chuyen mau)
        ImU32 col_top_left  = IM_COL32(40, 50, 75, 255);  
        ImU32 col_top_right = IM_COL32(25, 30, 50, 255);  
        ImU32 col_bot_right = IM_COL32(15, 15, 25, 255);  
        ImU32 col_bot_left  = IM_COL32(20, 25, 40, 255);  
        
        // Ve vien nen roi do mau
        draw_list->AddRectFilledMultiColor(p_bg_start, p_bg_end, col_top_left, col_top_right, col_bot_right, col_bot_left);
        draw_list->AddRect(p_bg_start, p_bg_end, IM_COL32(0, 200, 255, 150), 8.0f, 0, 2.0f); // Vien neon xanh

        // Ham tinh toa do tren man hinh
        auto GetPos = [&](int row, int col) {
            return ImVec2(p.x + col * spacing + 50.0f, p.y + row * spacing + 50.0f);
        };

        // 2. VE LUOI KHUNG BAN CO
        ImU32 line_color = IM_COL32(200, 200, 200, 150); 
        float line_thickness = 2.0f;                     

        // Ve 3 hinh vuong
        int rings[3][4] = {
            {0, 0, 6, 6},
            {1, 1, 5, 5},
            {2, 2, 4, 4} 
        };
        for (int r = 0; r < 3; r++) {
            ImVec2 top_left = GetPos(rings[r][0], rings[r][1]);
            ImVec2 bottom_right = GetPos(rings[r][2], rings[r][3]);
            draw_list->AddRect(top_left, bottom_right, line_color, 0.0f, 0, line_thickness);
        }

        // Ve duong chu thap ket noi cac hinh vuong
        draw_list->AddLine(GetPos(0, 3), GetPos(2, 3), line_color, line_thickness); 
        draw_list->AddLine(GetPos(4, 3), GetPos(6, 3), line_color, line_thickness); 
        draw_list->AddLine(GetPos(3, 0), GetPos(3, 2), line_color, line_thickness); 
        draw_list->AddLine(GetPos(3, 4), GetPos(3, 6), line_color, line_thickness); 

        // 3. VE CAC QUAN CO VA XU LY CLICK CHUOT
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (board[i][j] != ' ') {
                    ImGui::PushID(i * 7 + j);

                    ImVec2 center = GetPos(i, j);

                    // Tao nut tang hinh de click
                    ImGui::SetCursorScreenPos(ImVec2(center.x - radius, center.y - radius));
                    if (ImGui::InvisibleButton("node", ImVec2(radius * 2, radius * 2))) {
                        if (board[i][j] == '.') {
                            board[i][j] = currentTurn;
                            if (currentTurn == 'X') {
                                currentTurn = 'O';
                            } else {
                                currentTurn = 'X';
                            }
                        }
                    }

                    // To mau cho cac quan co
                    ImU32 color = IM_COL32(100, 100, 100, 255); // Xam de lam diem neo
                    if (board[i][j] == 'X') {
                        color = IM_COL32(255, 60, 50, 255); // X do
                    } else if (board[i][j] == 'O') {
                        color = IM_COL32(50, 200, 90, 255); // O xanh la
                    } else if (ImGui::IsItemHovered()) {
                        color = IM_COL32(255, 220, 0, 255); // Di chuot vao sang mau vang
                    }

                    // Ve hinh len man hinh
                    if (board[i][j] == '.') {
                        draw_list->AddCircleFilled(center, 5.0f, color); // Diem chua danh thi nho
                    } else {
                        draw_list->AddCircleFilled(center, radius, color); // Danh roi thi to len
                        draw_list->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 0, 2.0f); // Them vien trang
                    }

                    ImGui::PopID();
                }
            }
        }

        // Nut Reset Game nam phia duoi
        ImGui::SetCursorScreenPos(ImVec2(200.0f, p.y + 500.0f));
        if (ImGui::Button("Reset Game", ImVec2(120, 40))) {
            InitBoard();
            currentTurn = 'X';
        }

        ImGui::End();

        // 4. HIEN THI RA MAN HINH MAY TINH
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        // Mau nen ben ngoai ban co - Dung mau xanh den toi
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Don dep RAM khi tat app
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}