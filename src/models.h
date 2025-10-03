#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <ctime>
#include "linkedList.h"

// Các hằng số cho dễ quản lý
const int SEAT_ROWS = 5;
const int SEAT_COLS = 10;
const int MAX_SHOWTIMES_PER_MOVIE = 4; // Mỗi phim có tối đa 4 suất chiếu

// Struct định nghĩa một ghế ngồi
struct Seat {
    bool isBooked = false;
    std::string bookedByCCCD; 
};

// Struct định nghĩa một suất chiếu
struct Showtime {
    time_t time; // Thời gian chiếu
    Seat seats[SEAT_ROWS][SEAT_COLS];
};

// Struct định nghĩa một bộ phim
struct Movie {
    std::string title;
    Showtime showtimes[MAX_SHOWTIMES_PER_MOVIE];
    int showtimeCount = 0; // Số lượng suất chiếu thực tế của phim
};

// Struct định nghĩa thông tin một lần đặt vé
// Sửa lại để trỏ đến cả phim và suất chiếu cụ thể
struct Booking {
    Movie* movie;
    Showtime* showtime; 
    LinkedList<std::string> bookedSeats; 
};

// Struct định nghĩa thông tin khách hàng
struct Customer {
    std::string name;
    std::string cccd;
    LinkedList<Booking> bookings;
};

#endif // MODELS_H