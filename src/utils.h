#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <windows.h>
#include <regex>

// Khai báo hằng số màu 
extern const int COLOR_GREEN;
extern const int COLOR_GRAY;
extern const int COLOR_ORANGE;
extern const int COLOR_RED;

// Khai báo hàm 
void gotoXY(int x, int y);
void clearScreen();
void clearCurrentLine();
void setTextColor(int color);
void resetTextColor();

std::string toUpper(std::string str);
std::string toLower(std::string str);
std::string formatTime(time_t t);

std::string trim(const std::string& s);
bool isValidName(const std::string& input);
bool isValidCCCD(const std::string& input);
std::string promptValidatedName();
std::string promptValidatedCCCD();

#endif // UTILS_H
