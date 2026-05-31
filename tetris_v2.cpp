// ============================================================
// TETRIS v2 - Giao dien do hoa voi Raylib
// Bien dich (MinGW/Windows):
//   g++ tetris_v2.cpp -o tetris_v2.exe -lraylib -lopengl32 -lgdi32 -lwinmm
// Can cai Raylib: https://www.raylib.com
// Moi logic game giu nguyen tu v1 (thu tuc, khong OOP)
// ============================================================
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
// ============================================================
// HANG SO GAME
// ============================================================
const int ROWS      = 20;
const int COLS      = 10;
const int PSIZE     = 4;
const int NUM_TYPES = 7;
const int NUM_ROTS  = 4;
const int CELL_EMPTY  = 0;
const int CELL_LOCKED = 1;
// ============================================================
// HANG SO GIAO DIEN
// ============================================================
const int CELL_PX   = 32;          // Kich thuoc o tinh bang pixel
const int BOARD_X   = 20;          // Goc X cua bang
const int BOARD_Y   = 20;          // Goc Y cua bang
const int PANEL_X   = BOARD_X + COLS * CELL_PX + 24;
const int WIN_W     = PANEL_X + 190;
const int WIN_H     = ROWS * CELL_PX + 40;
// ============================================================
// DINH NGHIA 7 KHOI TETROMINO
// PIECES[type][rotation][row][col]
// ============================================================
const int PIECES[NUM_TYPES][NUM_ROTS][PSIZE][PSIZE] = {
    // 0: I
    { {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
      {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
      {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
      {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}} },
    // 1: O
    { {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}} },
    // 2: T
    { {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
      {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
      {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}} },
    // 3: S
    { {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
      {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
      {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}} },
    // 4: Z
    { {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
      {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
      {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}} },
    // 5: J
    { {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
      {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
      {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}} },
    // 6: L
    { {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
      {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
      {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
      {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}} }
};
// Mau sac tung loai khoi (RGBA)
Color PIECE_COLORS[NUM_TYPES] = {
    {0,   230, 230, 255},   // I - Cyan
    {230, 220, 0,   255},   // O - Yellow
    {160, 0,   220, 255},   // T - Purple
    {0,   200, 0,   255},   // S - Green
    {220, 30,  30,  255},   // Z - Red
    {30,  80,  220, 255},   // J - Blue
    {230, 140, 0,   255},   // L - Orange
};
// ============================================================
// CAU TRUC DU LIEU (chi data, khong co method)
// ============================================================
struct Piece {
    int shape[PSIZE][PSIZE];
    int row, col;
    int type, rotation;
};
struct GameState {
    int   board[ROWS][COLS];       // 0=trong, 1=da khoa
    int   boardColor[ROWS][COLS];  // type index cua o da khoa (de to mau)
    Piece current;
    Piece next;
    int   score, level, totalLines;
    bool  isOver, isPaused;
};
// ============================================================
// CAC HAM LOGIC GAME (giu nguyen tu Version 1)
// ============================================================
// Khoi tao bang ve trang
void initBoard(GameState *gs) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            gs->board[r][c]      = CELL_EMPTY;
            gs->boardColor[r][c] = -1;
        }
}
// Tao loai khoi ngau nhien
int randomType() {
    return rand() % NUM_TYPES;
}
// Nap hinh dang khoi vao struct Piece
void loadShape(Piece *p, int type, int rot) {
    p->type     = type;
    p->rotation = rot;
    for (int r = 0; r < PSIZE; r++)
        for (int c = 0; c < PSIZE; c++)
            p->shape[r][c] = PIECES[type][rot][r][c];
}
// Kiem tra va cham: khoi (shape) tai (row,col) co hop le khong
bool checkCollision(int board[ROWS][COLS],
                    int shape[PSIZE][PSIZE],
                    int row, int col)
{
    for (int r = 0; r < PSIZE; r++)
        for (int c = 0; c < PSIZE; c++)
            if (shape[r][c] == 1) {
                int nr = row + r, nc = col + c;
                if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) return true;
                if (board[nr][nc] == CELL_LOCKED)                  return true;
            }
    return false;
}
// Di chuyen trai
bool moveLeft(GameState *gs) {
    if (!checkCollision(gs->board, gs->current.shape,
                        gs->current.row, gs->current.col - 1)) {
        gs->current.col--;
        return true;
    }
    return false;
}
// Di chuyen phai
bool moveRight(GameState *gs) {
    if (!checkCollision(gs->board, gs->current.shape,
                        gs->current.row, gs->current.col + 1)) {
        gs->current.col++;
        return true;
    }
    return false;
}
// Di chuyen xuong 1 buoc
bool moveDown(GameState *gs) {
    if (!checkCollision(gs->board, gs->current.shape,
                        gs->current.row + 1, gs->current.col)) {
        gs->current.row++;
        return true;
    }
    return false;
}
// Hard drop: ha ngay xuong day
void hardDrop(GameState *gs) {
    while (moveDown(gs));
}
// Xoay khoi 90 do (co wall kick)
void rotatePiece(GameState *gs) {
    int newRot = (gs->current.rotation + 1) % NUM_ROTS;
    int tmp[PSIZE][PSIZE];
    for (int r = 0; r < PSIZE; r++)
        for (int c = 0; c < PSIZE; c++)
            tmp[r][c] = PIECES[gs->current.type][newRot][r][c];
    // Thu xoay o vi tri goc, roi thu wall kick trai/phai
    int kicks[3][2] = {{0,0},{0,1},{0,-1}};
    for (int i = 0; i < 3; i++) {
        int tr = gs->current.row + kicks[i][0];
        int tc = gs->current.col + kicks[i][1];
        if (!checkCollision(gs->board, tmp, tr, tc)) {
            gs->current.rotation = newRot;
            gs->current.row      = tr;
            gs->current.col      = tc;
            for (int r = 0; r < PSIZE; r++)
                for (int c = 0; c < PSIZE; c++)
                    gs->current.shape[r][c] = tmp[r][c];
            return;
        }
    }
}
// Khoa khoi vao bang sau khi khoi cham day
void lockPiece(GameState *gs) {
    for (int r = 0; r < PSIZE; r++)
        for (int c = 0; c < PSIZE; c++)
            if (gs->current.shape[r][c] == 1) {
                int br = gs->current.row + r;
                int bc = gs->current.col + c;
                if (br >= 0 && br < ROWS && bc >= 0 && bc < COLS) {
                    gs->board[br][bc]      = CELL_LOCKED;
                    gs->boardColor[br][bc] = gs->current.type;
                }
            }
}
// Kiem tra hang day
bool isLineFull(int board[ROWS][COLS], int row) {
    for (int c = 0; c < COLS; c++)
        if (board[row][c] == CELL_EMPTY) return false;
    return true;
}
// Xoa mot hang, dich cac hang tren xuong
void removeLine(GameState *gs, int row) {
    for (int r = row; r > 0; r--)
        for (int c = 0; c < COLS; c++) {
            gs->board[r][c]      = gs->board[r-1][c];
            gs->boardColor[r][c] = gs->boardColor[r-1][c];
        }
    for (int c = 0; c < COLS; c++) {
        gs->board[0][c]      = CELL_EMPTY;
        gs->boardColor[0][c] = -1;
    }
}
// Xoa tat ca hang day, tra ve so hang da xoa
int clearLines(GameState *gs) {
    int cleared = 0;
    int r = ROWS - 1;
    while (r >= 0) {
        if (isLineFull(gs->board, r)) { removeLine(gs, r); cleared++; }
        else r--;
    }
    return cleared;
}
// Cap nhat diem va level
void updateScore(GameState *gs, int lines) {
    int bonus[5] = {0, 100, 300, 500, 800};
    if (lines >= 1 && lines <= 4) {
        gs->score      += bonus[lines] * gs->level;
        gs->totalLines += lines;
        gs->level       = gs->totalLines / 10 + 1;
    }
}
// Sinh khoi moi: chuyen next -> current, tao next moi
void spawnPiece(GameState *gs) {
    gs->current     = gs->next;
    gs->current.row = 0;
    gs->current.col = COLS / 2 - 2;
    loadShape(&gs->next, randomType(), 0);
    gs->next.row = gs->next.col = 0;
}
// Khoi tao toan bo game
void initGame(GameState *gs) {
    initBoard(gs);
    gs->score = 0; gs->level = 1; gs->totalLines = 0;
    gs->isOver = false; gs->isPaused = false;
    loadShape(&gs->next, randomType(), 0);
    gs->next.row = gs->next.col = 0;
    spawnPiece(gs);
}
// Tinh khoang thoi gian roi theo level (giay)
float dropInterval(int level) {
    float t = 0.80f - (level - 1) * 0.07f;
    return (t < 0.10f) ? 0.10f : t;
}
// Tinh hang ghost (bong mo cho biet khoi se roi o dau)
int ghostRow(GameState *gs) {
    int g = gs->current.row;
    while (!checkCollision(gs->board, gs->current.shape, g + 1, gs->current.col))
        g++;
    return g;
}
// ============================================================
// HAM VE (RAYLIB RENDERING)
// ============================================================
// Ve mot o vuong
void drawCell(int row, int col, Color fill, Color border) {
    int x = BOARD_X + col * CELL_PX;
    int y = BOARD_Y + row * CELL_PX;
    DrawRectangle(x, y, CELL_PX,     CELL_PX,     fill);
    DrawRectangleLines(x, y, CELL_PX, CELL_PX,    border);
}
// Ve toan bo frame
void renderGame(GameState *gs) {
    ClearBackground(Color{12, 12, 25, 255});
    // -- Ghost piece --
    if (!gs->isOver && !gs->isPaused) {
        int gr = ghostRow(gs);
        Color gc = PIECE_COLORS[gs->current.type];
        gc.a = 55;
        for (int r = 0; r < PSIZE; r++)
            for (int c = 0; c < PSIZE; c++)
                if (gs->current.shape[r][c] == 1) {
                    int br = gr + r, bc = gs->current.col + c;
                    if (br >= 0 && br < ROWS && bc >= 0 && bc < COLS)
                        drawCell(br, bc, gc, gc);
                }
    }
    // -- Bang game --
    Color emptyFill   = {22, 22, 48,  255};
    Color emptyBorder = {38, 38, 70,  255};
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            if (gs->board[r][c] == CELL_LOCKED) {
                int t = gs->boardColor[r][c];
                Color f = (t >= 0) ? PIECE_COLORS[t] : GRAY;
                Color b = {(unsigned char)(f.r/3), (unsigned char)(f.g/3), (unsigned char)(f.b/3), 255};
                drawCell(r, c, f, b);
            } else {
                drawCell(r, c, emptyFill, emptyBorder);
            }
        }
    // -- Khoi dang roi --
    if (!gs->isOver) {
        Color f = PIECE_COLORS[gs->current.type];
        Color b = {(unsigned char)(f.r/3+20), (unsigned char)(f.g/3+20), (unsigned char)(f.b/3+20), 255};
        for (int r = 0; r < PSIZE; r++)
            for (int c = 0; c < PSIZE; c++)
                if (gs->current.shape[r][c] == 1) {
                    int br = gs->current.row + r, bc = gs->current.col + c;
                    if (br >= 0 && br < ROWS && bc >= 0 && bc < COLS)
                        drawCell(br, bc, f, b);
                }
    }
    // -- Vien bang --
    DrawRectangleLines(BOARD_X - 2, BOARD_Y - 2,
                       COLS * CELL_PX + 4, ROWS * CELL_PX + 4,
                       Color{80, 100, 200, 255});
    // ============================================================
    // -- PANEL THONG TIN --
    // ============================================================
    int px = PANEL_X, py = BOARD_Y;
    char buf[64];
    // Tieu de
    DrawText("TETRIS", px, py, 30, Color{80, 200, 255, 255});
    DrawLine(px, py+38, px+175, py+38, Color{60,60,130,255});
    // Score
    DrawText("SCORE",  px, py+50,  13, LIGHTGRAY);
    snprintf(buf, sizeof(buf), "%d", gs->score);
    DrawText(buf,      px, py+67,  24, WHITE);
    // Level
    DrawText("LEVEL",  px, py+105, 13, LIGHTGRAY);
    snprintf(buf, sizeof(buf), "%d", gs->level);
    DrawText(buf,      px, py+122, 24, Color{255,220,50,255});
    // Lines
    DrawText("LINES",  px, py+160, 13, LIGHTGRAY);
    snprintf(buf, sizeof(buf), "%d", gs->totalLines);
    DrawText(buf,      px, py+177, 24, Color{80,255,140,255});
    DrawLine(px, py+215, px+175, py+215, Color{60,60,130,255});
    // Next piece
    DrawText("NEXT", px, py+225, 13, LIGHTGRAY);
    int ns = 24;
    Color nf = PIECE_COLORS[gs->next.type];
    Color nb = {(unsigned char)(nf.r/3), (unsigned char)(nf.g/3), (unsigned char)(nf.b/3), 255};
    for (int r = 0; r < PSIZE; r++)
        for (int c = 0; c < PSIZE; c++)
            if (gs->next.shape[r][c] == 1) {
                DrawRectangle(px + c*ns, py+245 + r*ns, ns, ns, nf);
                DrawRectangleLines(px + c*ns, py+245 + r*ns, ns, ns, nb);
            }
    DrawLine(px, py+355, px+175, py+355, Color{60,60,130,255});
    // Controls
    Color ch = {160,160,160,255};
    DrawText("CONTROLS",      px, py+365, 12, LIGHTGRAY);
    DrawText("[LEFT/RIGHT] Di chuyen", px, py+382, 11, ch);
    DrawText("[UP]    Xoay",           px, py+397, 11, ch);
    DrawText("[DOWN]  Roi nhanh",      px, py+412, 11, ch);
    DrawText("[SPACE] Hard Drop",      px, py+427, 11, ch);
    DrawText("[P]     Tam dung",       px, py+442, 11, ch);
    DrawText("[R]     Choi lai",       px, py+457, 11, ch);
    DrawText("[Esc]   Thoat",          px, py+472, 11, ch);
    // -- Overlay GAME OVER --
    if (gs->isOver) {
        DrawRectangle(BOARD_X, BOARD_Y, COLS*CELL_PX, ROWS*CELL_PX, Color{0,0,0,170});
        DrawText("GAME OVER",        BOARD_X+18, BOARD_Y + ROWS*CELL_PX/2 - 35, 30, Color{255,70,70,255});
        DrawText("Nhan [R] de choi lai", BOARD_X+8, BOARD_Y + ROWS*CELL_PX/2 + 10, 17, WHITE);
    }
    // -- Overlay PAUSE --
    if (gs->isPaused && !gs->isOver) {
        DrawRectangle(BOARD_X, BOARD_Y, COLS*CELL_PX, ROWS*CELL_PX, Color{0,0,0,150});
        DrawText("TAM DUNG", BOARD_X+30, BOARD_Y + ROWS*CELL_PX/2 - 20, 28, Color{255,220,50,255});
        DrawText("Nhan [P] de tiep tuc", BOARD_X+5, BOARD_Y + ROWS*CELL_PX/2 + 18, 16, LIGHTGRAY);
    }
}
// ============================================================
// HAM XU LY INPUT RAYLIB
// ============================================================
// DAS = Delayed Auto Shift (giu phim de di chuyen lien tuc)
const float DAS_DELAY       = 0.150f;  // giay cho truoc khi tu dong lap
const float DAS_STEP        = 0.045f;  // giay giua moi buoc tu dong
const float SOFT_DROP_STEP  = 0.050f;  // giay giua moi buoc soft drop (~20 o/s)
// Bien toan cuc cho DAS va soft drop
static float s_dasAccum  = 0.0f;
static float s_repAccum  = 0.0f;
static int   s_lastHDir  = 0;
static float s_softAccum = 0.0f;
// Bug fix: nhan them float *pDropAccum de reset sau khi hard drop
// Neu khong reset, khoi moi co the bi khoa ngay lap tuc sau hard drop
void handleInput(GameState *gs, float dt, float *pDropAccum) {
    // Restart khi game over
    if (gs->isOver) {
        if (IsKeyPressed(KEY_R)) {
            initGame(gs);
            s_dasAccum=0; s_repAccum=0; s_lastHDir=0; s_softAccum=0;
            *pDropAccum = 0.0f;  // reset timer khi restart
        }
        return;
    }
    // Pause toggle
    if (IsKeyPressed(KEY_P)) { gs->isPaused = !gs->isPaused; return; }
    if (gs->isPaused) return;
    // Xoay (nhan mot lan)
    if (IsKeyPressed(KEY_UP))    rotatePiece(gs);
    // Hard drop: ha ngay + reset dropAccum tranh khoa khoi moi ngay lap tuc
    if (IsKeyPressed(KEY_SPACE)) { hardDrop(gs); *pDropAccum = 0.0f; }
    // Soft drop: dung timer, khong di chuyen moi frame (tranh qua nhanh 60x/s)
    if (IsKeyDown(KEY_DOWN)) {
        s_softAccum += dt;
        if (s_softAccum >= SOFT_DROP_STEP) {
            s_softAccum -= SOFT_DROP_STEP;
            moveDown(gs);
        }
    } else {
        s_softAccum = 0.0f;
    }
    // Di chuyen ngang voi DAS
    int hdir = 0;
    if (IsKeyDown(KEY_LEFT))  hdir = -1;
    if (IsKeyDown(KEY_RIGHT)) hdir =  1;
    // Khi bam lan dau hoac doi huong: di chuyen ngay, reset timer
    if (IsKeyPressed(KEY_LEFT))  { moveLeft(gs);  s_lastHDir=-1; s_dasAccum=0; s_repAccum=0; return; }
    if (IsKeyPressed(KEY_RIGHT)) { moveRight(gs); s_lastHDir= 1; s_dasAccum=0; s_repAccum=0; return; }
    // Giu phim: dem thoi gian DAS
    if (hdir != 0 && hdir == s_lastHDir) {
        s_dasAccum += dt;
        if (s_dasAccum >= DAS_DELAY) {
            s_repAccum += dt;
            if (s_repAccum >= DAS_STEP) {
                s_repAccum -= DAS_STEP;
                if (hdir == -1) moveLeft(gs);
                else            moveRight(gs);
            }
        }
    } else {
        s_lastHDir = hdir;
        s_dasAccum = 0;
        s_repAccum = 0;
    }
}
// ============================================================
// MAIN - Khoi dong Raylib va chay game loop
// ============================================================
int main() {
    srand((unsigned int)time(NULL));
    InitWindow(WIN_W, WIN_H, "TETRIS v2 - Raylib");
    SetTargetFPS(60);
    GameState gs;
    initGame(&gs);
    float dropAccum = 0.0f;  // Dem thoi gian de ha khoi tu dong
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        // -- Xu ly input --
        handleInput(&gs, dt, &dropAccum);
        // -- Cap nhat: tu dong ha khoi theo thoi gian --
        if (!gs.isOver && !gs.isPaused) {
            dropAccum += dt;
            if (dropAccum >= dropInterval(gs.level)) {
                dropAccum = 0.0f;
                if (!moveDown(&gs)) {
                    // Khoi cham day: khoa, xoa hang, sinh khoi moi
                    lockPiece(&gs);
                    int lines = clearLines(&gs);
                    updateScore(&gs, lines);
                    spawnPiece(&gs);
                    // Khoi moi xuat hien ma bi chong = game over
                    if (checkCollision(gs.board, gs.current.shape,
                                       gs.current.row, gs.current.col))
                        gs.isOver = true;
                }
            }
        }
        // -- Ve --
        BeginDrawing();
        renderGame(&gs);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
