// ============================================================
// TETRIS v3 - Arcade Classic
// Build: g++ tetris_v3.cpp -o tetris_v3.exe
//        -I".\raylib\raylib-5.5_win64_mingw-w64\include"
//        -L".\raylib\raylib-5.5_win64_mingw-w64\lib"
//        -lraylib -lopengl32 -lgdi32 -lwinmm
// Khong dung OOP - Thuan thu tuc (Procedural C++)
// ============================================================
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cmath>

// ============================================================
// HANG SO (CONSTANTS)
// ============================================================
const int WIN_W  = 960,  WIN_H   = 680;
const int ROWS   = 20,   COLS    = 10;
const int PSIZE  = 4,    CELL    = 28;
const int NUM_TYPES = 7, NUM_ROTS = 4;
const int EMPTY  = 0,   LOCKED  = 1;
const int ROUNDS = 6,   NUM_SKINS = 5;

// Layout don (single player)
const int SP_BX = 210, SP_BY = 40;
const int SP_PX = SP_BX + COLS*CELL + 20;

// Layout giai dau (tournament)
const int T_LBX = 25,  T_RBX = 650, T_BY = 90;
const int T_MID = T_LBX + COLS*CELL + 12;

// Man hinh (screens)
const int SCR_MENU = 0, SCR_PLAY = 1, SCR_TOUR = 2, SCR_SHOP = 3;

// DAS / Soft drop
const float DAS_D = 0.15f, DAS_S = 0.045f, SD_S = 0.05f;

// Giai dau: muc tieu hang xoa va thuong coin
const int T_TARGETS[6] = {5, 8, 12, 15, 20, 25};
const int T_COINS[6]   = {30, 50, 80, 120, 180, 300};

// Gia skin
const int SKIN_PRICES[5]     = {0, 100, 150, 200, 350};
const char* SKIN_NAMES[5]    = {"Classic","Neon","Retro","Shadow","Dark"};
const char* SKIN_DESC[5]     = {
    "Gach co dien don gian",
    "Vien sang neon ruc ro",
    "Phong cach pixel retro",
    "Co bong do 3D",
    "Toi huyen bi pulse"
};

// ============================================================
// DINH NGHIA 7 KHOI TETROMINO
// ============================================================
const int PIECES[NUM_TYPES][NUM_ROTS][PSIZE][PSIZE] = {
    // 0: I
    {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
     {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
    // 1: O
    {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
    // 2: T
    {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    // 3: S
    {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
     {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    // 4: Z
    {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}},
    // 5: J
    {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}},
    // 6: L
    {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}}
};

Color PIECE_COLORS[NUM_TYPES] = {
    {0,   230, 230, 255},  // I Cyan
    {230, 220, 0,   255},  // O Yellow
    {160, 0,   220, 255},  // T Purple
    {0,   200, 0,   255},  // S Green
    {220, 30,  30,  255},  // Z Red
    {30,  80,  220, 255},  // J Blue
    {230, 140, 0,   255},  // L Orange
};

// ============================================================
// CAU TRUC DU LIEU (chi data, khong co method)
// ============================================================
struct Piece {
    int shape[PSIZE][PSIZE];
    int row, col, type, rotation;
};

struct Board {
    int   cells[ROWS][COLS];
    int   colors[ROWS][COLS];
    Piece current, next;
    int   score, level, totalLines;
    bool  isOver, isPaused;
};

struct AIData {
    Board board;
    float dropAccum;
    float thinkTimer;
    int   difficulty;   // 1-6
    int   targetCol;
    int   targetRot;
    bool  moveReady;
};

struct TourData {
    int  round;         // 0-5
    int  playerWins;
    int  coinsEarned;
    bool roundOver;
    bool playerWon;
    bool coinsAwarded;
};

struct PlayerProfile {
    int hiScore;
    int coins;
    int skin;
    int unlocked[NUM_SKINS];
};

// ============================================================
// BIEN TOAN CUC CHO DAS / SOFT DROP
// ============================================================
static float s_das = 0, s_rep = 0, s_soft = 0;
static int   s_hdir = 0;

// ============================================================
// LOGIC GAME CO BAN (giu tu v2, doi Board*)
// ============================================================
void initBoard(Board *b) {
    for (int r=0;r<ROWS;r++)
        for (int c=0;c<COLS;c++) { b->cells[r][c]=EMPTY; b->colors[r][c]=-1; }
}

int randType() { return rand() % NUM_TYPES; }

void loadShape(Piece *p, int type, int rot) {
    p->type=type; p->rotation=rot;
    for (int r=0;r<PSIZE;r++)
        for (int c=0;c<PSIZE;c++)
            p->shape[r][c]=PIECES[type][rot][r][c];
}

bool collision(int cells[ROWS][COLS], int shape[PSIZE][PSIZE], int row, int col) {
    for (int r=0;r<PSIZE;r++)
        for (int c=0;c<PSIZE;c++)
            if (shape[r][c]) {
                int nr=row+r, nc=col+c;
                if (nr<0||nr>=ROWS||nc<0||nc>=COLS) return true;
                if (cells[nr][nc]==LOCKED) return true;
            }
    return false;
}

bool moveLeft(Board *b) {
    if (!collision(b->cells,b->current.shape,b->current.row,b->current.col-1))
        { b->current.col--; return true; }
    return false;
}
bool moveRight(Board *b) {
    if (!collision(b->cells,b->current.shape,b->current.row,b->current.col+1))
        { b->current.col++; return true; }
    return false;
}
bool moveDown(Board *b) {
    if (!collision(b->cells,b->current.shape,b->current.row+1,b->current.col))
        { b->current.row++; return true; }
    return false;
}
void hardDrop(Board *b)  { while (moveDown(b)); }

void rotatePiece(Board *b) {
    int nr=(b->current.rotation+1)%NUM_ROTS;
    int tmp[PSIZE][PSIZE];
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++) tmp[r][c]=PIECES[b->current.type][nr][r][c];
    int kicks[3][2]={{0,0},{0,1},{0,-1}};
    for (int i=0;i<3;i++) {
        int tr=b->current.row+kicks[i][0], tc=b->current.col+kicks[i][1];
        if (!collision(b->cells,tmp,tr,tc)) {
            b->current.rotation=nr; b->current.row=tr; b->current.col=tc;
            for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++) b->current.shape[r][c]=tmp[r][c];
            return;
        }
    }
}

void lockPiece(Board *b) {
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++)
        if (b->current.shape[r][c]) {
            int br=b->current.row+r, bc=b->current.col+c;
            if (br>=0&&br<ROWS&&bc>=0&&bc<COLS)
                { b->cells[br][bc]=LOCKED; b->colors[br][bc]=b->current.type; }
        }
}

bool isLineFull(int cells[ROWS][COLS], int row) {
    for (int c=0;c<COLS;c++) if (cells[row][c]==EMPTY) return false;
    return true;
}
void removeLine(Board *b, int row) {
    for (int r=row;r>0;r--) for (int c=0;c<COLS;c++)
        { b->cells[r][c]=b->cells[r-1][c]; b->colors[r][c]=b->colors[r-1][c]; }
    for (int c=0;c<COLS;c++) { b->cells[0][c]=EMPTY; b->colors[0][c]=-1; }
}
int clearLines(Board *b) {
    int cleared=0; int r=ROWS-1;
    while (r>=0) { if (isLineFull(b->cells,r)){removeLine(b,r);cleared++;} else r--; }
    return cleared;
}
void updateScore(Board *b, int lines) {
    int bonus[5]={0,100,300,500,800};
    if (lines>=1&&lines<=4) {
        b->score+=bonus[lines]*b->level;
        b->totalLines+=lines;
        b->level=b->totalLines/10+1;
    }
}
void spawnPiece(Board *b) {
    b->current=b->next; b->current.row=0; b->current.col=COLS/2-2;
    loadShape(&b->next,randType(),0); b->next.row=b->next.col=0;
}
void initGame(Board *b) {
    initBoard(b);
    b->score=0; b->level=1; b->totalLines=0;
    b->isOver=false; b->isPaused=false;
    loadShape(&b->next,randType(),0); b->next.row=b->next.col=0;
    spawnPiece(b);
}
float dropInterval(int level) { float t=0.8f-(level-1)*0.07f; return t<0.1f?0.1f:t; }
int ghostRow(Board *b) {
    int g=b->current.row;
    while (!collision(b->cells,b->current.shape,g+1,b->current.col)) g++;
    return g;
}

// ============================================================
// AI LOGIC (Heuristic Tetris AI)
// ============================================================

// Danh gia chat luong bang (diem cao = ban tot hon)
float evalBoard(int cells[ROWS][COLS]) {
    int colH[COLS]={0};
    for (int c=0;c<COLS;c++)
        for (int r=0;r<ROWS;r++)
            if (cells[r][c]==LOCKED) { colH[c]=ROWS-r; break; }
    int aggH=0, holes=0, bump=0, lines=0;
    for (int c=0;c<COLS;c++) {
        aggH+=colH[c];
        if (c<COLS-1) bump+=abs(colH[c]-colH[c+1]);
        bool started=false;
        for (int r=0;r<ROWS;r++) {
            if (cells[r][c]==LOCKED) started=true;
            else if (started) holes++;
        }
    }
    for (int r=0;r<ROWS;r++) {
        bool full=true;
        for (int c=0;c<COLS;c++) if(cells[r][c]==EMPTY){full=false;break;}
        if (full) lines++;
    }
    return -0.51f*aggH + 0.76f*lines - 0.36f*holes - 0.18f*bump;
}

// Mo phong dat khoi va tinh diem
float simPlace(int cells[ROWS][COLS], int shape[PSIZE][PSIZE], int col) {
    int row=0;
    while (!collision(cells,shape,row+1,col)) row++;
    int sim[ROWS][COLS];
    for (int r=0;r<ROWS;r++) for (int c=0;c<COLS;c++) sim[r][c]=cells[r][c];
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++)
        if (shape[r][c]&&row+r>=0&&row+r<ROWS&&col+c>=0&&col+c<COLS)
            sim[row+r][col+c]=LOCKED;
    return evalBoard(sim);
}

// Tim nuoc di tot nhat (rotation + col)
void findBestMove(int cells[ROWS][COLS], Piece *p, int *bestRot, int *bestCol) {
    float best=-1e9f;
    *bestRot=p->rotation; *bestCol=p->col;
    for (int rot=0;rot<NUM_ROTS;rot++) {
        int shape[PSIZE][PSIZE];
        for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++) shape[r][c]=PIECES[p->type][rot][r][c];
        for (int col=0;col<COLS;col++) {
            if (!collision(cells,shape,0,col)) {
                float s=simPlace(cells,shape,col);
                if (s>best) { best=s; *bestRot=rot; *bestCol=col; }
            }
        }
    }
}

void initAI(AIData *ai, int difficulty) {
    initGame(&ai->board);
    ai->dropAccum=0; ai->thinkTimer=0;
    ai->difficulty=difficulty;
    ai->moveReady=false;
    ai->targetCol=COLS/2-2; ai->targetRot=0;
}

// Thoi gian suy nghi giua moi buoc theo do kho
float aiDelay(int diff) {
    float d[6]={0.45f,0.32f,0.20f,0.12f,0.07f,0.03f};
    return d[diff-1];
}

// Cap nhat AI moi frame
void updateAI(AIData *ai, float dt) {
    if (ai->board.isOver) return;

    // Tinh nuoc di khi khoi moi xuat hien
    if (!ai->moveReady) {
        int rndChance=(6-ai->difficulty)*13; // diff1=65%, diff6=0% random
        if (rand()%100 < rndChance) {
            ai->targetRot=rand()%NUM_ROTS;
            ai->targetCol=rand()%COLS;
        } else {
            findBestMove(ai->board.cells,&ai->board.current,&ai->targetRot,&ai->targetCol);
        }
        ai->moveReady=true;
    }

    // Thuc hien nuoc di tung buoc
    ai->thinkTimer+=dt;
    if (ai->thinkTimer >= aiDelay(ai->difficulty)) {
        ai->thinkTimer=0;
        bool rotOK=(ai->board.current.rotation==ai->targetRot);
        bool colOK=(ai->board.current.col==ai->targetCol);
        if (!rotOK) {
            rotatePiece(&ai->board);
        } else if (!colOK) {
            if (ai->board.current.col < ai->targetCol) moveRight(&ai->board);
            else moveLeft(&ai->board);
        } else {
            // Da can chinh - ha va khoa
            hardDrop(&ai->board);
            lockPiece(&ai->board);
            int lines=clearLines(&ai->board);
            int bonus[5]={0,100,300,500,800};
            if (lines>=1&&lines<=4) {
                ai->board.score+=bonus[lines]*ai->board.level;
                ai->board.totalLines+=lines;
                ai->board.level=ai->board.totalLines/10+1;
            }
            spawnPiece(&ai->board);
            ai->moveReady=false;
            if (collision(ai->board.cells,ai->board.current.shape,ai->board.current.row,ai->board.current.col))
                ai->board.isOver=true;
        }
    }

    // Trong luc di chuyen, khoi van roi tu nhien
    ai->dropAccum+=dt;
    float dint=dropInterval(ai->board.level);
    if (ai->dropAccum>=dint) {
        ai->dropAccum=0;
        if (!moveDown(&ai->board)) {
            lockPiece(&ai->board);
            int lines=clearLines(&ai->board);
            int bonus[5]={0,100,300,500,800};
            if (lines>=1&&lines<=4) {
                ai->board.score+=bonus[lines]*ai->board.level;
                ai->board.totalLines+=lines;
                ai->board.level=ai->board.totalLines/10+1;
            }
            spawnPiece(&ai->board);
            ai->moveReady=false;
            if (collision(ai->board.cells,ai->board.current.shape,ai->board.current.row,ai->board.current.col))
                ai->board.isOver=true;
        }
    }
}

// ============================================================
// HE THONG GIAI DAU (TOURNAMENT)
// ============================================================
void initTourData(TourData *t) {
    t->round=0; t->playerWins=0; t->coinsEarned=0;
    t->roundOver=false; t->playerWon=false; t->coinsAwarded=false;
}

void checkRoundEnd(Board *player, AIData *ai, TourData *t) {
    if (t->roundOver) return;
    int target=T_TARGETS[t->round];
    // Nguoi thang: dat du hang hoac AI thua
    if (player->totalLines>=target || ai->board.isOver) {
        t->roundOver=true; t->playerWon=true; t->coinsAwarded=false;
    }
    // Nguoi thua: game over truoc khi dat muc tieu
    if (player->isOver && !ai->board.isOver) {
        t->roundOver=true; t->playerWon=false; t->coinsAwarded=false;
    }
}

// ============================================================
// LUU / TAI PROFILE (Save/Load)
// ============================================================
void saveProfile(PlayerProfile *p) {
    FILE *f=fopen("save.dat","w");
    if (!f) return;
    fprintf(f,"HISCORE %d\n",p->hiScore);
    fprintf(f,"COINS %d\n",p->coins);
    fprintf(f,"SKIN %d\n",p->skin);
    fprintf(f,"UNLOCKED");
    for (int i=0;i<NUM_SKINS;i++) fprintf(f," %d",p->unlocked[i]);
    fprintf(f,"\n");
    fclose(f);
}

void loadProfile(PlayerProfile *p) {
    p->hiScore=0; p->coins=0; p->skin=0;
    for (int i=0;i<NUM_SKINS;i++) p->unlocked[i]=(i==0)?1:0;
    FILE *f=fopen("save.dat","r");
    if (!f) return;
    char key[32];
    while (fscanf(f,"%31s",key)==1) {
        if      (strcmp(key,"HISCORE" )==0) fscanf(f,"%d",&p->hiScore);
        else if (strcmp(key,"COINS"   )==0) fscanf(f,"%d",&p->coins);
        else if (strcmp(key,"SKIN"    )==0) fscanf(f,"%d",&p->skin);
        else if (strcmp(key,"UNLOCKED")==0)
            for (int i=0;i<NUM_SKINS;i++) fscanf(f,"%d",&p->unlocked[i]);
    }
    fclose(f);
    // Clamp skin to valid unlocked value
    if (p->skin<0||p->skin>=NUM_SKINS||!p->unlocked[p->skin]) p->skin=0;
}

// ============================================================
// HO TRO VE (RENDER HELPERS)
// ============================================================

// Effect scanline (CRT monitor)
void drawScanlines() {
    for (int y=0;y<WIN_H;y+=3)
        DrawLine(0,y,WIN_W,y,Color{0,0,0,22});
}

// Ve mot o gan theo skin
void drawCell(int x, int y, Color f, int skin, float t) {
    unsigned char dr=(unsigned char)(f.r/3), dg=(unsigned char)(f.g/3), db=(unsigned char)(f.b/3);
    switch (skin) {
        case 0: // Classic
            DrawRectangle(x,y,CELL,CELL,f);
            DrawRectangleLines(x,y,CELL,CELL,Color{dr,dg,db,255});
            break;
        case 1: // Neon
            DrawRectangle(x+2,y+2,CELL-4,CELL-4,f);
            DrawRectangleLines(x,y,CELL,CELL,Color{255,255,255,140});
            DrawRectangleLines(x+1,y+1,CELL-2,CELL-2,Color{f.r,f.g,f.b,200});
            break;
        case 2: // Retro
            DrawRectangle(x,y,CELL,CELL,Color{dr,dg,db,255});
            DrawRectangle(x+2,y+2,CELL-4,CELL-4,f);
            DrawRectangle(x+2,y+2,CELL-4,3,Color{255,255,255,90});
            break;
        case 3: // Shadow
            DrawRectangle(x+3,y+3,CELL,CELL,Color{0,0,0,80});
            DrawRectangle(x,y,CELL,CELL,f);
            DrawRectangleLines(x,y,CELL,CELL,Color{dr,dg,db,255});
            break;
        case 4: { // Dark pulse
            Color dark={dr,dg,db,255};
            DrawRectangle(x,y,CELL,CELL,dark);
            float pulse=(sinf(t*3.0f)+1.0f)/2.0f;
            unsigned char pr=(unsigned char)(f.r), pg=(unsigned char)(f.g*pulse), pb=(unsigned char)(f.b*(1.0f-pulse));
            DrawRectangleLines(x,y,CELL,CELL,Color{pr,pg,pb,210});
            DrawRectangleLines(x+2,y+2,CELL-4,CELL-4,Color{f.r,f.g,f.b,80});
            break;
        }
        default:
            DrawRectangle(x,y,CELL,CELL,f);
            DrawRectangleLines(x,y,CELL,CELL,Color{50,50,50,255});
    }
}

// Ve luoi o trong
void drawGrid(int bx, int by) {
    Color empty=Color{18,18,42,255};
    Color grid =Color{32,32,68,255};
    for (int r=0;r<ROWS;r++) for (int c=0;c<COLS;c++) {
        int x=bx+c*CELL, y=by+r*CELL;
        DrawRectangle(x,y,CELL,CELL,empty);
        DrawRectangleLines(x,y,CELL,CELL,grid);
    }
}

// Ve noi dung bang (ghost + locked + active)
void drawBoardContent(int bx, int by, Board *b, int skin, float t) {
    // O da khoa
    for (int r=0;r<ROWS;r++) for (int c=0;c<COLS;c++)
        if (b->cells[r][c]==LOCKED) {
            int ci=b->colors[r][c];
            Color f=(ci>=0)?PIECE_COLORS[ci]:GRAY;
            drawCell(bx+c*CELL,by+r*CELL,f,skin,t);
        }
    if (b->isOver) return;
    // Ghost piece
    int gr=ghostRow(b);
    Color gc=PIECE_COLORS[b->current.type]; gc.a=50;
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++)
        if (b->current.shape[r][c]) {
            int br=gr+r, bc=b->current.col+c;
            if (br>=0&&br<ROWS&&bc>=0&&bc<COLS)
                DrawRectangle(bx+bc*CELL,by+br*CELL,CELL,CELL,gc);
        }
    // Khoi dang roi
    Color af=PIECE_COLORS[b->current.type];
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++)
        if (b->current.shape[r][c]) {
            int br=b->current.row+r, bc=b->current.col+c;
            if (br>=0&&br<ROWS&&bc>=0&&bc<COLS)
                drawCell(bx+bc*CELL,by+br*CELL,af,skin,t);
        }
}

void drawBoardBorder(int bx, int by, Color bc) {
    DrawRectangleLines(bx-2,by-2,COLS*CELL+4,ROWS*CELL+4,bc);
    DrawRectangleLines(bx-3,by-3,COLS*CELL+6,ROWS*CELL+6,Color{bc.r,bc.g,bc.b,60});
}

void drawNextPiece(int px, int py, Piece *next, int skin, float t) {
    DrawText("NEXT",px,py,12,LIGHTGRAY);
    int ns=20;
    Color nf=PIECE_COLORS[next->type];
    for (int r=0;r<PSIZE;r++) for (int c=0;c<PSIZE;c++)
        if (next->shape[r][c])
            drawCell(px+c*ns,py+18+r*ns,nf,skin,t);
}

// Panel thong tin cho che do don
void drawInfoPanel(int px, int py, Board *b, PlayerProfile *p, float t) {
    char buf[32];
    float glow=(float)((int)(GetTime()*2)%2);
    Color titleC = Color{(unsigned char)(80+glow*30),200,255,255};
    DrawText("TETRIS",px,py,30,titleC);
    DrawLine(px,py+38,px+165,py+38,Color{60,60,130,255});

    DrawText("HI-SCORE",px,py+50,11,Color{200,180,80,255});
    snprintf(buf,sizeof(buf),"%d",p->hiScore);
    DrawText(buf,px,py+64,18,Color{255,215,0,255});

    DrawText("SCORE",px,py+96,11,LIGHTGRAY);
    snprintf(buf,sizeof(buf),"%d",b->score);
    DrawText(buf,px,py+110,22,WHITE);

    DrawText("LEVEL",px,py+146,11,LIGHTGRAY);
    snprintf(buf,sizeof(buf),"%d",b->level);
    DrawText(buf,px,py+160,22,Color{255,220,50,255});

    DrawText("LINES",px,py+196,11,LIGHTGRAY);
    snprintf(buf,sizeof(buf),"%d",b->totalLines);
    DrawText(buf,px,py+210,22,Color{80,255,140,255});

    DrawText("COINS",px,py+246,11,LIGHTGRAY);
    snprintf(buf,sizeof(buf),"%d",p->coins);
    DrawText(buf,px,py+260,18,Color{255,200,50,255});

    DrawLine(px,py+292,px+165,py+292,Color{60,60,130,255});
    drawNextPiece(px,py+300,&b->next,p->skin,t);
    DrawLine(px,py+400,px+165,py+400,Color{60,60,130,255});

    Color ch=Color{130,130,160,255};
    DrawText("CONTROLS",px,py+410,11,LIGHTGRAY);
    DrawText("[<][>] Di chuyen", px,py+425,10,ch);
    DrawText("[^]    Xoay",      px,py+439,10,ch);
    DrawText("[v]    Nhanh",     px,py+453,10,ch);
    DrawText("[Spc]  Hard Drop", px,py+467,10,ch);
    DrawText("[P]    Pause",     px,py+481,10,ch);
    DrawText("[R]    Restart",   px,py+495,10,ch);
    DrawText("[Esc]  Menu",      px,py+509,10,ch);
}

// ============================================================
// MAN HINH: MENU CHINH
// ============================================================
void renderMenu(int sel, PlayerProfile *p) {
    ClearBackground(Color{6,6,18,255});
    drawScanlines();
    float t=(float)GetTime();
    // Title
    unsigned char glow=(unsigned char)(180+75*sinf(t*1.8f));
    int tw=MeasureText("TETRIS",88);
    DrawText("TETRIS",WIN_W/2-tw/2+3,103,88,Color{0,0,80,255}); // shadow
    DrawText("TETRIS",WIN_W/2-tw/2,  100,88,Color{0,glow,255,255});
    DrawText("ARCADE CLASSIC",WIN_W/2-MeasureText("ARCADE CLASSIC",18)/2,195,18,Color{140,140,200,255});
    char buf[48];
    snprintf(buf,sizeof(buf),"HI-SCORE: %d",p->hiScore);
    DrawText(buf,WIN_W/2-MeasureText(buf,16)/2,224,16,Color{255,215,0,255});
    snprintf(buf,sizeof(buf),"COINS: %d",p->coins);
    DrawText(buf,WIN_W/2-MeasureText(buf,14)/2,246,14,Color{200,180,50,255});
    // Options
    const char* opts[]={"> CHOI DON","> GIAI DAU (6 VONG)","> CUA HANG SKIN","> THOAT"};
    for (int i=0;i<4;i++) {
        int y=290+i*62;
        bool s=(i==sel);
        Color bg =s?Color{35,75,160,230}:Color{16,16,44,200};
        Color brd=s?Color{80,140,255,255}:Color{35,35,75,255};
        Color tc =s?WHITE:Color{140,140,185,255};
        DrawRectangle(WIN_W/2-160,y-8,320,46,bg);
        DrawRectangleLines(WIN_W/2-160,y-8,320,46,brd);
        DrawText(opts[i],WIN_W/2-MeasureText(opts[i],20)/2,y+8,20,tc);
    }
    DrawText("[UP]/[DOWN] chon  [ENTER] xac nhan",WIN_W/2-MeasureText("[UP]/[DOWN] chon  [ENTER] xac nhan",12)/2,WIN_H-30,12,Color{70,70,110,255});
}

// ============================================================
// MAN HINH: CHOI DON
// ============================================================
void renderGame(Board *b, PlayerProfile *p, float t) {
    ClearBackground(Color{6,6,18,255});
    drawGrid(SP_BX,SP_BY);
    drawBoardContent(SP_BX,SP_BY,b,p->skin,t);
    drawBoardBorder(SP_BX,SP_BY,Color{70,100,200,255});
    drawInfoPanel(SP_PX,SP_BY,b,p,t);
    if (b->isPaused && !b->isOver) {
        DrawRectangle(SP_BX,SP_BY,COLS*CELL,ROWS*CELL,Color{0,0,0,165});
        const char* pt="TAM DUNG";
        DrawText(pt,SP_BX+COLS*CELL/2-MeasureText(pt,26)/2,SP_BY+ROWS*CELL/2-28,26,Color{255,220,50,255});
        DrawText("Nhan [P] tiep tuc",SP_BX+48,SP_BY+ROWS*CELL/2+12,14,LIGHTGRAY);
    }
    if (b->isOver) {
        DrawRectangle(SP_BX,SP_BY,COLS*CELL,ROWS*CELL,Color{0,0,0,180});
        const char* go="GAME OVER";
        DrawText(go,SP_BX+COLS*CELL/2-MeasureText(go,28)/2,SP_BY+ROWS*CELL/2-42,28,Color{255,55,55,255});
        DrawText("[R] Choi lai",SP_BX+60,SP_BY+ROWS*CELL/2+8, 16,WHITE);
        DrawText("[Esc] Menu",  SP_BX+70,SP_BY+ROWS*CELL/2+32,14,LIGHTGRAY);
    }
    drawScanlines();
}

// ============================================================
// MAN HINH: GIAI DAU
// ============================================================
void renderTournament(Board *player, AIData *ai, TourData *t, PlayerProfile *p, float gt) {
    ClearBackground(Color{6,6,18,255});
    // Header
    char buf[64];
    snprintf(buf,sizeof(buf),"GIAI DAU  -  VONG %d / 6",t->round+1);
    DrawText(buf,WIN_W/2-MeasureText(buf,20)/2,8,20,Color{255,200,50,255});
    const char* aiNames[]={"ROOKIE","AMATEUR","SEMI-PRO","PRO","EXPERT","MASTER"};
    snprintf(buf,sizeof(buf),"Doi thu: %s  |  Muc tieu: xoa %d hang",aiNames[t->round],T_TARGETS[t->round]);
    DrawText(buf,WIN_W/2-MeasureText(buf,13)/2,34,13,Color{150,200,150,255});

    // Left board - player
    drawGrid(T_LBX,T_BY);
    drawBoardContent(T_LBX,T_BY,player,p->skin,gt);
    drawBoardBorder(T_LBX,T_BY,Color{60,160,255,255});
    DrawText("NGUOI CHOI",T_LBX,T_BY-22,14,Color{60,160,255,255});

    // Right board - AI
    drawGrid(T_RBX,T_BY);
    drawBoardContent(T_RBX,T_BY,&ai->board,0,gt);
    drawBoardBorder(T_RBX,T_BY,Color{255,70,70,255});
    DrawText(aiNames[t->round],T_RBX,T_BY-22,14,Color{255,70,70,255});

    // Middle info panel
    int mx=T_MID; int my=T_BY;
    DrawText("VS",mx+50,my+ROWS*CELL/2-22,36,Color{255,255,255,180});

    // Player progress
    DrawText("NGUOI:",mx,my+55,11,Color{100,180,255,255});
    snprintf(buf,sizeof(buf),"%d / %d hang",player->totalLines,T_TARGETS[t->round]);
    DrawText(buf,mx,my+70,13,WHITE);
    snprintf(buf,sizeof(buf),"Diem: %d",player->score);
    DrawText(buf,mx,my+88,12,Color{100,180,255,255});
    // Progress bar player
    float pct=(float)player->totalLines/T_TARGETS[t->round]; if(pct>1)pct=1;
    DrawRectangle(mx,my+108,120,10,Color{30,30,80,255});
    DrawRectangle(mx,my+108,(int)(120*pct),10,Color{60,160,255,255});

    DrawText("AI:",mx,my+130,11,Color{255,100,100,255});
    snprintf(buf,sizeof(buf),"%d / %d hang",ai->board.totalLines,T_TARGETS[t->round]);
    DrawText(buf,mx,my+145,13,WHITE);
    snprintf(buf,sizeof(buf),"Diem: %d",ai->board.score);
    DrawText(buf,mx,my+163,12,Color{255,100,100,255});
    // Progress bar AI
    float apct=(float)ai->board.totalLines/T_TARGETS[t->round]; if(apct>1)apct=1;
    DrawRectangle(mx,my+183,120,10,Color{30,30,80,255});
    DrawRectangle(mx,my+183,(int)(120*apct),10,Color{255,80,80,255});

    // Coin reward
    snprintf(buf,sizeof(buf),"Thuong: %d",T_COINS[t->round]);
    DrawText(buf,mx,my+210,13,Color{255,215,0,255});
    snprintf(buf,sizeof(buf),"Coins: %d",p->coins);
    DrawText(buf,mx,my+228,12,Color{200,180,50,255});

    DrawLine(mx,my+248,mx+130,my+248,Color{50,50,100,255});

    // Next pieces
    drawNextPiece(mx,my+258,&player->next,p->skin,gt);
    drawNextPiece(mx,my+368,&ai->board.next,0,gt);

    // Win/Lose overlay
    if (player->isOver) {
        DrawRectangle(T_LBX,T_BY,COLS*CELL,ROWS*CELL,Color{0,0,0,175});
        const char* lt="THUA!";
        DrawText(lt,T_LBX+COLS*CELL/2-MeasureText(lt,30)/2,T_BY+ROWS*CELL/2-18,30,Color{255,55,55,255});
    }
    if (ai->board.isOver) {
        DrawRectangle(T_RBX,T_BY,COLS*CELL,ROWS*CELL,Color{0,0,0,175});
        const char* wt="AI THUA!";
        DrawText(wt,T_RBX+COLS*CELL/2-MeasureText(wt,22)/2,T_BY+ROWS*CELL/2-14,22,Color{50,255,100,255});
    }
    drawScanlines();
}

// ============================================================
// MAN HINH: KET QUA VONG DAU
// ============================================================
void renderRoundResult(TourData *t, PlayerProfile *p) {
    ClearBackground(Color{6,6,18,255});
    drawScanlines();
    const char* title=t->playerWon?"CHIEN THANG!":"THUA CUOC!";
    Color tc=t->playerWon?Color{50,255,100,255}:Color{255,55,55,255};
    DrawText(title,WIN_W/2-MeasureText(title,42)/2,130,42,tc);
    char buf[64];
    snprintf(buf,sizeof(buf),"Vong %d / 6",t->round+1);
    DrawText(buf,WIN_W/2-MeasureText(buf,18)/2,185,18,LIGHTGRAY);
    if (t->playerWon) {
        snprintf(buf,sizeof(buf),"+ %d COINS",T_COINS[t->round]);
        DrawText(buf,WIN_W/2-MeasureText(buf,30)/2,220,30,Color{255,215,0,255});
    }
    snprintf(buf,sizeof(buf),"Tong coins: %d",p->coins);
    DrawText(buf,WIN_W/2-MeasureText(buf,18)/2,268,18,Color{200,180,100,255});
    if (t->playerWon && t->round<ROUNDS-1) {
        snprintf(buf,sizeof(buf),"Tiep theo: Vong %d - %s",t->round+2,(t->round+1<ROUNDS)?"tiep":"chung ket");
        DrawText(buf,WIN_W/2-MeasureText(buf,15)/2,318,15,Color{140,200,255,255});
        DrawText("Nhan [ENTER] choi vong tiep",WIN_W/2-MeasureText("Nhan [ENTER] choi vong tiep",16)/2,358,16,Color{80,180,255,255});
    } else if (t->playerWon && t->round==ROUNDS-1) {
        DrawText("CHUC MUNG! Ban da chinh phuc tat ca 6 vong!",WIN_W/2-MeasureText("CHUC MUNG! Ban da chinh phuc tat ca 6 vong!",16)/2,318,16,Color{255,215,0,255});
        DrawText("Nhan [ENTER] ve menu",WIN_W/2-MeasureText("Nhan [ENTER] ve menu",16)/2,358,16,LIGHTGRAY);
    } else {
        DrawText("Giai dau ket thuc!",WIN_W/2-MeasureText("Giai dau ket thuc!",18)/2,318,18,LIGHTGRAY);
        DrawText("Nhan [ENTER] ve menu",WIN_W/2-MeasureText("Nhan [ENTER] ve menu",16)/2,358,16,Color{80,180,255,255});
    }
}

// ============================================================
// MAN HINH: CUA HANG
// ============================================================
void renderShop(int sel, PlayerProfile *p, float t) {
    ClearBackground(Color{6,6,18,255});
    drawScanlines();
    DrawText("CUA HANG SKIN",WIN_W/2-MeasureText("CUA HANG SKIN",32)/2,28,32,Color{255,200,50,255});
    char buf[32];
    snprintf(buf,sizeof(buf),"COINS CUA BAN: %d",p->coins);
    DrawText(buf,WIN_W/2-MeasureText(buf,15)/2,72,15,Color{255,215,0,255});
    for (int i=0;i<NUM_SKINS;i++) {
        int y=108+i*100;
        bool s=(i==sel);
        bool owned=(p->unlocked[i]==1);
        bool equipped=(p->skin==i);
        Color bg =s?Color{35,70,155,220}:Color{14,14,38,200};
        Color brd=s?Color{80,140,255,255}:Color{32,32,72,255};
        DrawRectangle(50,y,WIN_W-100,88,bg);
        DrawRectangleLines(50,y,WIN_W-100,88,brd);
        DrawText(SKIN_NAMES[i],75,y+12,20,owned?WHITE:Color{100,100,130,255});
        DrawText(SKIN_DESC[i], 75,y+38,13,Color{140,140,170,255});
        // Preview 2 cells
        Color pf=PIECE_COLORS[1];
        drawCell(WIN_W-210,y+18,pf,i,t);
        drawCell(WIN_W-210+CELL,y+18,PIECE_COLORS[2],i,t);
        // Status
        if (equipped) {
            DrawText("[DANG SU DUNG]",WIN_W-390,y+28,14,Color{50,255,100,255});
        } else if (owned) {
            DrawText("[ENTER] Trang bi",WIN_W-400,y+28,13,Color{80,200,255,255});
        } else {
            snprintf(buf,sizeof(buf),"%d COINS",SKIN_PRICES[i]);
            Color pc=(p->coins>=SKIN_PRICES[i])?Color{255,215,0,255}:Color{160,60,60,255};
            DrawText(buf,WIN_W-380,y+22,14,pc);
            if (p->coins>=SKIN_PRICES[i])
                DrawText("[ENTER] Mua",WIN_W-380,y+44,12,Color{80,255,130,255});
        }
    }
    DrawText("[UP]/[DOWN] chon   [ENTER] mua/trang bi   [ESC] quay lai",
             WIN_W/2-MeasureText("[UP]/[DOWN] chon   [ENTER] mua/trang bi   [ESC] quay lai",12)/2,WIN_H-28,12,Color{70,70,110,255});
}

// ============================================================
// XU LY INPUT
// ============================================================

// Ham chung: xu ly phim di chuyen (dung cho ca 2 che do)
void applyMovement(Board *b, float dt, float *dropAccum) {
    if (b->isOver||b->isPaused) return;
    if (IsKeyPressed(KEY_UP))    rotatePiece(b);
    if (IsKeyPressed(KEY_SPACE)) { hardDrop(b); *dropAccum=0; }
    // Soft drop
    if (IsKeyDown(KEY_DOWN)) {
        s_soft+=dt;
        if (s_soft>=SD_S){s_soft-=SD_S;moveDown(b);}
    } else s_soft=0;
    // DAS
    int hdir=0;
    if (IsKeyDown(KEY_LEFT))  hdir=-1;
    if (IsKeyDown(KEY_RIGHT)) hdir=1;
    if (IsKeyPressed(KEY_LEFT))  {moveLeft(b);  s_hdir=-1;s_das=0;s_rep=0;return;}
    if (IsKeyPressed(KEY_RIGHT)) {moveRight(b); s_hdir=1; s_das=0;s_rep=0;return;}
    if (hdir!=0&&hdir==s_hdir) {
        s_das+=dt;
        if (s_das>=DAS_D){s_rep+=dt; if(s_rep>=DAS_S){s_rep-=DAS_S;if(hdir<0)moveLeft(b);else moveRight(b);}}
    } else {s_hdir=hdir;s_das=0;s_rep=0;}
}

void handleMenuInput(int *scr, int *sel, PlayerProfile *p, Board *b, AIData *ai, TourData *t, float *dAcc) {
    if (IsKeyPressed(KEY_UP))   {(*sel)--;if(*sel<0)*sel=3;}
    if (IsKeyPressed(KEY_DOWN)) {(*sel)++;if(*sel>3)*sel=0;}
    if (IsKeyPressed(KEY_ENTER)) {
        s_das=0;s_rep=0;s_soft=0;s_hdir=0;*dAcc=0;
        switch (*sel) {
            case 0: initGame(b);                          *scr=SCR_PLAY; break;
            case 1: initTourData(t);initGame(b);initAI(ai,1); *scr=SCR_TOUR; break;
            case 2:                                       *scr=SCR_SHOP; break;
            case 3: CloseWindow(); break;
        }
    }
}

void handlePlayInput(Board *b, float dt, float *dAcc, int *scr, PlayerProfile *p) {
    if (b->isOver) {
        if (IsKeyPressed(KEY_R)) {
            if (b->score>p->hiScore){p->hiScore=b->score;saveProfile(p);}
            initGame(b);*dAcc=0;s_das=0;s_rep=0;s_soft=0;s_hdir=0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (b->score>p->hiScore){p->hiScore=b->score;saveProfile(p);}
            *scr=SCR_MENU;
        }
        return;
    }
    if (IsKeyPressed(KEY_P)) {b->isPaused=!b->isPaused;return;}
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (b->score>p->hiScore){p->hiScore=b->score;saveProfile(p);}
        *scr=SCR_MENU;return;
    }
    applyMovement(b,dt,dAcc);
}

void handleTourInput(Board *player, AIData *ai, float dt, float *dAcc, TourData *t, int *scr, PlayerProfile *p) {
    // Xu ly man ket qua
    if (t->roundOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            if (t->playerWon && t->round<ROUNDS-1) {
                t->round++;
                initGame(player);
                initAI(ai, t->round+1);
                *dAcc=0;s_das=0;s_rep=0;s_soft=0;s_hdir=0;
                t->roundOver=false;t->coinsAwarded=false;
            } else {
                *scr=SCR_MENU;saveProfile(p);
            }
        }
        return;
    }
    // Trong vong dau
    if (IsKeyPressed(KEY_ESCAPE)) {*scr=SCR_MENU;saveProfile(p);return;}
    if (IsKeyPressed(KEY_P)) {player->isPaused=!player->isPaused;return;}
    if (!player->isOver) applyMovement(player,dt,dAcc);
}

void handleShopInput(int *sel, PlayerProfile *p, int *scr) {
    if (IsKeyPressed(KEY_ESCAPE)) {*scr=SCR_MENU;return;}
    if (IsKeyPressed(KEY_UP))   {(*sel)--;if(*sel<0)*sel=NUM_SKINS-1;}
    if (IsKeyPressed(KEY_DOWN)) {(*sel)++;if(*sel>=NUM_SKINS)*sel=0;}
    if (IsKeyPressed(KEY_ENTER)) {
        int i=*sel;
        if (p->unlocked[i]) {
            p->skin=i;saveProfile(p);
        } else if (p->coins>=SKIN_PRICES[i]) {
            p->coins-=SKIN_PRICES[i];
            p->unlocked[i]=1;
            p->skin=i;
            saveProfile(p);
        }
    }
}

// ============================================================
// MAIN
// ============================================================
int main() {
    srand((unsigned int)time(NULL));
    InitWindow(WIN_W,WIN_H,"TETRIS v3 - Arcade Classic");
    SetTargetFPS(60);

    PlayerProfile profile; loadProfile(&profile);

    Board      playerBoard;
    AIData     aiData;
    TourData   tourData;
    initGame(&playerBoard);
    initTourData(&tourData);
    initAI(&aiData,1);

    int   screen  = SCR_MENU;
    int   menuSel = 0;
    int   shopSel = 0;
    float dropAcc = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float t  = (float)GetTime();

        // ---- INPUT ----
        switch (screen) {
            case SCR_MENU: handleMenuInput(&screen,&menuSel,&profile,&playerBoard,&aiData,&tourData,&dropAcc); break;
            case SCR_PLAY: handlePlayInput(&playerBoard,dt,&dropAcc,&screen,&profile); break;
            case SCR_TOUR: handleTourInput(&playerBoard,&aiData,dt,&dropAcc,&tourData,&screen,&profile); break;
            case SCR_SHOP: handleShopInput(&shopSel,&profile,&screen); break;
        }

        // ---- UPDATE ----
        // Che do don: ha khoi tu dong
        if (screen==SCR_PLAY && !playerBoard.isOver && !playerBoard.isPaused) {
            dropAcc+=dt;
            if (dropAcc>=dropInterval(playerBoard.level)) {
                dropAcc=0;
                if (!moveDown(&playerBoard)) {
                    lockPiece(&playerBoard);
                    int lines=clearLines(&playerBoard);
                    updateScore(&playerBoard,lines);
                    profile.coins+=lines*2; // 2 coins moi hang xoa
                    if (playerBoard.score>profile.hiScore) profile.hiScore=playerBoard.score;
                    spawnPiece(&playerBoard);
                    if (collision(playerBoard.cells,playerBoard.current.shape,playerBoard.current.row,playerBoard.current.col))
                        playerBoard.isOver=true;
                }
            }
        }

        // Giai dau: cap nhat ca hai ban
        if (screen==SCR_TOUR && !tourData.roundOver) {
            if (!playerBoard.isOver && !playerBoard.isPaused) {
                dropAcc+=dt;
                if (dropAcc>=dropInterval(playerBoard.level)) {
                    dropAcc=0;
                    if (!moveDown(&playerBoard)) {
                        lockPiece(&playerBoard);
                        int lines=clearLines(&playerBoard);
                        updateScore(&playerBoard,lines);
                        profile.coins+=lines;
                        spawnPiece(&playerBoard);
                        if (collision(playerBoard.cells,playerBoard.current.shape,playerBoard.current.row,playerBoard.current.col))
                            playerBoard.isOver=true;
                    }
                }
            }
            updateAI(&aiData,dt);
            checkRoundEnd(&playerBoard,&aiData,&tourData);
            // Cap nhat coin khi vong vua ket thuc (chi 1 lan)
            if (tourData.roundOver && !tourData.coinsAwarded) {
                if (tourData.playerWon) {
                    profile.coins+=T_COINS[tourData.round];
                    tourData.coinsEarned+=T_COINS[tourData.round];
                }
                tourData.coinsAwarded=true;
                saveProfile(&profile);
            }
        }

        // ---- RENDER ----
        BeginDrawing();
        switch (screen) {
            case SCR_MENU: renderMenu(menuSel,&profile); break;
            case SCR_PLAY: renderGame(&playerBoard,&profile,t); break;
            case SCR_TOUR:
                if (tourData.roundOver) renderRoundResult(&tourData,&profile);
                else                   renderTournament(&playerBoard,&aiData,&tourData,&profile,t);
                break;
            case SCR_SHOP: renderShop(shopSel,&profile,t); break;
        }
        EndDrawing();
    }

    // Luu truoc khi thoat
    if (playerBoard.score>profile.hiScore) profile.hiScore=playerBoard.score;
    saveProfile(&profile);
    CloseWindow();
    return 0;
}
