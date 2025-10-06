#include "utils.h"

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
    std::cout << "\r\33[2K";
    std::cout.flush();
}

void setTextColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetTextColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return std::toupper(c); });
    return str;
}

std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return str;
}

std::string formatTime(time_t t) {
    char buffer[30];
    tm localTime;
    localtime_s(&localTime, &t);
    strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", &localTime);
    return std::string(buffer);
}

// ====== Validation helpers ======
std::string trim(const std::string& s) {
    size_t start = 0; 
    while (start < s.size() && std::isspace((unsigned char)s[start])) start++;
    size_t end = s.size(); 
    while (end > start && std::isspace((unsigned char)s[end-1])) end--;
    return s.substr(start, end - start);
}

bool isValidName(const std::string& input) {
    std::string s = trim(input);
    if (s.size() < 2) return false;
    for (unsigned char ch : s) {
        if (ch == ' ') continue;
        if (!std::isalpha(ch)) return false;
    }
    return true;
}

bool isValidCCCD(const std::string& input) {
    if (input.size() != 12) return false;
    for (unsigned char ch : input)
        if (!std::isdigit(ch)) return false;
    return true;
}

std::string promptValidatedName() {
    while (true) {
        std::string name; std::getline(std::cin, name);
        if (isValidName(name)) return trim(name);
        std::cout << "Ten khong hop le. Ten chi duoc chua chu cai va khoang trang, tu 2 ky tu tro len. Vui long nhap lai: ";
    }
}

std::string promptValidatedCCCD() {
    while (true) {
        std::string cccd; std::getline(std::cin, cccd);
        if (isValidCCCD(cccd)) return cccd;
        std::cout << "CCCD khong hop le. CCCD phai gom 12 chu so. Vui long nhap lai: ";
    }
}
