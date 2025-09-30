// models.h
#pragma once // Directives này giúp tránh việc include file nhiều lần

#include <string>

// ===== Hằng số toàn cục =====
const int MAX_ROWS = 5;
const int MAX_COLS = 10;
const int MAX_SHOWS = 5;
const int MAX_MOVIES = 8;
const int MAX_TOKENS = 50;

// ===== Structs =====
struct User {
    std::string name;
    std::string cccd;
};

struct Seat {
    bool booked;
    User user;
    Seat() : booked(false), user() {}
};

struct Show {
    std::string time; // "HH:MM"
    Seat seats[MAX_ROWS][MAX_COLS];
    int rows, cols;
    Show() : time("00:00"), rows(MAX_ROWS), cols(MAX_COLS) {}
    Show(std::string t) : time(t), rows(MAX_ROWS), cols(MAX_COLS) {}
};

struct Movie {
    std::string title;
    float rating; // 1..5
    Show shows[MAX_SHOWS];
    int showCount;
    Movie() : title(""), rating(0.0f), showCount(0) {}
};