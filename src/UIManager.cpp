#include "UIManager.h"
#include "MovieManager.h"
#include "utils.h"
#include "IndexManager.h"
#include <iostream>

using namespace std;

void UIManager::run() {
    // build initial index from current movies
    getIndex().build(movieManager.getMoviesArray(), movieManager.getMovieCount());
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
        } else if (choice == 3 && bookingManager.hasUndoHistory()) {
            // Undo last booking action
            if (bookingManager.undoLastAction(movieManager.getMoviesArray(), movieManager.getMovieCount())) {
                cout << "Da hoan tac thanh cong!\n";
                // Rebuild index after undo
                getIndex().build(movieManager.getMoviesArray(), movieManager.getMovieCount());
            }
        } else {
            std::cout << "Lua chon khong hop le. Thu lai.\n";
        }
    }
}

void UIManager::displayMainMenu() {
    cout << "\n===== MENU RAP CHIEU =====\n";
    cout << "1. Danh sach phim (sap xep mac dinh theo gio)\n";
    cout << "2. Tim kiem nguoi dat (nhap TEN, neu nhieu ket qua se yeu cau CCCD)\n";
    if (bookingManager.hasUndoHistory()) {
        cout << "3. Hoan tac thao tac cuoi (" << (bookingManager.hasUndoHistory() ? "Co" : "Khong") << " lich su)\n";
    }
    cout << "0. Thoat\n";
    cout << "Chon: ";
}

void UIManager::searchUserAndAction() {
    Movie* movies = movieManager.getMoviesArray();
    int movieCount = movieManager.getMovieCount();
    cin.ignore();
    cout << "Nhap TEN de tim: ";
    string q; getline(cin, q);
    if (!isValidName(q)) { cout << "Ten khong hop le (chi chu cai va khoang trang).\n"; return; }

    Position results[128]; int rc = 0;
    // Name index uses exact key match. To support substring, we first try exact; if none, fallback to scan with lowercase compare (bounded).
    getIndex().queryByName(q, results, rc, 128);
    if (rc == 0) {
        // fallback: scan to support substring, but still bounded over current movies
        for (int mi=0; mi<movieCount; mi++) {
            for (int si=0; si<movies[mi].showCount; si++) {
                Show &sh = movies[mi].shows[si];
                for (int r=0;r<sh.rows;r++)
                    for (int c=0;c<sh.cols;c++)
                        if (sh.seats[r][c].booked) {
                            string nm = sh.seats[r][c].user.name;
                            string nlower = nm; for (size_t a=0;a<nlower.size();a++) nlower[a] = tolower(nlower[a]);
                            string qlower = q; for (size_t a=0;a<qlower.size();a++) qlower[a] = tolower(qlower[a]);
                            if (nlower.find(qlower) != string::npos) {
                                if (rc < 128) { results[rc++] = Position{mi,si,r,c}; }
                            }
                        }
            }
        }
    }

    if (rc == 0) { cout << "Khong tim thay ket qua.\n"; return; }
    
    // Group results by unique CCCD to avoid duplicate users
    string uniqueCCCDs[128]; int uniqueCount = 0;
    for (int i=0;i<rc;i++) {
        Position m = results[i];
        Show &sh = movies[m.movieIndex].shows[m.showIndex];
        string cccdCheck = sh.seats[m.rowIndex][m.colIndex].user.cccd;
        bool alreadyExists = false;
        for (int j=0;j<uniqueCount;j++) {
            if (uniqueCCCDs[j] == cccdCheck) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists && uniqueCount < 128) {
            uniqueCCCDs[uniqueCount++] = cccdCheck;
        }
    }
    
    if (uniqueCount == 1) {
        // Only one unique person found
        string cccdTarget = uniqueCCCDs[0];
        string nameTarget = "";
        bool any = false;
        cout << "-----\n";
        for (int mi=0; mi<movieCount; mi++) {
            for (int si=0; si<movies[mi].showCount; si++) {
                Show &sh2 = movies[mi].shows[si];
                for (int r=0;r<sh2.rows;r++)
                    for (int c=0;c<sh2.cols;c++)
                        if (sh2.seats[r][c].booked && sh2.seats[r][c].user.cccd == cccdTarget) {
                            if (!any) {
                                nameTarget = sh2.seats[r][c].user.name;
                                cout << "Ten: " << nameTarget << "\n";
                                cout << "CCCD: " << cccdTarget << "\n";
                                any = true;
                            }
                            char rchar = 'A'+r;
                            cout << "Phim: " << movies[mi].title << " | Suat: " << sh2.time << " | Ghe: " << rchar << c+1 << "\n";
                        }
            }
        }
        if (!any) { cout << "Khong tim thay ve nao.\n"; return; }
        // store context
        contextName = nameTarget;
        contextCCCD = cccdTarget;
        postSearchActions();
        return;
    }

    string cccd;
    while (true) {
        cout << "Tim thay " << uniqueCount << " nguoi khac nhau. Nhap CCCD de xac dinh (0=Quay lai): ";
        getline(cin, cccd);
        if (cccd == "0") return; // back to menu without result
        if (isValidCCCD(cccd)) break;
        cout << "CCCD khong hop le (chi so). Thu lai.\n";
    }
    bool foundAny = false; string foundName = "";
    cout << "-----\n";
    for (int mi=0; mi<movieCount; mi++) {
        for (int si=0; si<movies[mi].showCount; si++) {
            Show &sh = movies[mi].shows[si];
            for (int r=0;r<sh.rows;r++)
                for (int c=0;c<sh.cols;c++)
                    if (sh.seats[r][c].booked && sh.seats[r][c].user.cccd == cccd) {
                        if (!foundAny) {
                            foundName = sh.seats[r][c].user.name;
                            cout << "Ten: " << foundName << "\n";
                            cout << "CCCD: " << cccd << "\n";
                            foundAny = true;
                        }
                        char rchar = 'A'+r;
                        cout << "Phim: " << movies[mi].title << " | Suat: " << sh.time << " | Ghe: " << rchar << c+1 << "\n";
                    }
        }
    }
    if (!foundAny) {
        cout << "Khong co ket qua trung CCCD.\n";
    } else {
        contextName = foundName;
        contextCCCD = cccd;
        postSearchActions();
    }
}

void UIManager::postSearchActions() {
    Movie* movies = movieManager.getMoviesArray();
    int movieCount = movieManager.getMovieCount();
    cout << "\nSau khi xem thong tin, ban muon:\n";
    cout << "1. Dat them ghe (cho nguoi da ton tai - nhap thong tin se su dung CCCD/ten da tim)\n";
    cout << "2. Huy ghe\n";
    cout << "0. Quay lai\n";
    cout << "Chon: ";
    int ch; cin >> ch;
    if (ch == 1) {
        cin.ignore();
        string q;
        if (!contextCCCD.empty()) {
            q = contextCCCD;
            cout << "Su dung CCCD tu ket qua tim kiem: " << q << "\n";
        } else {
            cout << "Nhap CCCD cua nguoi ban muon dat them (0=Quay lai): ";
            getline(cin, q);
            if (q == "0") return;
        }
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
        cout << "Nhap so ghe muon dat them (0=Quay lai): "; int nn; cin >> nn; if (nn==0) return;
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
        // Invoice-like confirm before commit
        int seatRows[MAX_TOKENS];
        int seatCols[MAX_TOKENS];
        for (int i=0;i<desiredCount;i++) parseSeatCode(desired[i], seatRows[i], seatCols[i], sh);
        const int pricePerSeat = 75000; int total = pricePerSeat * desiredCount;
        cout << "\n===== HOA DON =====\n";
        cout << "Ten: " << foundName << "\n";
        cout << "CCCD: " << q << "\n";
        cout << "Phim: " << mptr->title << "\n";
        cout << "Suat: " << sh.time << "\n";
        cout << "Ghe: ";
        for (int i=0;i<desiredCount;i++) { char rr='A'+seatRows[i]; cout << rr << seatCols[i]+1 << (i+1<desiredCount?", ":"\n"); }
        cout << "Tong tien: " << total << " VND\n";
        cout << "Xac nhan thanh toan? (1=Co, 0=Khong): "; int confirm; cin >> confirm; if (confirm != 1) { cout << "Da huy dat.\n"; return; }

        for (int i=0;i<desiredCount;i++) {
            int r = seatRows[i], c = seatCols[i];
            sh.seats[r][c].booked = true;
            sh.seats[r][c].user.name = foundName;
            sh.seats[r][c].user.cccd = q;
        }
        cout << "Da xac nhan thanh toan. Chuc ban xem phim vui ve!\n";
        // rebuild index after booking changes
        getIndex().build(movieManager.getMoviesArray(), movieManager.getMovieCount());
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
        cout << "Nhap ma ghe can huy (0=Quay lai): ";
        string id; cin >> id; if (id == "0") return;
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
        if (canceled) {
            cout << "Huy ghe thanh cong.\n";
            // rebuild index after cancellation
            getIndex().build(movieManager.getMoviesArray(), movieManager.getMovieCount());
        } else {
            cout << "Khong the huy. Kiem tra thong tin.\n";
        }
    } else {
        return;
    }
}

void UIManager::flowListAndBook() {
RESTART_LIST:
    movieManager.listMoviesDefault();
    cout << "Nhap so thu tu phim, hoac '?' de tim theo ten, '/' de sap xep, '0' de quay lai: ";
    std::string cmd; cin >> cmd;
    if (cmd == "0") { return; }
    if (cmd == "/") {
        movieManager.listMoviesSortedMenu();
        int mode; cin >> mode;
        if (mode >=1 && mode <=3) movieManager.listMoviesWithMode(mode-1); else cout << "Lua chon khong hop le.\n";
        cout << "\nChon phim de xem va dat (nhap so thu tu phim, '0' de quay lai): ";
        cmd.clear(); cin >> cmd; if (cmd == "0") return;
    } else if (cmd == "?") {
        cout << "Nhap ten phim can tim: ";
        std::string q; cin.ignore(); getline(cin, q);
        Movie* movies = movieManager.getMoviesArray();
        int movieCount = movieManager.getMovieCount();
        while (true) {
            // Simple search and list matches with remap
            cout << "\nKet qua tim kiem:\n";
            int matchIdx[256]; int matchCount = 0;
            for (int i=0;i<movieCount && matchCount<256;i++) {
                std::string t = movies[i].title; std::string tl = t, ql = q; for (size_t a=0;a<tl.size();a++) tl[a]=tolower(tl[a]); for (size_t a=0;a<ql.size();a++) ql[a]=tolower(ql[a]);
                if (tl.find(ql) != std::string::npos) {
                    matchIdx[matchCount] = i;
                    cout << matchCount+1 << ". " << movies[i].title << " | Earliest: " << (movies[i].showCount>0?movies[i].shows[0].time:"N/A") << "\n";
                    matchCount++;
                }
            }
            if (matchCount == 0) {
                cout << "Khong tim thay ket qua. Ban muon (1=Tim lai, 0=Quay lai): ";
                std::string opt; cin >> opt;
                if (opt == "1") {
                    cout << "Nhap ten phim can tim: ";
                    cin.ignore(); getline(cin, q);
                    continue;
                } else {
                    goto RESTART_LIST;
                }
            }
            cout << "\nChon phim de xem va dat (nhap so thu tu phim, '0' de quay lai): ";
            std::string pick; cin >> pick;
            if (pick == "0") { goto RESTART_LIST; }
            int pickNum = stoi(pick);
            if (pickNum < 1 || pickNum > matchCount) { cout << "Lua chon khong hop le.\n"; continue; }
            // map to global index for later stage
            cmd = std::to_string(matchIdx[pickNum-1] + 1);
            break;
        }
    }
    cout << "\nChon phim de xem va dat (nhap so thu tu phim): ";
    int idx = 0; if (!cmd.empty()) idx = stoi(cmd); else cin >> idx;
    Movie* mptr = movieManager.chooseMovieByIndex(idx);
    if (!mptr) { cout << "Phim khong hop le.\n"; return; }
    // Use current local time to filter shows
    int nowMin = getCurrentMinutesLocal();
    // List only shows in future
    cout << "\nSuat chieu (chi hien suat chua chieu):\n";
    int remap[MAX_SHOWS]; int remapCount = 0;
    for (int i=0;i<mptr->showCount;i++) {
        if (timeToMinutes(mptr->shows[i].time) >= nowMin) {
            remap[remapCount++] = i;
            cout << remapCount << ". " << mptr->shows[i].time << "\n";
        }
    }
    if (remapCount == 0) { cout << "Khong con suat chieu phu hop.\n"; return; }
    cout << "Chon suat (so, '0' de quay lai): ";
    int pick; cin >> pick; if (pick==0) return; if (pick<1 || pick>remapCount) { cout << "Khong hop le.\n"; return; }
    int si = remap[pick-1];
    if (si < 0) { cout << "Suat khong hop le.\n"; return; }
    Show &sh = mptr->shows[si];
    bookingManager.displaySeats(sh);
    cout << "\nBan muon dat ghe cho suat nay? (1 = Co, 0 = Khong): ";
    int bok; cin >> bok;
    if (bok == 1) {
        int movieIndex = idx - 1; // Convert from 1-based to 0-based index
        bookingManager.bookMultipleSeats(sh, movieIndex, si);
        // rebuild index after booking changes
        getIndex().build(movieManager.getMoviesArray(), movieManager.getMovieCount());
    } else {
        cout << "Khong dat. Quay lai menu.\n";
    }
}
