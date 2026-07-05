# 🎮 Cờ Chùi - Nine Men's Morris

Dự án trò chơi Cờ Chùi được xây dựng bằng C++ với giao diện hiện đại sử dụng ImGui và GLFW. Trò chơi hỗ trợ chế độ người chơi đối kháng và chơi với AI ở nhiều mức độ khó khác nhau.

## ✨ Tính năng

- Chế độ PvP (người vs người)
- Chế độ PvE (người vs AI)
- AI ở 3 mức: dễ, trung bình, khó
- Luật chơi đầy đủ của Cờ Chùi: đặt quân, di chuyển quân, tạo mill và ăn quân đối thủ
- Hỗ trợ một số tính năng quản lý trạng thái như lưu/đọc trò chơi, undo/redo

## 🧩 Kiến trúc dự án

Dự án được tổ chức theo hướng phân lớp rõ ràng:

- Model: xử lý bàn cờ, luật chơi, kiểm tra nước đi và điều kiện kết thúc
- View: giao diện người dùng bằng ImGui
- Controller: điều phối luồng trò chơi và trạng thái game
- AI: triển khai các chiến lược chơi cho bot

## 📁 Cấu trúc thư mục

```text
.
├── CoChui_UI/            # Giao diện đồ họa ImGui/GLFW
│   ├── main.cpp
│   ├── backend/
│   ├── imgui/
│   └── include/
├── souce/
│   ├── AI/               # AI Easy/Medium/Hard
│   ├── Controller/       # GameEngine
│   ├── Model/            # Logic bàn cờ, luật chơi
│   ├── View/             # Console UI
│   ├── GameEnd.cpp/.h
│   ├── SaveLoad.cpp/.h
│   ├── UndoRedo.cpp/.h
│   └── ...
└── README.md
```

## 🛠️ Yêu cầu

- Trình biên dịch C++ hỗ trợ C++17 trở lên
- Windows (khuyến nghị) với MinGW hoặc MSVC
- Thư viện GLFW và OpenGL có sẵn trong thư mục CoChui_UI

## ▶️ Cách biên dịch và chạy

### Windows với MinGW

Ví dụ lệnh biên dịch:

```bash
g++ -std=c++17 -I. -ICoChui_UI -ICoChui_UI/include -ICoChui_UI/backend -ICoChui_UI/imgui ^
    CoChui_UI/main.cpp CoChui_UI/backend/imgui_impl_glfw.cpp CoChui_UI/backend/imgui_impl_opengl3.cpp ^
    CoChui_UI/imgui/imgui.cpp CoChui_UI/imgui/imgui_draw.cpp CoChui_UI/imgui/imgui_tables.cpp CoChui_UI/imgui/imgui_widgets.cpp ^
    souce/Model/Board.cpp souce/Model/GameRule.cpp souce/Controller/GameEngine.cpp ^
    souce/AI/AI_EasyMed.cpp souce/AI/AI_Minimax.cpp souce/GameEnd.cpp souce/SaveLoad.cpp souce/UndoRedo.cpp ^
    -o CoChui.exe -LCoChui_UI/lib -lglfw3 -lopengl32 -lgdi32 -luser32
```

Sau đó chạy:

```bash
CoChui.exe
```

## 🎯 Luật chơi cơ bản

1. Giai đoạn 1: đặt quân lên bàn cờ.
2. Giai đoạn 2: di chuyển quân đến vị trí trống liền kề.
3. Khi tạo thành mill, người chơi có thể loại bỏ một quân của đối thủ.
4. Trò chơi kết thúc khi một bên còn ít hơn 3 quân hoặc không còn nước đi hợp lệ.

## 👥 Ghi chú

Dự án này phù hợp để học tập về lập trình hướng đối tượng, cấu trúc MVC, thuật toán AI và phát triển game C++.
```