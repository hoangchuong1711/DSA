#ifndef CINEMA_SYSTEM_H
#define CINEMA_SYSTEM_H

#include "models.h"
#include "hashTable.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <sstream>

class CinemaSystem {
public:
    CinemaSystem() {
        initializeMovies();
    }

    void run() {
        bool running = true;
        while (running) {
            displayMainMenu();
            char choice;
            std::cin >> choice;
            std::cin.ignore(10000, '\n'); 

            switch (choice) {
                case '1':
                    processMovieSelection();
                    break;
                case '2':
                    processCustomerSearch();
                    break;
                case '0':
                    running = false;
                    break;
                default:
                    std::cout << "Lua chon khong hop le. Vui long thu lai.\n";
                    system("pause");
                    break;
            }
        }
    }

private:
    Movie movieList[5];
    HashTable customerTable;

    // Hàm tiện ích để tạo time_t cho một giờ, phút cụ thể trong ngày hôm nay
    time_t createTodayShowtime(int hour, int minute) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now); // Lấy thời gian hiện tại
        localTime.tm_hour = hour;
        localTime.tm_min = minute;
        localTime.tm_sec = 0;
        return mktime(&localTime);
    }

    void initializeMovies() {
        // Khởi tạo các suất chiếu cố định
        time_t showtimes[] = {
            createTodayShowtime(9, 0),   // 09:00
            createTodayShowtime(14, 30), // 14:30
            createTodayShowtime(19, 0),  // 19:00
            createTodayShowtime(22, 15)  // 22:15
        };
        int numShowtimes = 4;

        const char* titles[] = {
            "DAO KIEM VUNG DAT QUY",
            "DORAEMON: NOBITA VA BAN GIAO HUONG",
            "LAT MAT 7: MOT DIEU UOC",
            "HANH TINH KHI: VUONG QUOC MOI",
            "TAROT"
        };

        for (int i = 0; i < 5; ++i) {
            movieList[i].title = titles[i];
            for (int j = 0; j < numShowtimes; ++j) {
                movieList[i].showtimes[j].time = showtimes[j];
            }
            movieList[i].showtimeCount = numShowtimes;
        }
    }
    
    // === Sắp xếp phim theo tên (Alphabetical) ===
    void sortMoviesByTitle() {
        // Sử dụng Insertion Sort - đơn giản và hiệu quả với N=5
        for (int i = 1; i < 5; i++) {
            Movie key = movieList[i];
            int j = i - 1;
            while (j >= 0 && movieList[j].title > key.title) {
                movieList[j + 1] = movieList[j];
                j = j - 1;
            }
            movieList[j + 1] = key;
        }
    }

    // === Các hàm hiển thị giao diện ===
    void displayMainMenu() {
        clearScreen();
        gotoXY(30, 5);
        std::cout << "===== MENU RAP CHIEU PHIM =====\n";
        gotoXY(30, 7);
        std::cout << "1. Chon phim\n";
        gotoXY(30, 8);
        std::cout << "2. Tim kiem nguoi dat\n";
        gotoXY(30, 9);
        std::cout << "0. Thoat\n";
        gotoXY(30, 11);
        std::cout << "Lua chon cua ban: ";
    }

    void displayMovieList() {
        clearScreen();
        gotoXY(10, 3);
        std::cout << "===== DANH SACH PHIM =====\n\n";
        sortMoviesByTitle(); // Sắp xếp phim theo tên
        for (int i = 0; i < 5; ++i) {
            std::cout << " " << i + 1 << ". " << movieList[i].title << "\n";
        }
    }

    void displaySeatMap(const std::string& movieTitle, Showtime& showtime) {
        clearScreen();
        gotoXY(20, 2);
        std::cout << "SO DO GHE - Phim: " << movieTitle << "\n";
        gotoXY(20, 3);
        std::cout << "Suat chieu: " << formatTime(showtime.time) << "\n";
        gotoXY(30, 5);
        std::cout << "--- MAN HINH ---\n\n";
        for (int i = 0; i < SEAT_ROWS; ++i) {
            std::cout << " " << (char)('A' + i) << " ";
            for (int j = 0; j < SEAT_COLS; ++j) {
                if (showtime.seats[i][j].isBooked) {
                    std::cout << "[X] ";
                } else {
                    std::cout << "[" << j + 1 << "] ";
                }
            }
            std::cout << "\n\n";
        }
        std::cout << "[X]: Da dat | [So]: Con trong\n";
    }

    // === Các hàm xử lý logic ===
    void processMovieSelection() {
        displayMovieList();
        std::cout << "\nChon phim ban muon xem: ";
        int movieIndex;
        std::cin >> movieIndex;
        std::cin.ignore(10000, '\n');

        if (movieIndex > 0 && movieIndex <= 5) {
            // Chuyển sang bước chọn suất chiếu
            processShowtimeSelection(&movieList[movieIndex - 1], nullptr);
        } else {
            std::cout << "Lua chon khong hop le.\n";
            system("pause");
        }
    }
    
    void processShowtimeSelection(Movie* movie, Customer* existingCustomer) {
        clearScreen();
        gotoXY(10, 3);
        std::cout << "PHIM: " << movie->title << "\n";
        std::cout << "===== VUI LONG CHON SUAT CHIEU =====\n\n";
        
        time_t now = time(0);
        int validShowtimeCount = 0;
        // Mảng để lưu chỉ số của các suất chiếu hợp lệ
        int validShowtimeIndices[MAX_SHOWTIMES_PER_MOVIE]; 

        for (int i = 0; i < movie->showtimeCount; ++i) {
            if (movie->showtimes[i].time > now) { // Chỉ hiện suất chưa chiếu
                std::cout << " " << validShowtimeCount + 1 << ". " << formatTime(movie->showtimes[i].time) << "\n";
                validShowtimeIndices[validShowtimeCount] = i;
                validShowtimeCount++;
            }
        }

        if (validShowtimeCount == 0) {
            std::cout << "Phim nay da het cac suat chieu trong hom nay.\n";
            system("pause");
            return;
        }

        std::cout << "\nChon suat chieu: ";
        int showtimeChoice;
        std::cin >> showtimeChoice;
        std::cin.ignore(10000, '\n');

        if (showtimeChoice > 0 && showtimeChoice <= validShowtimeCount) {
            int actualShowtimeIndex = validShowtimeIndices[showtimeChoice - 1];
            handleBooking(movie, &movie->showtimes[actualShowtimeIndex], existingCustomer);
        } else {
            std::cout << "Lua chon khong hop le.\n";
            system("pause");
        }
    }
    
    void handleBooking(Movie* movie, Showtime* showtime, Customer* existingCustomer) {
        displaySeatMap(movie->title, *showtime);
        std::cout << "Nhap cac ghe can dat, cach nhau boi dau cach (vi du: A1 B2 C3): ";
        std::string seatInput;
        getline(std::cin, seatInput);
        
        const int MAX_SEATS_PER_BOOKING = 20;
        std::string seatCodes[MAX_SEATS_PER_BOOKING];
        int seatCount = 0;
        std::stringstream ss(toUpper(seatInput));
        std::string token;
        while (ss >> token && seatCount < MAX_SEATS_PER_BOOKING) {
            seatCodes[seatCount++] = token;
        }
        
        if (seatCount == 0) {
            std::cout << "Vui long nhap ma ghe.\n";
            system("pause");
            return;
        }
        
        int validSeatRows[MAX_SEATS_PER_BOOKING];
        int validSeatCols[MAX_SEATS_PER_BOOKING];
        int validSeatCount = 0;
        bool allSeatsValid = true;
        for (int i = 0; i < seatCount; ++i) {
            const std::string& code = seatCodes[i];
            if (code.length() < 2 || !isalpha(code[0]) || !isdigit(code[1])) {
                std::cout << "Ma ghe '" << code << "' khong hop le.\n"; allSeatsValid = false; break;
            }
            int row = code[0] - 'A';
            int col = std::stoi(code.substr(1)) - 1;
            if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) {
                std::cout << "Ma ghe '" << code << "' khong ton tai.\n"; allSeatsValid = false; break;
            }
            if (showtime->seats[row][col].isBooked) {
                std::cout << "Ghe '" << code << "' da co nguoi dat.\n"; allSeatsValid = false; break;
            }
            validSeatRows[validSeatCount] = row;
            validSeatCols[validSeatCount] = col;
            validSeatCount++;
        }

        if (allSeatsValid) {
            Customer* customer = existingCustomer;
            if (!customer) {
                std::string name, cccd;
                std::cout << "Vui long nhap ten: ";
                getline(std::cin, name);
                std::cout << "Vui long nhap CCCD: ";
                getline(std::cin, cccd);
                
                auto foundCustomer = customerTable.get(cccd);
                if (foundCustomer.has_value()) {
                     if ((*foundCustomer)->name != name) {
                         std::cout << "Loi: CCCD nay da duoc dang ky voi ten khac!\n"; system("pause"); return;
                     }
                     customer = *foundCustomer;
                } else {
                    customer = new Customer{name, cccd};
                    customerTable.add(cccd, customer);
                }
            }
            
            Booking newBooking;
            newBooking.movie = movie;
            newBooking.showtime = showtime; // Lưu suất chiếu
            for (int i = 0; i < validSeatCount; ++i) {
                 showtime->seats[validSeatRows[i]][validSeatCols[i]].isBooked = true;
                 showtime->seats[validSeatRows[i]][validSeatCols[i]].bookedByCCCD = customer->cccd;
                 newBooking.bookedSeats.add(seatCodes[i]);
            }
            customer->bookings.add(newBooking);
            printReceipt(*customer, movie, showtime, seatCodes, seatCount);
        } else {
            system("pause");
        }
    }

    void printReceipt(const Customer& customer, const Movie* movie, const Showtime* showtime, const std::string seatCodes[], int seatCount) {
        clearScreen();
        gotoXY(25, 3);
        std::cout << "====== HOA DON DAT VE ======\n\n";
        std::cout << "  Khach hang: " << customer.name << "\n";
        std::cout << "  CCCD:       " << customer.cccd << "\n";
        std::cout << "  Phim:       " << movie->title << "\n";
        std::cout << "  Suat chieu: " << formatTime(showtime->time) << "\n";
        std::cout << "  Ghe da dat: ";
        for (int i = 0; i < seatCount; ++i) {
            std::cout << seatCodes[i] << " ";
        }
        std::cout << "\n";
        std::cout << "  So luong:   " << seatCount << " ve\n";
        std::cout << "  Tong tien:  " << seatCount * 75000 << " VND\n\n";
        std::cout << "  CAM ON QUY KHACH!\n";
        system("pause");
    }

    void processCustomerSearch() {
        clearScreen();
        std::cout << "Nhap ten hoac CCCD cua nguoi dat: ";
        std::string keyword;
        getline(std::cin, keyword);
        auto foundByCCCD = customerTable.get(keyword);
        if (foundByCCCD.has_value()) {
            displayCustomerInfo(*foundByCCCD);
            return;
        }
        
        const int MAX_NAME_MATCHES = 10;
        Customer* foundCustomers[MAX_NAME_MATCHES];
        int foundCount = 0;
        for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
            for (int j = 0; j < customerTable.table[i].count; ++j) {
                if (toUpper(customerTable.table[i].values[j].customerData->name).find(toUpper(keyword)) != std::string::npos) {
                    if (foundCount < MAX_NAME_MATCHES) {
                        foundCustomers[foundCount++] = customerTable.table[i].values[j].customerData;
                    }
                }
            }
        }

        if (foundCount == 0) {
            std::cout << "Khong tim thay khach hang nao.\n";
        } else if (foundCount == 1) {
            displayCustomerInfo(foundCustomers[0]);
        } else {
            std::cout << "Tim thay nhieu khach hang co ten trung nhau. Vui long nhap CCCD de xac dinh:\n";
             for (int i = 0; i < foundCount; ++i) {
                std::cout << " - Ten: " << foundCustomers[i]->name << ", CCCD: " << foundCustomers[i]->cccd << std::endl;
            }
            processCustomerSearch(); 
        }
        system("pause");
    }
    
    void displayCustomerInfo(Customer* customer) {
        clearScreen();
        std::cout << "===== THONG TIN KHACH HANG =====\n\n";
        std::cout << "Ten: " << customer->name << "\n";
        std::cout << "CCCD: " << customer->cccd << "\n\n";
        std::cout << "--- Cac ve da dat ---\n";

        if(customer->bookings.isEmpty()) {
            std::cout << "Khach hang chua dat ve nao.\n";
        } else {
            Node<Booking>* currentBookingNode = customer->bookings.head;
            while(currentBookingNode) {
                std::cout << " > Phim: " << currentBookingNode->data.movie->title << "\n";
                std::cout << "   Suat chieu: " << formatTime(currentBookingNode->data.showtime->time) << "\n";
                std::cout << "   Ghe: ";
                Node<std::string>* currentSeatNode = currentBookingNode->data.bookedSeats.head;
                while(currentSeatNode) {
                    std::cout << currentSeatNode->data << " ";
                    currentSeatNode = currentSeatNode->next;
                }
                std::cout << "\n\n";
                currentBookingNode = currentBookingNode->next;
            }
        }

        std::cout << "--------------------------------\n";
        std::cout << "1. Dat them ghe\n";
        std::cout << "2. Huy ghe\n";
        std::cout << "0. Quay lai menu chinh\n";
        std::cout << "Lua chon: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        switch(choice) {
            case '1':
                processMovieSelectionForExistingCustomer(customer);
                break;
            case '2':
                std::cout << "Chuc nang huy ve hien chua duoc ho tro.\n";
                system("pause");
                break;
            case '0':
            default:
                return;
        }
    }
    
    void processMovieSelectionForExistingCustomer(Customer* customer) {
        displayMovieList();
        std::cout << "\nChon phim ban muon dat them ve: ";
        int movieIndex;
        std::cin >> movieIndex;
        std::cin.ignore(10000, '\n');
        if (movieIndex > 0 && movieIndex <= 5) {
            // Chuyển sang chọn suất chiếu cho khách hàng cũ
            processShowtimeSelection(&movieList[movieIndex - 1], customer);
        } else {
            std::cout << "Lua chon khong hop le.\n";
            system("pause");
        }
    }
};

#endif // CINEMA_SYSTEM_H