/*
 * Game Tetris - C++ don gian (khong huong doi tuong)
 * Bien dich (Windows, MinGW hoac MSVC):
 *   g++ tetris.cpp -o tetris.exe
 * Chay: tetris.exe
 *
 * Dieu khien:
 *   A / mui ten TRAI  : dich trai
 *   D / mui ten PHAI  : dich phai
 *   S / mui ten XUONG : roi nhanh
 *   W / mui ten LEN   : xoay khoi
 *   Q                 : thoat
 */

#include <iostream>  // Thu vien nhap xuat co ban (cout, cin)
#include <cstdlib>   // Thu vien ham he thong (system, rand, exit)
#include <ctime>     // Thu vien thoi gian (time, de tao so ngau nhien)
#include <cstring>   // Thu vien xu ly chuoi / mang (co the dung memset)

#ifdef _WIN32
#include <conio.h>   // Thu vien doc phim nhanh (_kbhit, _getch) tren Windows
#include <windows.h> // Thu vien Sleep, dieu khien console Windows
#endif

using namespace std;  // Dung namespace chuan (phong cach hoc sinh moi)

/* ========== HANG SO (CONSTANTS) ========== */

const int CHIEU_RONG = 10;   // So cot cua bàn chơi (chieu ngang)
const int CHIEU_CAO = 20;    // So hang cua bàn chơi (chieu doc)
const int KICH_THUOC_KHOI = 4;  // Moi khoi goc duoc bieu dien trong luoi 4x4
const int SO_LOAI_KHOI = 7;     // 7 loai khoi Tetris: I, O, T, S, Z, J, L
const int SO_HUONG_XOAY = 4;  // Moi khoi co toi da 4 huong xoay (0, 1, 2, 3)

/* ========== DU LIEU HINH KHOI (MA TRAN 4x4) ========== */
/*
 * duLieuHinh[loai][gocXoay][hang][cot] = 1 neu co o, 0 neu trong
 * Chi dung mang so nguyen - khong dung class
 */
int duLieuHinh[SO_LOAI_KHOI][SO_HUONG_XOAY][KICH_THUOC_KHOI][KICH_THUOC_KHOI] = {
    /* 0 - Khoi I (thanh doc/ngang) */
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}
    },
    /* 1 - Khoi O (vuong 2x2) */
    {
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
    },
    /* 2 - Khoi T */
    {
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    },
    /* 3 - Khoi S */
    {
        {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
    },
    /* 4 - Khoi Z */
    {
        {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}
    },
    /* 5 - Khoi J */
    {
        {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
    },
    /* 6 - Khoi L */
    {
        {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
        {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
    }
};

/* Ky tu hien thi tren console cho tung loai khoi da dat */
char kyTuKhoi[SO_LOAI_KHOI + 1] = {' ', 'I', 'O', 'T', 'S', 'Z', 'J', 'L'};

/* ========== BIEN TOAN CUC TRANG THAI GAME ========== */

int banCo[CHIEU_CAO][CHIEU_RONG];  // Ban choi: 0 = trong, 1-7 = loai khoi da dat
int diemSo = 0;                    // Diem nguoi choi
int capDo = 1;                     // Cap do (tang toc do roi)
int loaiKhoiHienTai = 0;           // Loai khoi dang roi (0..6)
int gocXoayHienTai = 0;            // Goc xoay hien tai (0..3)
int viTriX = 0;                    // Cot goc cua khoi dang roi
int viTriY = 0;                    // Hang goc cua khoi dang roi
bool troChoiKetThuc = false;       // Co bao game over khong

/* ========== KHAI BAO TRUOC CAC HAM (FUNCTION PROTOTYPES) ========== */

void xoaManHinh();                                              // Xoa console
void khoiTaoBanCo(int (*ban)[CHIEU_RONG]);                       // Dat tat ca o ve 0
void veKhungHuongDan();                                         // In huong dan phim
void veBanCo(const int (*ban)[CHIEU_RONG], int diem, int cap);  // Ve ban + khoi dang roi
bool kiemTraVaCham(const int (*ban)[CHIEU_RONG], int loai, int goc, int x, int y);
void ganKhoiVaoBan(int (*ban)[CHIEU_RONG], int loai, int goc, int x, int y);
int xoaHangDay(int (*ban)[CHIEU_RONG]);                         // Tra ve so hang da xoa
void tangDiem(int *conTroDiem, int soHangXoa);                  // Dung con tro de cap nhat diem
int taoSoNgauNhien(int minVal, int maxVal);                     // So ngau nhien trong khoang
bool taoKhoiMoi(int (*ban)[CHIEU_RONG]);                        // Sinh khoi moi, tra false neu game over
bool dichChuyen(int (*ban)[CHIEU_RONG], int dx, int dy);        // Dich khoi dang roi
bool xoayKhoiHienTai(int (*ban)[CHIEU_RONG]);                  // Xoay khoi neu khong va cham
bool roiXuongMotBuoc(int (*ban)[CHIEU_RONG]);                  // Ha 1 hang; khoa khoi neu cham
void xuLyPhim(int (*ban)[CHIEU_RONG]);                          // Doc phim nguoi choi
void tamDungGiay(int giay);                                     // Sleep don gian
int tinhThoiGianCho();                                          // Thoi gian giua cac buoc roi

/* ========== DINH NGHIA CAC HAM ========== */

void xoaManHinh() {
    // Goi lenh he dieu hanh de xoa man hinh console (Windows)
    system("cls");
}

void khoiTaoBanCo(int (*ban)[CHIEU_RONG]) {
    // ban la con tro toi mang 2 chieu: ban[hang][cot]
    int hang;  // Bien dem hang
    int cot;   // Bien dem cot
    for (hang = 0; hang < CHIEU_CAO; hang++) {       // Duyet tung hang
        for (cot = 0; cot < CHIEU_RONG; cot++) {     // Duyet tung cot trong hang
            ban[hang][cot] = 0;                      // Dat o trong (gia tri 0)
        }
    }
}

void veKhungHuongDan() {
    cout << "========== TETRIS (C++ don gian) ==========\n";
    cout << "A / <- : trai   D / -> : phai\n";
    cout << "S / v  : xuong   W / ^  : xoay\n";
    cout << "Q      : thoat game\n";
    cout << "===========================================\n\n";
}

void veBanCo(const int (*ban)[CHIEU_RONG], int diem, int cap) {
    // Mang tam de gop ban co dinh + khoi dang roi
    char hienThi[CHIEU_CAO][CHIEU_RONG];
    int hang, cot, i, j, loaiO, x, y;

    for (hang = 0; hang < CHIEU_CAO; hang++) {
        for (cot = 0; cot < CHIEU_RONG; cot++) {
            if (ban[hang][cot] == 0) {
                hienThi[hang][cot] = '.';  // O trong
            } else {
                hienThi[hang][cot] = kyTuKhoi[ban[hang][cot]];  // O da co khoi
            }
        }
    }

    // Ve khoi dang roi len mang hien thi
    for (i = 0; i < KICH_THUOC_KHOI; i++) {
        for (j = 0; j < KICH_THUOC_KHOI; j++) {
            if (duLieuHinh[loaiKhoiHienTai][gocXoayHienTai][i][j] == 1) {
                y = viTriY + i;
                x = viTriX + j;
                if (y >= 0 && y < CHIEU_CAO && x >= 0 && x < CHIEU_RONG) {
                    hienThi[y][x] = kyTuKhoi[loaiKhoiHienTai + 1];
                }
            }
        }
    }

    // In vien tren
    cout << "+";
    for (cot = 0; cot < CHIEU_RONG; cot++) {
        cout << "--";
    }
    cout << "+\n";

    // In tung hang ban co
    for (hang = 0; hang < CHIEU_CAO; hang++) {
        cout << "|";
        for (cot = 0; cot < CHIEU_RONG; cot++) {
            cout << hienThi[hang][cot] << ' ';
        }
        cout << "|\n";
    }

    // In vien duoi
    cout << "+";
    for (cot = 0; cot < CHIEU_RONG; cot++) {
        cout << "--";
    }
    cout << "+\n";

    cout << "Diem: " << diem << "  |  Cap do: " << cap << "\n\n";
}

bool kiemTraVaCham(const int (*ban)[CHIEU_RONG], int loai, int goc, int x, int y) {
    int i, j, cx, cy;
    for (i = 0; i < KICH_THUOC_KHOI; i++) {
        for (j = 0; j < KICH_THUOC_KHOI; j++) {
            if (duLieuHinh[loai][goc][i][j] == 1) {
                cx = x + j;  // Cot thuc tren ban co
                cy = y + i;  // Hang thuc tren ban co
                if (cx < 0 || cx >= CHIEU_RONG || cy >= CHIEU_CAO) {
                    return true;  // Vuot bien -> co va cham
                }
                if (cy >= 0 && ban[cy][cx] != 0) {
                    return true;  // Dam vao khoi da dat
                }
            }
        }
    }
    return false;  // Khong va cham
}

void ganKhoiVaoBan(int (*ban)[CHIEU_RONG], int loai, int goc, int x, int y) {
    int i, j, cx, cy;
    for (i = 0; i < KICH_THUOC_KHOI; i++) {
        for (j = 0; j < KICH_THUOC_KHOI; j++) {
            if (duLieuHinh[loai][goc][i][j] == 1) {
                cx = x + j;
                cy = y + i;
                if (cy >= 0 && cy < CHIEU_CAO && cx >= 0 && cx < CHIEU_RONG) {
                    ban[cy][cx] = loai + 1;  // Luu loai khoi (1..7) tren ban
                }
            }
        }
    }
}

int xoaHangDay(int (*ban)[CHIEU_RONG]) {
    int hang, cot, soHangXoa = 0;
    bool day;

    for (hang = CHIEU_CAO - 1; hang >= 0; hang--) {
        day = true;
        for (cot = 0; cot < CHIEU_RONG; cot++) {
            if (ban[hang][cot] == 0) {
                day = false;
                break;
            }
        }
        if (day) {
            soHangXoa++;
            // Dich cac hang phia tren xuong 1 hang
            int h2, c2;
            for (h2 = hang; h2 > 0; h2--) {
                for (c2 = 0; c2 < CHIEU_RONG; c2++) {
                    ban[h2][c2] = ban[h2 - 1][c2];
                }
            }
            for (c2 = 0; c2 < CHIEU_RONG; c2++) {
                ban[0][c2] = 0;
            }
            hang++;  // Kiem tra lai hang vua dich xuong
        }
    }
    return soHangXoa;
}

void tangDiem(int *conTroDiem, int soHangXoa) {
    // Dung con tro int* de thay doi bien diemSo ben ngoai
    if (soHangXoa == 1) {
        *conTroDiem = *conTroDiem + 100;
    } else if (soHangXoa == 2) {
        *conTroDiem = *conTroDiem + 300;
    } else if (soHangXoa == 3) {
        *conTroDiem = *conTroDiem + 500;
    } else if (soHangXoa >= 4) {
        *conTroDiem = *conTroDiem + 800;
    }
}

int taoSoNgauNhien(int minVal, int maxVal) {
    int khoang = maxVal - minVal + 1;
    return minVal + (rand() % khoang);
}

bool taoKhoiMoi(int (*ban)[CHIEU_RONG]) {
    loaiKhoiHienTai = taoSoNgauNhien(0, SO_LOAI_KHOI - 1);
    gocXoayHienTai = 0;
    viTriX = CHIEU_RONG / 2 - 2;  // Can giua theo luoi 4x4
    viTriY = 0;

    if (kiemTraVaCham(ban, loaiKhoiHienTai, gocXoayHienTai, viTriX, viTriY)) {
        return false;  // Khong dat duoc khoi moi -> game over
    }
    return true;
}

bool dichChuyen(int (*ban)[CHIEU_RONG], int dx, int dy) {
    int xMoi = viTriX + dx;
    int yMoi = viTriY + dy;
    if (!kiemTraVaCham(ban, loaiKhoiHienTai, gocXoayHienTai, xMoi, yMoi)) {
        viTriX = xMoi;
        viTriY = yMoi;
        return true;
    }
    return false;
}

bool xoayKhoiHienTai(int (*ban)[CHIEU_RONG]) {
    int gocMoi = (gocXoayHienTai + 1) % SO_HUONG_XOAY;
    if (!kiemTraVaCham(ban, loaiKhoiHienTai, gocMoi, viTriX, viTriY)) {
        gocXoayHienTai = gocMoi;
        return true;
    }
    return false;
}

bool roiXuongMotBuoc(int (*ban)[CHIEU_RONG]) {
    if (dichChuyen(ban, 0, 1)) {
        return true;  // Con roi duoc
    }
    // Khong roi duoc nua -> khoa khoi vao ban
    ganKhoiVaoBan(ban, loaiKhoiHienTai, gocXoayHienTai, viTriX, viTriY);
    int soHang = xoaHangDay(ban);
    if (soHang > 0) {
        tangDiem(&diemSo, soHang);  // Truyen dia chi diemSo bang con tro
        capDo = 1 + diemSo / 500;   // Moi 500 diem tang 1 cap
    }
    if (!taoKhoiMoi(ban)) {
        troChoiKetThuc = true;
    }
    return false;
}

void xuLyPhim(int (*ban)[CHIEU_RONG]) {
#ifdef _WIN32
    if (_kbhit()) {           // Co phim nhan chua doc khong
        char phim = _getch(); // Doc 1 ky tu phim
        if (phim == -32 || phim == 0 || phim == 224) {
            phim = _getch();  // Phim mui ten: ky tu thu 2
            if (phim == 75) {
                dichChuyen(ban, -1, 0);  // Trai
            } else if (phim == 77) {
                dichChuyen(ban, 1, 0);   // Phai
            } else if (phim == 80) {
                roiXuongMotBuoc(ban);    // Xuong
            } else if (phim == 72) {
                xoayKhoiHienTai(ban);    // Len = xoay
            }
        } else if (phim == 'a' || phim == 'A') {
            dichChuyen(ban, -1, 0);
        } else if (phim == 'd' || phim == 'D') {
            dichChuyen(ban, 1, 0);
        } else if (phim == 's' || phim == 'S') {
            roiXuongMotBuoc(ban);
        } else if (phim == 'w' || phim == 'W') {
            xoayKhoiHienTai(ban);
        } else if (phim == 'q' || phim == 'Q') {
            troChoiKetThuc = true;
        }
    }
#else
    (void)ban;  // Tranh canh bao bien khong dung tren he khac Windows
    cout << "(Phien ban nay uu tien Windows voi conio.h)\n";
#endif
}

void tamDungGiay(int giay) {
#ifdef _WIN32
    Sleep(giay);  // Don vi milli giay
#else
    (void)giay;
#endif
}

int tinhThoiGianCho() {
    int tocDo = 500 - (capDo - 1) * 40;  // Cap cao -> cho ngan hon
    if (tocDo < 120) {
        tocDo = 120;
    }
    return tocDo;
}

/* ========== HAM MAIN - DIEM VAO CHUONG TRINH ========== */

int main() {
    srand((unsigned int)time(0));  // Khoi tao hat giong cho ham rand()

    khoiTaoBanCo(banCo);           // Truyen mang banCo (tu dong thanh con tro)
    veKhungHuongDan();

    if (!taoKhoiMoi(banCo)) {
        cout << "Game over ngay khi bat dau!\n";
        return 0;
    }

    cout << "Nhan phim bat ky de bat dau...\n";
#ifdef _WIN32
    _getch();
#endif

    while (!troChoiKetThuc) {
        xoaManHinh();
        veKhungHuongDan();
        veBanCo(banCo, diemSo, capDo);

        xuLyPhim(banCo);              // Doc va xu ly phim nguoi choi
        roiXuongMotBuoc(banCo);       // Tu dong roi theo thoi gian vong lap

        tamDungGiay(tinhThoiGianCho());
    }

    xoaManHinh();
    cout << "TRO CHOI KET THUC!\n";
    cout << "Diem cua ban: " << diemSo << "\n";
    cout << "Cam on ban da choi Tetris.\n";

    return 0;
}
