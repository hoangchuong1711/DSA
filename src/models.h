#ifndef MODELS_H
#define MODELS_H

#include <string>
#include "linkedList.h"

// Các hằng số cho dễ quản lý
const int SEAT_ROWS = 5;
const int SEAT_COLS = 10;
const int MAX_SHOWTIMES_PER_MOVIE = 8; // Mỗi phim có tối đa 8 suất chiếu

// Enum định nghĩa trạng thái ghế
enum SeatState {
    AVAILABLE = 0,    // Ghế trống (màu xanh lá)
    RESERVED = 1,     // Ghế đã đặt tạm (màu xám)
    BOOKED = 2        // Ghế đã thanh toán (màu cam)
};

// Struct định nghĩa một ghế ngồi
struct Seat {
    SeatState state = AVAILABLE;
    std::string bookedByCCCD; 
    std::string reservedByCCCD; // CCCD của người đặt tạm
};

// Struct định nghĩa một suất chiếu
struct Showtime {
    time_t time; // Thời gian chiếu
    Seat seats[SEAT_ROWS][SEAT_COLS];
};

// Struct định nghĩa một bộ phim
struct Movie {
    std::string title;
    double rating = 0.0;              // thang diem 0..10
    Showtime showtimes[MAX_SHOWTIMES_PER_MOVIE];
    int showtimeCount = 0;            // Số lượng suất chiếu thực tế của phim

    int duration = 0;                 // thời lượng (phút)
    std::string genre;                // thể loại
    int ageLimit = 0;                 // giới hạn tuổi
    std::string synopsis;             // nội dung chính
    std::string director;             // đạo diễn
    Movie(std::string t = "", double r = 0.0, int dur = 0, std::string g = "",
        int age = 0, std::string syn = "", std::string dir = "")
        : title(t), rating(r), duration(dur), genre(g),
        ageLimit(age), synopsis(syn), director(dir) {}
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