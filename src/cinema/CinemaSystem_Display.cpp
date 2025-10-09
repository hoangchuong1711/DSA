#include "CinemaSystem.h"
#include <algorithm>
using namespace std;

    // === Các hàm hiển thị giao diện ===
    void CinemaSystem::displayMainMenu() {
        clearScreen();
        cout << "\033[1;36m"; 
        gotoXY(30, 5); cout << "===== MENU RAP CHIEU PHIM =====\n";
        cout << "\033[0m"; 
        
        gotoXY(30, 7); cout << "\033[31m" << "1. " << "\033[0m" <<"Chon phim\n";
        gotoXY(30, 8); cout << "\033[31m" << "2. " << "\033[0m" <<"Tim kiem nguoi dat\n";
        gotoXY(30, 9); cout << "\033[31m" << "3. " << "\033[0m" <<"Thoat\n";

        cout << "\033[36m"; 
        gotoXY(30, 11); cout << "Lua chon cua ban: ";
        cout << "\033[0m"; 
    }

    void CinemaSystem::displayMovieList() {
        clearScreen();
        cout << "\033[1;36m";
        gotoXY(10, 3); cout << "===== DANH SACH PHIM =====\n\n";
        cout << "\033[0m\n";
        sortMoviesByTitle();
        for (int i = 0; i < 5; ++i) {
            cout << " "  << "\033[31m" << i + 1 << ". " << "\033[0m"<< movieList[i].title  << "\n";
        }
        cout << "\n\033[36m0. Quay lai\033[0m\n";
    }

    void CinemaSystem::displayMovieListSortedByRating() {
        clearScreen();
        cout << "\033[1;36m";
        gotoXY(10, 3); cout << "===== DANH SACH PHIM (Rating giam dan) =====\n\n";
        cout << "\033[0m\n";
        Movie temp[5];
        for (int i = 0; i < 5; ++i) temp[i] = movieList[i];
        quickSortMoviesByRating(temp, 0, 4);
        for (int i = 0; i < 5; ++i) {
            cout << " "  << "\033[31m" << i + 1 << ". " << "\033[0m" << temp[i].title << " (" << temp[i].rating << ")\n";
        }
        cout << "\n\033[36m0. Quay lai\033[0m\n";
        // Copy back selection order to main list so index mapping is consistent when user picks
        for (int i = 0; i < 5; ++i) movieList[i] = temp[i];
    }

    void CinemaSystem::displaySeatMap(const string& movieTitle, Showtime& showtime ) {
        clearScreen();
        cout << "\033[1;36m";
        gotoXY(20, 2); cout << "SO DO GHE - Phim: " << movieTitle << "\n";
        gotoXY(20, 3); cout << "Suat chieu: " << formatTime(showtime.time) << "\n";
        gotoXY(30, 5); cout << "--- MAN HINH ---\n\n";
        cout << "\033[0m\n";
        for (int i = 0; i < SEAT_ROWS; ++i) {
            cout << " " << (char)('A' + i) << " ";
            for (int j = 0; j < SEAT_COLS; ++j) {
                Seat& seat = showtime.seats[i][j];
                
                // Đặt màu theo trạng thái ghế
                switch (seat.state) {
                    case AVAILABLE:
                        setTextColor(COLOR_GREEN);
                        cout << "[" << j + 1 << "] ";
                        break;
                    case RESERVED:
                            setTextColor(COLOR_GRAY);
                            cout << "[" << j + 1 << "] ";
                        break;
                    case BOOKED:
                        setTextColor(COLOR_ORANGE);
                        cout << "[X] ";
                        break;
                }
                resetTextColor();
            }
            cout << "\n\n";
        }
        
        // Hiển thị chú thích màu sắc
        setTextColor(COLOR_GREEN);
        cout << "[So]: Con trong ";
        resetTextColor();
        setTextColor(COLOR_GRAY);
        cout << "[So]: Da dat tam ";
        resetTextColor();
        setTextColor(COLOR_ORANGE);
        cout << "[X]: Da thanh toan ";
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