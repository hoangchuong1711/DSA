// CinemaSystem.cpp
#include "CinemaSystem.h"
#include "utils.h" // Sử dụng các hàm tiện ích
#include <iostream>
#include <iomanip>

using namespace std;

CinemaSystem::CinemaSystem() {
    movieCount = 0;
    initMovies();
}

void CinemaSystem::initMovies() {
    // ... (copy a nguyên văn phần thân hàm initMovies từ file gốc)
        // Add 5 sample movies (you can edit / add more later easily)
        // Movie 1
        movies[0].title = "Avengers";
        movies[0].rating = 4.6f;
        movies[0].showCount = 2;
        movies[0].shows[0] = Show("09:00");
        movies[0].shows[1] = Show("14:00");

        // Movie 2
        movies[1].title = "Inception";
        movies[1].rating = 4.8f;
        movies[1].showCount = 2;
        movies[1].shows[0] = Show("10:00");
        movies[1].shows[1] = Show("16:00");

        // Movie 3
        movies[2].title = "Interstellar";
        movies[2].rating = 4.7f;
        movies[2].showCount = 1;
        movies[2].shows[0] = Show("12:30");

        // Movie 4
        movies[3].title = "Joker";
        movies[3].rating = 4.3f;
        movies[3].showCount = 2;
        movies[3].shows[0] = Show("11:00");
        movies[3].shows[1] = Show("19:00");

        // Movie 5
        movies[4].title = "The Matrix";
        movies[4].rating = 4.5f;
        movies[4].showCount = 1;
        movies[4].shows[0] = Show("18:00");

        movieCount = 5;
}

// ===== Copy tất cả các hàm thành viên còn lại của CinemaSystem vào đây =====
// NHỚ: Thêm "CinemaSystem::" vào trước tên mỗi hàm. Ví dụ:
// void displayMainMenu()  =>  void CinemaSystem::displayMainMenu()

void CinemaSystem::displayMainMenu() {
    cout << "\n===== MENU RAP CHIEU =====\n";
    cout << "1. Danh sach phim (sap xep mac dinh theo gio)\n";
    cout << "2. Tim kiem nguoi dat (ten/CCCD)\n";
    cout << "0. Thoat\n";
    cout << "Chon: ";
}

// ... copy và sửa tất cả các hàm khác: listMoviesDefault, listMoviesSortedMenu, ...
// ... bookMultipleSeats, cancelSeat, searchUserAndAction, postSearchActions, flowListAndBook
// (Nội dung các hàm giữ nguyên, chỉ thêm CinemaSystem::)

// Ví dụ một hàm được chuyển đổi:
void CinemaSystem::listMoviesDefault() {
    quickSortMovies(movies, 0, movieCount-1, 0); // sort by earliest show time asc
    cout << "\n===== DANH SACH PHIM (theo gio chieu) =====\n";
    for (int i=0;i<movieCount;i++) {
        cout << i+1 << ". " << movies[i].title
             << "  | Rating: " << fixed << setprecision(1) << movies[i].rating
             << "  | Earliest: " << (movies[i].showCount>0 ? movies[i].shows[0].time : "N/A")
             << "  | Free seats total: " << totalFreeSeats(movies[i])
             << "\n";
    }
}

// (Bạn hãy tự copy và sửa các hàm còn lại tương tự nhé)

//
// Dưới đây là phần còn lại đã được chuyển đổi sẵn
//
void CinemaSystem::listMoviesSortedMenu() {
    cout << "\nSap xep phim theo:\n";
    cout << "1. Gio chieu (tang dan)\n";
    cout << "2. So ghe trong (giam dan)\n";
    cout << "3. Danh gia (rating, giam dan)\n";
    cout << "Chon: ";
}

void CinemaSystem::listMoviesWithMode(int mode) {
    quickSortMovies(movies, 0, movieCount-1, mode);
    cout << "\n===== DANH SACH PHIM =====\n";
    for (int i=0;i<movieCount;i++) {
        cout << i+1 << ". " << movies[i].title
             << "  | Rating: " << fixed << setprecision(1) << movies[i].rating
             << "  | Earliest: " << (movies[i].showCount>0 ? movies[i].shows[0].time : "N/A")
             << "  | Free seats total: " << totalFreeSeats(movies[i])
             << "\n";
    }
}

Movie* CinemaSystem::chooseMovieByIndex(int idx) {
    if (idx < 1 || idx > movieCount) return nullptr;
    return &movies[idx-1];
}

int CinemaSystem::chooseShowForMovie(Movie &m) {
    if (m.showCount == 0) return -1;
    cout << "\nSu at chieu cua phim: " << m.title << "\n";
    for (int i=0;i<m.showCount;i++) {
        cout << i+1 << ". " << m.shows[i].time << "  | Free seats: ";
        int freeCnt = 0;
        for (int r=0;r<m.shows[i].rows;r++)
            for (int c=0;c<m.shows[i].cols;c++)
                if (!m.shows[i].seats[r][c].booked) freeCnt++;
        cout << freeCnt << "\n";
    }
    cout << "Chon suat (so): ";
    int ch; cin >> ch;
    if (ch < 1 || ch > m.showCount) return -1;
    return ch-1;
}

void CinemaSystem::displaySeats(const Show &sh) {
    cout << "\nSo do ghe - Suat " << sh.time << " (O = trong, X = da dat)\n";
    cout << "   ";
    for (int c=0;c<sh.cols;c++) {
        cout << setw(3) << c+1;
    }
    cout << "\n";
    for (int i=0;i<sh.rows;i++) {
        char rowChar = 'A' + i;
        cout << rowChar << "  ";
        for (int j=0;j<sh.cols;j++) {
            cout << setw(3) << (sh.seats[i][j].booked ? "X" : "O");
        }
        cout << "\n";
    }
}

bool CinemaSystem::isFull(const Show &sh) {
    for (int i=0;i<sh.rows;i++)
        for (int j=0;j<sh.cols;j++)
            if (!sh.seats[i][j].booked) return false;
    return true;
}

void CinemaSystem::bookMultipleSeats(Show &sh) {
    if (isFull(sh)) { cout << "Het ghe trong suat nay!\n"; return; }
    cout << "Nhap so luong ghe muon dat: ";
    int n; cin >> n;
    if (n <= 0 || n > (sh.rows * sh.cols)) { cout << "So luong khong hop le!\n"; return; }
    cin.ignore();
    string line;
    cout << "Nhap danh sach ma ghe cach nhau boi dau cach (vd: A1 A2 A3):\n";
    getline(cin, line);
    string tokens[MAX_TOKENS];
    int tcount = splitTokens(line, tokens, MAX_TOKENS);
    string desired[MAX_TOKENS];
    int desiredCount = 0;
    for (int i=0;i<tcount && desiredCount < n;i++) desired[desiredCount++] = tokens[i];
    while (desiredCount < n) {
        cout << "Vẫn thiếu " << (n - desiredCount) << " ma ghe, nhap them: ";
        string addline;
        getline(cin, addline);
        int addc = splitTokens(addline, tokens, MAX_TOKENS);
        for (int i=0;i<addc && desiredCount < n;i++) desired[desiredCount++] = tokens[i];
    }

    bool okAll = false;
    while (!okAll) {
        okAll = true;
        for (int i=0;i<desiredCount;i++) {
            int r,c;
            if (!parseSeatCode(desired[i], r, c, sh)) {
                cout << "Ma ghe '" << desired[i] << "' khong hop le. Nhap lai ma ghe: ";
                string s; cin >> s;
                desired[i] = s;
                okAll = false;
                continue;
            }
            if (sh.seats[r][c].booked) {
                cout << "Ma ghe '" << desired[i] << "' da duoc dat. Nhap lai ma ghe: ";
                string s; cin >> s;
                desired[i] = s;
                okAll = false;
                continue;
            }
        }
    }

    cin.ignore();
    User u;
    cout << "Nhap ten nguoi dat: ";
    getline(cin, u.name);
    cout << "Nhap CCCD: ";
    getline(cin, u.cccd);

    for (int i=0;i<desiredCount;i++) {
        int r,c;
        parseSeatCode(desired[i], r, c, sh);
        sh.seats[r][c].booked = true;
        sh.seats[r][c].user = u;
    }
    cout << "Dat thanh cong " << desiredCount << " ghe.\n";
}

void CinemaSystem::cancelSeat(Show &sh) {
    cout << "Nhap ma ghe can huy: ";
    string id; cin >> id;
    int r,c;
    if (!parseSeatCode(id, r, c, sh)) { cout << "Ma ghe khong hop le!\n"; return; }
    if (!sh.seats[r][c].booked) { cout << "Ghe dang trong, khong the huy!\n"; return; }
    sh.seats[r][c].booked = false;
    sh.seats[r][c].user.name = "";
    sh.seats[r][c].user.cccd = "";
    cout << "Huy ghe " << id << " thanh cong!\n";
}

void CinemaSystem::searchUserAndAction() {
    cin.ignore();
    cout << "Tim theo (1) Ten, (2) CCCD. Chon: ";
    int mode; cin >> mode;
    cin.ignore();
    if (mode == 1) {
        cout << "Nhap ten (hoac chuoi ten) de tim: ";
        string q; getline(cin, q);
        bool found = false;
        for (int mi=0; mi<movieCount; mi++) {
            for (int si=0; si<movies[mi].showCount; si++) {
                Show &sh = movies[mi].shows[si];
                for (int r=0;r<sh.rows;r++)
                    for (int c=0;c<sh.cols;c++)
                        if (sh.seats[r][c].booked) {
                            string nm = sh.seats[r][c].user.name;
                            string nn = nm; string qq = q;
                            for (size_t a=0;a<nn.size();a++) nn[a] = tolower(nn[a]);
                            for (size_t a=0;a<qq.size();a++) qq[a] = tolower(qq[a]);
                            if (nn.find(qq) != string::npos) {
                                found = true;
                                cout << "-----\n";
                                cout << "Ten: " << nm << "\n";
                                cout << "CCCD: " << sh.seats[r][c].user.cccd << "\n";
                                cout << "Phim: " << movies[mi].title << "\n";
                                cout << "Suat: " << sh.time << "\n";
                                char rc = 'A'+r;
                                cout << "Ghe: " << rc << c+1 << "\n";
                            }
                        }
            }
        }
        if (!found) cout << "Khong tim thay ket qua.\n";
        else postSearchActions();
    } else if (mode == 2) {
        cout << "Nhap CCCD: ";
        string q; getline(cin, q);
        bool found = false;
        for (int mi=0; mi<movieCount && !found; mi++) {
            for (int si=0; si<movies[mi].showCount && !found; si++) {
                Show &sh = movies[mi].shows[si];
                for (int r=0;r<sh.rows && !found;r++)
                    for (int c=0;c<sh.cols && !found;c++)
                        if (sh.seats[r][c].booked && sh.seats[r][c].user.cccd == q) {
                            found = true;
                            cout << "-----\n";
                            cout << "Ten: " << sh.seats[r][c].user.name << "\n";
                            cout << "CCCD: " << sh.seats[r][c].user.cccd << "\n";
                            cout << "Phim: " << movies[mi].title << "\n";
                            cout << "Suat: " << sh.time << "\n";
                            char rc = 'A'+r;
                            cout << "Ghe: " << rc << c+1 << "\n";
                        }
            }
        }
        if (!found) cout << "Khong tim thay ket qua.\n";
        else postSearchActions();
    } else {
        cout << "Lua chon khong hop le.\n";
    }
}

void CinemaSystem::postSearchActions() {
    cout << "\nSau khi xem thong tin, ban muon:\n";
    cout << "1. Dat them ghe (cho nguoi da ton tai - nhap thong tin se su dung CCCD/ten da tim)\n";
    cout << "2. Huy ghe\n";
    cout << "0. Quay lai\n";
    cout << "Chon: ";
    int ch; cin >> ch;
    if (ch == 1) {
        cin.ignore();
        cout << "Nhap CCCD cua nguoi ban muon dat them: ";
        string q; getline(cin, q);
        string foundName = "";
        for (int mi=0; mi<movieCount && foundName==""; mi++)
            for (int si=0; si<movies[mi].showCount && foundName==""; si++) {
                Show &sh = movies[mi].shows[si];
                for (int r=0;r<sh.rows && foundName=="";r++)
                    for (int c=0;c<sh.cols && foundName=="";c++)
                        if (sh.seats[r][c].booked && sh.seats[r][c].user.cccd == q) {
                            foundName = sh.seats[r][c].user.name;
                        }
            }
        if (foundName == "") { cout << "Khong tim thay user voi CCCD nay.\n"; return; }
        cout << "Chon phim de dat them (se hien danh sach):\n";
        listMoviesWithMode(0);
        cout << "Nhap so thu tu phim: "; int pm; cin >> pm;
        Movie* mptr = chooseMovieByIndex(pm);
        if (!mptr) { cout << "Phim khong hop le.\n"; return; }
        int si = chooseShowForMovie(*mptr);
        if (si < 0) { cout << "Suat khong hop le.\n"; return; }
        Show &sh = mptr->shows[si];
        cout << "Nhap so ghe muon dat them: "; int nn; cin >> nn;
        cin.ignore();
        if (nn <= 0) { cout << "So luong khong hop le.\n"; return; }
        cout << "Nhap danh sach ma ghe (A1 A2 ...):\n";
        string line; getline(cin, line);
        string tokens[MAX_TOKENS];
        int tcount = splitTokens(line, tokens, MAX_TOKENS);
        string desired[MAX_TOKENS];
        int desiredCount = 0;
        for (int i=0;i<tcount && desiredCount<nn;i++) desired[desiredCount++] = tokens[i];
        while (desiredCount < nn) {
            cout << "Con thieu " << (nn - desiredCount) << " ma ghe, nhap them: ";
            string addline; getline(cin, addline);
            int addc = splitTokens(addline, tokens, MAX_TOKENS);
            for (int i=0;i<addc && desiredCount<nn;i++) desired[desiredCount++] = tokens[i];
        }
        bool okAll = false;
        while (!okAll) {
            okAll = true;
            for (int i=0;i<desiredCount;i++) {
                int r,c;
                if (!parseSeatCode(desired[i], r, c, sh)) {
                    cout << "Ma ghe '" << desired[i] << "' khong hop le. Nhap lai ma ghe: ";
                    string s; cin >> s; desired[i] = s; okAll = false; continue;
                }
                if (sh.seats[r][c].booked) {
                    cout << "Ma ghe '" << desired[i] << "' da duoc dat. Nhap lai ma ghe: ";
                    string s; cin >> s; desired[i] = s; okAll = false; continue;
                }
            }
        }
        for (int i=0;i<desiredCount;i++) {
            int r,c; parseSeatCode(desired[i], r, c, sh);
            sh.seats[r][c].booked = true;
            sh.seats[r][c].user.name = foundName;
            sh.seats[r][c].user.cccd = q;
        }
        cout << "Dat them thanh cong.\n";
    } else if (ch == 2) {
        cin.ignore();
        cout << "Nhap CCCD cua nguoi muon huy: ";
        string q; getline(cin, q);
        bool found = false;
        for (int mi=0; mi<movieCount; mi++) {
            for (int si=0; si<movies[mi].showCount; si++) {
                Show &sh = movies[mi].shows[si];
                for (int r=0;r<sh.rows;r++)
                    for (int c=0;c<sh.cols;c++)
                        if (sh.seats[r][c].booked && sh.seats[r][c].user.cccd == q) {
                            found = true;
                            char rc = 'A'+r;
                            cout << "Tim thay ghe: Phim='" << movies[mi].title << "' Suat=" << sh.time
                                 << " Ghe=" << rc << c+1 << "\n";
                        }
            }
        }
        if (!found) { cout << "Khong tim thay ghe nao cua CCCD nay.\n"; return; }
        cout << "Nhap ma ghe can huy: ";
        string id; cin >> id;
        bool canceled = false;
        for (int mi=0; mi<movieCount && !canceled; mi++) {
            for (int si=0; si<movies[mi].showCount && !canceled; si++) {
                Show &sh = movies[mi].shows[si];
                int r,c;
                if (parseSeatCode(id, r, c, sh)) {
                    if (sh.seats[r][c].booked && sh.seats[r][c].user.cccd == q) {
                        sh.seats[r][c].booked = false;
                        sh.seats[r][c].user.name = "";
                        sh.seats[r][c].user.cccd = "";
                        canceled = true;
                    }
                }
            }
        }
        if (canceled) cout << "Huy ghe thanh cong.\n"; else cout << "Khong the huy. Kiem tra thong tin.\n";
    } else {
        return;
    }
}

void CinemaSystem::flowListAndBook() {
    listMoviesDefault();
    cout << "\nBan muon sap xep khac khong? (1 = Co, 0 = Khong): ";
    int ans; cin >> ans;
    if (ans == 1) {
        listMoviesSortedMenu();
        int mode; cin >> mode;
        if (mode >=1 && mode <=3) {
            listMoviesWithMode(mode-1);
        } else {
            cout << "Lua chon khong hop le, tiep tuc voi mac dinh.\n";
            listMoviesDefault();
        }
    }
    cout << "\nChon phim de xem va dat (nhap so thu tu phim): ";
    int idx; cin >> idx;
    Movie* mptr = chooseMovieByIndex(idx);
    if (!mptr) { cout << "Phim khong hop le.\n"; return; }
    int si = chooseShowForMovie(*mptr);
    if (si < 0) { cout << "Suat khong hop le.\n"; return; }
    Show &sh = mptr->shows[si];
    displaySeats(sh);
    cout << "\nBan muon dat ghe cho suat nay? (1 = Co, 0 = Khong): ";
    int bok; cin >> bok;
    if (bok == 1) {
        bookMultipleSeats(sh);
    } else {
        cout << "Khong dat. Quay lai menu.\n";
    }
}