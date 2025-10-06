#include "CinemaSystem.h"
#include <algorithm>

    // === Các hàm hiển thị giao diện ===
    void CinemaSystem::displayMainMenu() {
        clearScreen();
        gotoXY(30, 5); std::cout << "===== MENU RAP CHIEU PHIM =====\n";
        gotoXY(30, 7); std::cout << "1. Chon phim\n";
        gotoXY(30, 8); std::cout << "2. Tim kiem nguoi dat\n";
        gotoXY(30, 9); std::cout << "0. Thoat\n";
        gotoXY(30, 11); std::cout << "Lua chon cua ban: ";
    }

    void CinemaSystem::displayMovieList() {
        clearScreen();
        gotoXY(10, 3); std::cout << "===== DANH SACH PHIM =====\n\n";
        sortMoviesByTitle();
        for (int i = 0; i < 5; ++i) {
            std::cout << " " << i + 1 << ". " << movieList[i].title << "\n";
        }
        std::cout << " 0. Quay lai\n";
    }

    void CinemaSystem::displayMovieListSortedByRating() {
        clearScreen();
        gotoXY(10, 3); std::cout << "===== DANH SACH PHIM (Rating giam dan) =====\n\n";
        Movie temp[5];
        for (int i = 0; i < 5; ++i) temp[i] = movieList[i];
        quickSortMoviesByRating(temp, 0, 4);
        for (int i = 0; i < 5; ++i) {
            std::cout << " " << i + 1 << ". " << temp[i].title << " (" << temp[i].rating << ")\n";
        }
        std::cout << " 0. Quay lai\n";
        // Copy back selection order to main list so index mapping is consistent when user picks
        for (int i = 0; i < 5; ++i) movieList[i] = temp[i];
    }

    void CinemaSystem::displaySeatMap(const std::string& movieTitle, Showtime& showtime ) {
        clearScreen();
        gotoXY(20, 2); std::cout << "SO DO GHE - Phim: " << movieTitle << "\n";
        gotoXY(20, 3); std::cout << "Suat chieu: " << formatTime(showtime.time) << "\n";
        gotoXY(30, 5); std::cout << "--- MAN HINH ---\n\n";
        
        for (int i = 0; i < SEAT_ROWS; ++i) {
            std::cout << " " << (char)('A' + i) << " ";
            for (int j = 0; j < SEAT_COLS; ++j) {
                Seat& seat = showtime.seats[i][j];
                
                // Đặt màu theo trạng thái ghế
                switch (seat.state) {
                    case AVAILABLE:
                        setTextColor(COLOR_GREEN);
                        std::cout << "[" << j + 1 << "] ";
                        break;
                    case RESERVED:
                            setTextColor(COLOR_GRAY);
                            std::cout << "[" << j + 1 << "] ";
                        break;
                    case BOOKED:
                        setTextColor(COLOR_ORANGE);
                        std::cout << "[X] ";
                        break;
                }
                resetTextColor();
            }
            std::cout << "\n\n";
        }
        
        // Hiển thị chú thích màu sắc
        setTextColor(COLOR_GREEN);
        std::cout << "[So]: Con trong ";
        resetTextColor();
        setTextColor(COLOR_GRAY);
        std::cout << "[So]: Da dat tam ";
        resetTextColor();
        setTextColor(COLOR_ORANGE);
        std::cout << "[X]: Da thanh toan ";
        resetTextColor();
    }

    // Hàm xóa tất cả đặt tạm của một khách hàng
    void CinemaSystem::clearReservations(Showtime& showtime) {
        for (int i = 0; i < SEAT_ROWS; ++i) {
            for (int j = 0; j < SEAT_COLS; ++j) {
                if (showtime.seats[i][j].state == RESERVED) {
                    showtime.seats[i][j].state = AVAILABLE;
                    showtime.seats[i][j].reservedByCCCD.clear();
                }
            }
        }
    }