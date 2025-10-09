#include "utils.h"
using namespace std;
// ====== Các hằng số ======
const int COLOR_GREEN = 10;   // Ghế trống
const int COLOR_GRAY = 8;     // Ghế đã đặt tạm
const int COLOR_ORANGE = 6;   // Ghế đã thanh toán
const int COLOR_RED = 12;     // Ghế đã đặt bởi người khác

// ====== Định nghĩa hàm ======
void gotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearScreen() {
    system("cls");
}

void clearCurrentLine() {
    cout << "\r\33[2K";
    cout.flush();
}

void setTextColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetTextColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return toupper(c); });
    return str;
}

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return tolower(c); });
    return str;
}

string formatTime(time_t t) {
    char buffer[30];
    tm localTime;
    localtime_s(&localTime, &t);
    strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", &localTime);
    return string(buffer);
}

// ====== Validation helpers ======
string trim(const string& s) {
    size_t start = 0; 
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    size_t end = s.size(); 
    while (end > start && isspace((unsigned char)s[end-1])) end--;
    return s.substr(start, end - start);
}

bool isValidName(const string& input) {
    string s = trim(input);
    if (s.size() < 2) return false;
    for (unsigned char ch : s) {
        if (ch == ' ') continue;
        if (!isalpha(ch)) return false;
    }
    return true;
}

bool isValidCCCD(const string& input) {
    if (input.size() != 12) return false;
    for (unsigned char ch : input)
        if (!isdigit(ch)) return false;
    return true;
}

string promptValidatedName() {
    while (true) {
        string name; getline(cin, name);
        if (isValidName(name)) return trim(name);
        cout << "Ten khong hop le. Ten chi duoc chua chu cai va khoang trang, tu 2 ky tu tro len. Vui long nhap lai: ";
    }
}

string promptValidatedCCCD() {
    while (true) {
        string cccd; getline(cin, cccd);
        if (isValidCCCD(cccd)) return cccd;
        cout << "CCCD khong hop le. CCCD phai gom 12 chu so. Vui long nhap lai: ";
    }
}
