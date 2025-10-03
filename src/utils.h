#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm> // << THÊM DÒNG NÀY
#include <cctype>    // << THÊM DÒNG NÀY
#include <ctime>
#include <windows.h> 
#include <regex>

// Hàm di chuyển con trỏ console đến vị trí (x, y)
void gotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Hàm xóa màn hình console
void clearScreen() {
    system("cls");
}

// Hàm chuyển đổi chuỗi thành chữ hoa
std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                [](unsigned char c){ return std::toupper(c); });
    return str;
}

// Hàm chuyển đổi chuỗi thành chữ thường
std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
                [](unsigned char c){ return std::tolower(c); });
    return str;
}

// Hàm định dạng thời gian từ time_t thành chuỗi "HH:MM DD/MM/YYYY"
std::string formatTime(time_t t) {
    char buffer[30];
    tm localTime;
    // Sử dụng localtime_s an toàn hơn localtime trên Windows
    localtime_s(&localTime, &t); 
    strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", &localTime);
    return std::string(buffer);
}

// --- Helpers: validation for Name and CCCD ---
static inline std::string trim(const std::string& s) {
    size_t start = 0; while (start < s.size() && std::isspace((unsigned char)s[start])) start++;
    size_t end = s.size(); while (end > start && std::isspace((unsigned char)s[end-1])) end--;
    return s.substr(start, end - start);
}

inline bool isValidName(const std::string& input) {
    std::string s = trim(input);
    if (s.size() < 2) return false;
    for (unsigned char ch : s) {
        if (ch == ' ') continue;
        if (!std::isalpha(ch)) return false;
    }
    return true;
}

inline bool isValidCCCD(const std::string& input) {
    if (input.size() != 12) return false;
    for (unsigned char ch : input) if (!std::isdigit(ch)) return false;
    return true;
}

inline std::string promptValidatedName() {
    while (true) {
        std::string name; std::getline(std::cin, name);
        if (isValidName(name)) return trim(name);
        std::cout << "Ten khong hop le. Ten chi duoc chua chu cai va khoang trang, tu 2 ky tu tro len. Vui long nhap lai: ";
    }
}

inline std::string promptValidatedCCCD() {
    while (true) {
        std::string cccd; std::getline(std::cin, cccd);
        if (isValidCCCD(cccd)) return cccd;
        std::cout << "CCCD khong hop le. CCCD phai gom 12 chu so. Vui long nhap lai: ";
    }
}

#endif // UTILS_H