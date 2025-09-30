#include "UIManager.h"
#include "MovieManager.h"
#include "utils.h"
#include <iostream>

using namespace std;

void UIManager::run() {
    while (true) {
        displayMainMenu();
        int choice;
        std::cin >> choice;
        if (choice == 0) {
            std::cout << "Thoat chuong trinh.\n";
            break;
        } else if (choice == 1) {
            flowListAndBook();
        } else if (choice == 2) {
            // Lấy mảng phim từ movieManager để truyền cho bookingManager
            searchUserAndAction();
        } else {
            std::cout << "Lua chon khong hop le. Thu lai.\n";
        }
    }
}

void UIManager::displayMainMenu() {
    cout << "\n===== MENU RAP CHIEU =====\n";
    cout << "1. Danh sach phim (sap xep mac dinh theo gio)\n";
    cout << "2. Tim kiem nguoi dat (ten/CCCD)\n";
    cout << "0. Thoat\n";
    cout << "Chon: ";
}

void UIManager::searchUserAndAction() {
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

void UIManager::postSearchActions() {
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
        movieManager.listMoviesWithMode(0);
        cout << "Nhap so thu tu phim: "; int pm; cin >> pm;
        Movie* mptr = movieManager.chooseMovieByIndex(pm);
        if (!mptr) { cout << "Phim khong hop le.\n"; return; }
        int si = bookingManager.chooseShowForMovie(*mptr);
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

void UIManager::flowListAndBook() {
    movieManager.listMoviesDefault();
    cout << "\nBan muon sap xep khac khong? (1 = Co, 0 = Khong): ";
    int ans; cin >> ans;
    if (ans == 1) {
        movieManager.listMoviesSortedMenu();
        int mode; cin >> mode;
        if (mode >=1 && mode <=3) {
            movieManager.listMoviesWithMode(mode-1);
        } else {
            cout << "Lua chon khong hop le, tiep tuc voi mac dinh.\n";
            movieManager.listMoviesDefault();
        }
    }
    cout << "\nChon phim de xem va dat (nhap so thu tu phim): ";
    int idx; cin >> idx;
    Movie* mptr = movieManager.chooseMovieByIndex(idx);
    if (!mptr) { cout << "Phim khong hop le.\n"; return; }
    int si = bookingManager.chooseShowForMovie(*mptr);
    if (si < 0) { cout << "Suat khong hop le.\n"; return; }
    Show &sh = mptr->shows[si];
    bookingManager.displaySeats(sh);
    cout << "\nBan muon dat ghe cho suat nay? (1 = Co, 0 = Khong): ";
    int bok; cin >> bok;
    if (bok == 1) {
        bookingManager.bookMultipleSeats(sh);
    } else {
        cout << "Khong dat. Quay lai menu.\n";
    }
}
