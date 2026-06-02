# Teris — Game Tetris Arcade Classic

## Mô tả dự án

**Teris** là dự án game Tetris được phát triển bằng ngôn ngữ **C++** trong khuôn khổ môn học **Kỹ thuật lập trình**. Dự án được xây dựng theo phong cách lập trình **thủ tục (procedural)**, không sử dụng lập trình hướng đối tượng (OOP), tập trung vào các kỹ thuật cơ bản: hàm, mảng, chuỗi, cấu trúc điều khiển và con trỏ.

Dự án có **3 phiên bản** phát triển dần theo thời gian:

| Phiên bản | File nguồn | Mô tả |
|-----------|------------|-------|
| **v1** | `tetris.cpp` | Game chạy trên console (Windows), điều khiển bằng bàn phím |
| **v2** | `tetris_v2.cpp` | Giao diện đồ họa với thư viện [Raylib](https://www.raylib.com), hiển thị điểm số và khối tiếp theo |
| **v3** *(khuyến nghị)* | `tetris_v3.cpp` | Phiên bản hoàn chỉnh phong cách **arcade cổ điển**: màu sắc đa dạng, menu, chế độ giải đấu 6 vòng, cửa hàng skin và lưu dữ liệu người chơi |

### Tính năng chính (v3)

- 7 loại khối Tetromino (I, O, T, S, Z, J, L) với màu sắc riêng biệt
- Lưới (grid) và hiệu ứng scanline phong cách máy arcade
- Panel thông tin: **Score**, **Level**, **Lines**, **Hi-Score**, **Coins**
- Chế độ **Chơi đơn** với tăng tốc độ theo cấp độ
- Chế độ **Giải đấu 6 vòng** đấu với AI (độ khó tăng dần, thưởng coin khi thắng)
- **Cửa hàng skin** gạch: Classic, Neon, Retro, Shadow, Dark
- Lưu/tải dữ liệu người chơi qua file `save.dat`
- Trạng thái **Game Over**, **Pause** và khởi động lại

---

## Mục lục

1. [Mô tả dự án](#mô-tả-dự-án)
2. [Cấu trúc thư mục](#cấu-trúc-thư-mục)
3. [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
4. [Hướng dẫn chạy dự án](#hướng-dẫn-chạy-dự-án)
5. [Cách sử dụng dự án](#cách-sử-dụng-dự-án)
6. [Thành viên nhóm](#thành-viên-nhóm)
7. [Giấy phép](#giấy-phép)

---

## Cấu trúc thư mục

```
teris/
├── tetris.cpp          # Phiên bản 1 — Console
├── tetris_v2.cpp       # Phiên bản 2 — Đồ họa Raylib cơ bản
├── tetris_v3.cpp       # Phiên bản 3 — Arcade Classic (đầy đủ tính năng)
├── build.bat           # Script build tự động (Windows)
├── save.dat            # File lưu điểm cao, coin, skin (tự tạo khi chơi)
├── prompt.md           # Ghi chú yêu cầu phát triển các phiên bản
└── raylib/
    └── raylib-5.5_win64_mingw-w64/
        └── include/    # Header Raylib
```

---

## Yêu cầu hệ thống

- **Hệ điều hành:** Windows 10 trở lên
- **Trình biên dịch:** MinGW-w64 (`g++`) hoặc tương thích
- **Thư viện:** Raylib 5.5 (đã có sẵn header trong thư mục `raylib/`)

> **Lưu ý:** Để build phiên bản v2 và v3, cần có thư mục `lib` chứa file `libraylib.a` bên trong `raylib/raylib-5.5_win64_mingw-w64/`. Nếu chưa có, tải bản **raylib 5.5 win64 mingw-w64** từ [raylib.com](https://www.raylib.com) và giải nén đầy đủ vào thư mục `raylib/`.

---

## Hướng dẫn chạy dự án

### Cách 1: Dùng script build (khuyến nghị)

1. Mở **Command Prompt** hoặc **PowerShell** tại thư mục dự án `teris/`
2. Chạy lệnh:

```bat
build.bat
```

3. Chọn phiên bản cần build:
   - `1` — Build `tetris_v2.exe`
   - `2` — Build `tetris_v3.exe` *(mặc định)*
   - `3` — Build cả hai

4. Sau khi build thành công, game sẽ tự động khởi chạy.

### Cách 2: Build thủ công

**Phiên bản 1 — Console:**

```bat
g++ tetris.cpp -o tetris.exe
tetris.exe
```

**Phiên bản 3 — Arcade Classic:**

```bat
g++ tetris_v3.cpp -o tetris_v3.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm

tetris_v3.exe
```

**Phiên bản 2:**

```bat
g++ tetris_v2.cpp -o tetris_v2.exe ^
  -I".\raylib\raylib-5.5_win64_mingw-w64\include" ^
  -L".\raylib\raylib-5.5_win64_mingw-w64\lib" ^
  -lraylib -lopengl32 -lgdi32 -lwinmm

tetris_v2.exe
```

---

## Cách sử dụng dự án

### Phiên bản 1 — Console (`tetris.cpp`)

| Phím | Chức năng |
|------|-----------|
| `A` / Mũi tên trái | Dịch khối sang trái |
| `D` / Mũi tên phải | Dịch khối sang phải |
| `S` / Mũi tên xuống | Rơi nhanh |
| `W` / Mũi tên lên | Xoay khối |
| `Q` | Thoát game |

### Phiên bản 2 & 3 — Đồ họa Raylib

#### Menu chính (v3)

Dùng phím **↑ / ↓** để chọn, **Enter** để xác nhận:

| Mục menu | Mô tả |
|----------|-------|
| **CHƠI ĐƠN** | Chơi một mình, tích lũy điểm và coin |
| **GIẢI ĐẤU (6 VÒNG)** | Đấu với AI qua 6 vòng, độ khó tăng dần |
| **CỬA HÀNG SKIN** | Mua và trang bị skin gạch bằng coin |
| **THOÁT** | Đóng game |

#### Điều khiển trong game

| Phím | Chức năng |
|------|-----------|
| `←` / `→` | Di chuyển khối trái / phải |
| `↑` | Xoay khối |
| `↓` | Soft drop (rơi nhanh) |
| `Space` | Hard drop (rơi thẳng xuống đáy) |
| `P` | Tạm dừng / tiếp tục |
| `R` | Chơi lại (khi Game Over) |
| `Esc` | Quay về menu chính |

#### Chế độ Giải đấu

- Có **6 vòng**, mỗi vòng đối đầu với một AI có độ khó khác nhau: Rookie → Amateur → Semi-Pro → Pro → Expert → Master
- Mục tiêu mỗi vòng: xóa đủ số hàng quy định trước đối thủ
- Thắng vòng sẽ nhận **coin thưởng** (30 → 300 coin tùy vòng)
- Nhấn **Enter** sau mỗi vòng để tiếp tục hoặc quay về menu

#### Cửa hàng Skin

| Skin | Giá (coin) | Mô tả |
|------|------------|-------|
| Classic | 0 (mặc định) | Gạch cổ điển đơn giản |
| Neon | 100 | Viền sáng neon rực rỡ |
| Retro | 150 | Phong cách pixel retro |
| Shadow | 200 | Có bóng đổ 3D |
| Dark | 350 | Tối huyền bí pulse |

Dùng **↑ / ↓** chọn skin, **Enter** để mua hoặc trang bị, **Esc** quay lại menu.

#### Cơ chế tính điểm & coin

- Xóa hàng trong chế độ đơn: **+2 coin** mỗi hàng
- Điểm số và **Hi-Score** được lưu tự động vào `save.dat`
- Level tăng theo số hàng đã xóa → tốc độ rơi nhanh hơn

---

## Thành viên nhóm

*Môn học: **Kỹ thuật lập trình***

| STT | MSSV | Họ và tên | Vai trò |
|-----|------|-----------|---------|
| 1 | 036207014966 | *Đoàn Đức Duy* | Trưởng nhóm |
| 2 | | *Nguyễn Đang Nhựt* | Thành viên |
| 3 | 051207000452 | *Lê Trần Thanh Phong* | Thành viên |
| 4 | 051207000452 | *Trần Quốc Đạt* | Thành viên |

---

## Giấy phép

### Mã nguồn dự án Teris

Dự án này được phát hành theo giấy phép **MIT License**.

```
MIT License

Copyright (c) 2026 Nhóm Teris — Môn Kỹ thuật lập trình

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Thư viện Raylib

Dự án sử dụng thư viện [Raylib](https://www.raylib.com) (phiên bản 5.5), được cấp phép theo **zlib/libpng License**. Chi tiết xem tại `raylib/raylib-5.5_win64_mingw-w64/LICENSE`.
