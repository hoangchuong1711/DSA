#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm> // << THÊM DÒNG NÀY
#include <cctype>    // << THÊM DÒNG NÀY
#include <ctime>
#include <windows.h> 

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

// Hàm định dạng thời gian từ time_t thành chuỗi "HH:MM DD/MM/YYYY"
std::string formatTime(time_t t) {
    char buffer[30];
    tm localTime;
    // Sử dụng localtime_s an toàn hơn localtime trên Windows
    localtime_s(&localTime, &t); 
    strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", &localTime);
    return std::string(buffer);
}

#endif // UTILS_H