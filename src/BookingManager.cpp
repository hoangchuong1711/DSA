#include "BookingManager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>

using namespace std;

int BookingManager::chooseShowForMovie(Movie &m) {
    if (m.showCount == 0) return -1;
    cout << "\nSuat chieu cua phim: " << m.title << "\n";
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

void BookingManager::displaySeats(const Show &sh) {
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

bool BookingManager::isFull(const Show &sh) {
    for (int i=0;i<sh.rows;i++)
        for (int j=0;j<sh.cols;j++)
            if (!sh.seats[i][j].booked) return false;
    return true;
}

void BookingManager::bookMultipleSeats(Show &sh) {
    if (isFull(sh)) { cout << "Het ghe trong suat nay!\n"; return; }
    cout << "Nhap so luong ghe muon dat: ";
    int n; cin >> n;
    int freeSeats = countFreeSeats(sh);
    if (n <= 0 || n > freeSeats) { cout << "So luong khong hop le! Con trong: " << freeSeats << "\n"; return; }
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
  
    User u;
    cout << "Nhap ten nguoi dat: ";
    getline(cin, u.name);
    while (!isValidName(u.name)) {
        cout << "Ten khong hop le. Nhap lai: ";
        getline(cin, u.name);
    }
    cout << "Nhap CCCD: ";
    getline(cin, u.cccd);
    while (!isValidCCCD(u.cccd)) {
        cout << "CCCD khong hop le. Nhap lai: ";
        getline(cin, u.cccd);
    }

    // Provisional seats
    int seatRows[MAX_TOKENS];
    int seatCols[MAX_TOKENS];
    for (int i=0;i<desiredCount;i++) { parseSeatCode(desired[i], seatRows[i], seatCols[i], sh); }

    // Invoice
    const int pricePerSeat = 75000;
    int total = pricePerSeat * desiredCount;
    cout << "\n===== HOA DON =====\n";
    cout << "Ten: " << u.name << "\n";
    cout << "CCCD: " << u.cccd << "\n";
    cout << "Suat: " << sh.time << "\n";
    cout << "Ghe: ";
    for (int i=0;i<desiredCount;i++) {
        char rc = 'A' + seatRows[i];
        cout << rc << seatCols[i]+1;
        if (i+1<desiredCount) cout << ", ";
    }
    cout << "\nTong tien: " << total << " VND\n";
    cout << "Xac nhan thanh toan? (1=Co, 0=Khong): ";
    int confirm; cin >> confirm;
    if (confirm != 1) {
        cout << "Da huy dat ve, quay lai chon ghe.\n";
        return;
    }

    // Commit
    for (int i=0;i<desiredCount;i++) {
        int r = seatRows[i];
        int c = seatCols[i];
        sh.seats[r][c].booked = true;
        sh.seats[r][c].user = u;
    }
    cout << "Da xac nhan thanh toan. Chuc ban xem phim vui ve!\n";
}

void BookingManager::cancelSeat(Show &sh) {
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

