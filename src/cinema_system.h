#ifndef CINEMA_SYSTEM_H
#define CINEMA_SYSTEM_H

#include "models.h"
#include "hashTable.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <algorithm>

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
                    break;
            }
        }
    }

private:
    Movie movieList[5];
    HashTable customerTable;

    time_t createTodayShowtime(int hour, int minute) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        localTime.tm_hour = hour;
        localTime.tm_min = minute;
        localTime.tm_sec = 0;
        return mktime(&localTime);
    }

    void initializeMovies() {
        time_t showtimes[] = {
            createTodayShowtime(23, 59), createTodayShowtime(14, 30),
            createTodayShowtime(23, 30), createTodayShowtime(23, 15)
        };
        int numShowtimes = 4;
        const char* titles[] = {
            "DAO KIEM VUNG DAT QUY", "DORAEMON: NOBITA VA BAN GIAO HUONG",
            "LAT MAT 7: MOT DIEU UOC", "HANH TINH KHI: VUONG QUOC MOI", "TAROT"
        };
        double ratings[] = {7.6, 8.3, 6.8, 8.0, 5.9};
        for (int i = 0; i < 5; ++i) {
            movieList[i].title = titles[i];
            movieList[i].rating = ratings[i];
            for (int j = 0; j < numShowtimes; ++j) {
                movieList[i].showtimes[j].time = showtimes[j];
            }
            movieList[i].showtimeCount = numShowtimes;
        }
    }
    
    // === THUẬT TOÁN SẮP XẾP 1: INSERTION SORT (cho danh sách phim) ===
    void sortMoviesByTitle() {
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

    // === THUẬT TOÁN SẮP XẾP 3: QUICK SORT (sắp xếp phim theo rating giảm dần) ===
    int partitionByRating(Movie arr[], int left, int right) {
        double pivot = arr[(left + right) / 2].rating; // pivot rating
        int i = left, j = right;
        while (i <= j) {
            while (arr[i].rating > pivot) i++;      // rating cao đứng trước
            while (arr[j].rating < pivot) j--;      // rating thấp đứng sau
            if (i <= j) { std::swap(arr[i], arr[j]); i++; j--; }
        }
        return i;
    }
    void quickSortMoviesByRating(Movie arr[], int left, int right) {
        if (left >= right) return;
        int idx = partitionByRating(arr, left, right);
        if (left < idx - 1) quickSortMoviesByRating(arr, left, idx - 1);
        if (idx < right) quickSortMoviesByRating(arr, idx, right);
    }
    // Thêm hàm so sánh cho các con trỏ Booking
    bool compareBookingPtrs(const Booking* a, const Booking* b) {
        // Sắp xếp theo thời gian suất chiếu tăng dần (gần nhất trước)
        return a->showtime->time < b->showtime->time; 
    }
    // === THUẬT TOÁN SẮP XẾP 2: MERGE SORT (cho lịch sử đặt vé) ===
    // Hàm trộn hai mảng con đã sắp xếp
    void mergeBookingPtrs(Booking* arr[], int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Sử dụng mảng tạm chứa con trỏ
        Booking** L = new Booking*[n1];
        Booking** R = new Booking*[n2];

        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            if (compareBookingPtrs(L[i], R[j])) { // So sánh bằng hàm mới
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];

        delete[] L;
        delete[] R;
    }

    // Hàm chính của Merge Sort
    void mergeSortBookingPtrs(Booking* arr[], int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortBookingPtrs(arr, left, mid);
            mergeSortBookingPtrs(arr, mid + 1, right);
            mergeBookingPtrs(arr, left, mid, right);
        }
    }


    // === Các hàm hiển thị giao diện ===
    void displayMainMenu() {
        clearScreen();
        gotoXY(30, 5); std::cout << "===== MENU RAP CHIEU PHIM =====\n";
        gotoXY(30, 7); std::cout << "1. Chon phim\n";
        gotoXY(30, 8); std::cout << "2. Tim kiem nguoi dat\n";
        gotoXY(30, 9); std::cout << "0. Thoat\n";
        gotoXY(30, 11); std::cout << "Lua chon cua ban: ";
    }

    void displayMovieList() {
        clearScreen();
        gotoXY(10, 3); std::cout << "===== DANH SACH PHIM =====\n\n";
        sortMoviesByTitle();
        for (int i = 0; i < 5; ++i) {
            std::cout << " " << i + 1 << ". " << movieList[i].title << "\n";
        }
        std::cout << " 0. Quay lai\n";
    }

    void displayMovieListSortedByRating() {
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

    void displaySeatMap(const std::string& movieTitle, Showtime& showtime ) {
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
    void clearReservations(Showtime& showtime) {
        for (int i = 0; i < SEAT_ROWS; ++i) {
            for (int j = 0; j < SEAT_COLS; ++j) {
                if (showtime.seats[i][j].state == RESERVED) {
                    showtime.seats[i][j].state = AVAILABLE;
                    showtime.seats[i][j].reservedByCCCD.clear();
                }
            }
        }
    }

    // === Các hàm xử lý logic ===
    void processMovieSelection() {
        while (true) {
            clearScreen();
            std::cout << "===== CHON PHIM =====\n";
            std::cout << "1. Sap xep theo ten (A->Z)\n";
            std::cout << "2. Sap xep theo rating (cao->thap)\n";
            std::cout << "0. Quay lai\n";
            std::cout << "Lua chon sap xep: ";
            std::string sortChoice; std::getline(std::cin, sortChoice);
            if (sortChoice == "0") return;
            if (sortChoice == "2") displayMovieListSortedByRating(); 
            if(sortChoice=="1") displayMovieList();
            else {
                std::cout << "Thong tin khong hop le, vui long nhap lai.\n";
                std::cin.ignore();
                continue;
            }
            while(true){
                clearCurrentLine();
                std::cout << "\nChon phim ban muon xem (0 de quay lai): ";
                std::string line; std::getline(std::cin, line);
                if (line == "0") return; // back to main menu
                bool isNumber = !line.empty() && std::all_of(line.begin(), line.end(), ::isdigit);
                if(!isNumber || std::stoi(line) < 0 || std::stoi(line) > 5) { 
                    std::cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.";
                    std::cin.ignore(); 
                    clearCurrentLine();
                    if(sortChoice=="2") {
                        displayMovieListSortedByRating();
                        continue;
                    }
                    else {
                        displayMovieList();
                        continue;
                    }
                }
                int movieIndex= std::stoi(line);
                if (movieIndex > 0 && movieIndex <= 5) {
                    bool booked = processShowtimeSelection(&movieList[movieIndex - 1], nullptr);
                    if (booked) return; 
                    continue;
                } 
                
            }
        }
    }
    
    bool processShowtimeSelection(Movie* movie, Customer* existingCustomer) {
        time_t now = time(0);
        int validShowtimeIndices[MAX_SHOWTIMES_PER_MOVIE];
        while (true) {
            clearScreen();
            gotoXY(10, 3);
            std::cout << "PHIM: " << movie->title << "\n";
            std::cout << "===== VUI LONG CHON SUAT CHIEU =====\n\n";

            int validShowtimeCount = 0;
            for (int i = 0; i < movie->showtimeCount; ++i) {
                if (movie->showtimes[i].time > now) {
                    std::cout << " " << validShowtimeCount + 1 << ". " << formatTime(movie->showtimes[i].time) << "\n";
                    validShowtimeIndices[validShowtimeCount] = i;
                    validShowtimeCount++;
                }
            }

            if (validShowtimeCount == 0) {
                std::cout << "Phim nay da het cac suat chieu trong hom nay.\n";
                return false;
            }

            std::cout << " 0. Quay lai\n";
            std::cout << "\nChon suat chieu ban muon xem: ";
            std::string line; std::getline(std::cin, line);
            if (line == "0") return false; 
            bool isNumber = !line.empty() && std::all_of(line.begin(), line.end(), ::isdigit);
            if (!isNumber || std::stoi(line)<0 || std::stoi(line)>validShowtimeCount) { 
                std::cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.\n"; std::cin.ignore();continue; 
            }
            int showtimeChoice = std::stoi(line);
            if (showtimeChoice > 0 && showtimeChoice <= validShowtimeCount) {
                int actualShowtimeIndex = validShowtimeIndices[showtimeChoice - 1];
                bool bookedNow = handleBooking(movie, &movie->showtimes[actualShowtimeIndex], existingCustomer);
                if (bookedNow) return true; 
                continue;
            } 
        }
    }
    
    bool handleBooking(Movie* movie, Showtime* showtime, Customer* existingCustomer) {
        const int MAX_SEATS_PER_BOOKING = 20;
        Customer* customer = existingCustomer;
        std::string customerCCCD = "";
        
        while (true) {
            displaySeatMap(movie->title, *showtime);
            std::cout << "\nNhap cac ghe can dat, cach nhau boi dau cach (vi du: A1 B2 C3).\n";
            std::cout << "Nhap 0 de quay lai.\n> ";
            std::string seatInput; getline(std::cin, seatInput);
            if (seatInput == "0") {
                // Xóa tất cả đặt tạm trước khi quay lại
                clearReservations(*showtime);
                return false;
            }
            
            std::string seatCodes[MAX_SEATS_PER_BOOKING];
            int seatCount = 0;
            std::stringstream ss(toUpper(seatInput));
            std::unordered_set<std::string> seen;
            std::string token;
            while (ss >> token && seatCount < MAX_SEATS_PER_BOOKING) {
                if (seen.count(token)) { 
                    std::cout << "Ma ghe nhap bi trung ('" << token << "'). Vui long nhap lai toan bo danh sach ghe.\n"; 
                    std::cin.ignore();
                    seatCount = 0; 
                    break; 
                }
                seen.insert(token);
                seatCodes[seatCount++] = token;
            }
            if (seatCount == 0) continue;

            int validSeatRows[MAX_SEATS_PER_BOOKING], validSeatCols[MAX_SEATS_PER_BOOKING];
            int validSeatCount = 0; 
            bool allSeatsValid = true;
            std::unordered_set<std::string> validatedSeen;
            
            for (int i = 0; i < seatCount; ++i) {
                const std::string& code = seatCodes[i];
                if (validatedSeen.count(code)) { 
                    std::cout << "Ma ghe bi trung ('" << code << "'). Vui long nhap lai danh sach ghe.\n"; 
                    std::cin.ignore();
                    allSeatsValid = false; 
                    break; 
                }
                validatedSeen.insert(code);
                
                if (code.length() < 2 || !isalpha(code[0]) || !isdigit(code[1])) {
                    std::cout << "Ma ghe '" << code << "' khong hop le.\n"; 
                    std::cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                int row = code[0] - 'A', col = std::stoi(code.substr(1)) - 1;
                if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) {
                    std::cout << "Ma ghe '" << code << "' khong ton tai.\n"; std::cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                Seat& seat = showtime->seats[row][col];
                if (seat.state == BOOKED) {
                    std::cout << "Ghe '" << code << "' da co nguoi dat. Vui long nhap lai danh sach ghe.\n"; std::cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                if (seat.state == RESERVED && seat.reservedByCCCD != customerCCCD) {
                    std::cout << "Ghe '" << code << "' da co nguoi khac dat tam. Vui long nhap lai danh sach ghe.\n"; std::cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                validSeatRows[validSeatCount] = row;
                validSeatCols[validSeatCount] = col;
                validSeatCount++;
            }

            if (!allSeatsValid) { 
                std::cout << "Vui long nhap lai danh sach ghe.\n"; std::cin.ignore();
                continue; 
            }

            // Xóa tất cả đặt tạm cũ của khách hàng này
            clearReservations(*showtime);
            
            // Đặt tạm các ghế mới
            for (int i = 0; i < validSeatCount; ++i) {
                Seat& seat = showtime->seats[validSeatRows[i]][validSeatCols[i]];
                seat.state = RESERVED;
                
            }
            
            // Hiển thị lại sơ đồ ghế với đặt tạm
            displaySeatMap(movie->title, *showtime);
            
            // Xác nhận thanh toán
            int totalCost = validSeatCount * 75000;
            std::cout << "\nXac nhan thanh toan " << totalCost << " VND cho " << validSeatCount << " ve? (y/n): ";
            std::string confirm; std::getline(std::cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0]) != 'y' && tolower((unsigned char)confirm[0]) != 'n')) {
                std::cout << "Lua chon khong hop le. Vui long nhap y/n.\n"; 
                std::cin.ignore();
                // Xóa đặt tạm nếu không xác nhận
                clearReservations(*showtime);
                continue;
            }
            
            if (tolower((unsigned char)confirm[0]) == 'n') {
                std::cout << "Da huy thanh toan. Cac ghe da dat tam se duoc giai phong.\n";std::cin.ignore();
                // Xóa đặt tạm
                clearReservations(*showtime);
                continue; // quay lai nhap ghe
            }

            // Xác nhận thanh toán thành công - chuyển từ RESERVED sang BOOKED
            Booking newBooking;
            newBooking.movie = movie; 
            newBooking.showtime = showtime;
            
            // Lấy thông tin khách hàng
            if (!customer) {
                std::string name, cccd;
                std::cout << "Vui long nhap ten: ";
                name = toLower(promptValidatedName());
                std::cout << "Vui long nhap CCCD: ";
                cccd = promptValidatedCCCD();
                auto foundCustomer = customerTable.get(cccd);
                if (foundCustomer.has_value()) {
                    if ((*foundCustomer)->name != name) {
                        std::cout << "Loi: CCCD nay da duoc dang ky voi ten khac!\n"; 
                        std::cin.ignore();
                        return false;
                    }
                    customer = *foundCustomer;
                } else {
                    customer = new Customer{name, cccd};
                    customerTable.add(cccd, customer);
                }
            }
            customerCCCD = customer->cccd;

            for (int i = 0; i < validSeatCount; ++i) {
                Seat& seat = showtime->seats[validSeatRows[i]][validSeatCols[i]];
                seat.state = BOOKED;
                seat.bookedByCCCD = customerCCCD;
                seat.reservedByCCCD.clear(); // Xóa thông tin đặt tạm
                newBooking.bookedSeats.add(seatCodes[i]);
            }
            
            customer->bookings.add(newBooking);
            printReceipt(*customer, movie, showtime, seatCodes, seatCount);
            return true;
        }
    }

    void printReceipt(const Customer& customer, const Movie* movie, const Showtime* showtime, const std::string seatCodes[], int seatCount) {
        clearScreen();
        gotoXY(25, 3); std::cout << "====== HOA DON DAT VE ======\n\n";
        std::cout << "  Khach hang: " << customer.name << "\n";
        std::cout << "  CCCD:       " << customer.cccd << "\n";
        std::cout << "  Phim:       " << movie->title << "\n";
        std::cout << "  Suat chieu: " << formatTime(showtime->time) << "\n";
        std::cout << "  Ghe da dat: ";
        for (int i = 0; i < seatCount; ++i) std::cout << seatCodes[i] << " ";
        std::cout << "\n";
        std::cout << "  So luong:   " << seatCount << " ve\n";
        std::cout << "  Tong tien:  " << seatCount * 75000 << " VND\n\n";
        std::cout << "  CAM ON QUY KHACH!\n\n";
        std::cout << "Nhan Enter de quay lai menu chinh...";
        std::cin.ignore();
    }

    void processCustomerSearch() {
        while (true) {
            clearScreen();
            
            // Check if customer table is completely empty
            bool hasAnyCustomers = false;
            for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
                if (customerTable.table[i].count > 0) {
                    hasAnyCustomers = true;
                    break;
                }
            }
            
            if (!hasAnyCustomers) {
                std::cout << "===== TIM KIEM KHACH HANG =====\n\n";
                std::cout << "Hien tai chua co khach hang nao dat ve.\n";
                std::cout << "Vui long dat ve truoc khi tim kiem thong tin khach hang.\n\n";
            std::cout << "Nhan Enter de quay lai menu chinh...";
            std::cin.ignore();
                return;
            }
            
            std::cout << "Nhap ten khach hang (0 de quay lai): ";
            std::string nameQuery; getline(std::cin, nameQuery);
            if (nameQuery == "0") return;
            if (!isValidName(nameQuery)) { std::cout << "Ten khong hop le, vui long nhap lai.\n";std::cin.ignore(); continue; }
            std::string nameQueryNorm = toLower(trim(nameQuery));

            const int MAX_NAME_MATCHES = 50;
            Customer* foundCustomers[MAX_NAME_MATCHES];
            int foundCount = 0;
            for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
                for (int j = 0; j < customerTable.table[i].count; ++j) {
                    if (toLower(customerTable.table[i].values[j].customerData->name) == nameQueryNorm) {
                        if (foundCount < MAX_NAME_MATCHES)
                            foundCustomers[foundCount++] = customerTable.table[i].values[j].customerData;
                    }
                }
            }

            if (foundCount == 0) {
                std::cout << "Khong tim thay khach hang nao voi ten '" << nameQuery << "'.\n";std::cin.ignore();
                continue;
            } else if (foundCount == 1) {
                displayCustomerInfo(foundCustomers[0]);
                return;
            } else {
                std::cout << "Tim thay nhieu khach hang co ten trung nhau. Vui long nhap CCCD de xac dinh:\n";std::cin.ignore();
                for (int i = 0; i < foundCount; ++i)
                    std::cout << " - Ten: " << foundCustomers[i]->name << std::endl;
                std::cout << "Nhap CCCD (12 chu so, 0 de quay lai): ";
                std::string cccd; 
                std::getline(std::cin, cccd);
                if (cccd == "0") continue; // go back to name input
                if (!isValidCCCD(cccd)) { std::cout << "CCCD khong hop le.\n";std::cin.ignore(); continue; }
                auto foundByCCCD = customerTable.get(cccd);
                if (foundByCCCD.has_value()) { displayCustomerInfo(*foundByCCCD); return; }
                else { std::cout << "Khong tim thay theo CCCD da nhap.\n";std::cin.ignore(); continue; }
            }
        }
    }
    
    void displayCustomerInfo(Customer* customer) {
        clearScreen();
        std::cout << "===== THONG TIN KHACH HANG =====\n\n";
        std::cout << "Ten: " << customer->name << "\n";
        std::cout << "CCCD: " << customer->cccd << "\n\n";
        std::cout << "--- Cac ve da dat (da sap xep theo suat chieu moi nhat) ---\n";

            if(customer->bookings.isEmpty()) {
        std::cout << "Khach hang chua dat ve nao.\n";
    } else {
        // Chuyển LinkedList sang mảng CON TRỎ để sắp xếp
        int bookingCount = customer->bookings.size();
        Booking** bookingsPtrArray = new Booking*[bookingCount]; // Mảng con trỏ
        Node<Booking>* current = customer->bookings.head;
        for (int i = 0; i < bookingCount; ++i) {
            bookingsPtrArray[i] = &current->data; // Lấy địa chỉ của Booking gốc
            current = current->next;
        }

        // GỌI HÀM SẮP XẾP CON TRỎ
        mergeSortBookingPtrs(bookingsPtrArray, 0, bookingCount - 1);

        // In ra từ mảng con trỏ đã sắp xếp
        for (int i = 0; i < bookingCount; ++i) {
            Booking* booking = bookingsPtrArray[i]; // Lấy con trỏ booking
            std::cout << " > Phim: " << booking->movie->title << "\n";
            std::cout << "   Suat chieu: " << formatTime(booking->showtime->time) << "\n";
            std::cout << "   Ghe: ";
            Node<std::string>* seatNode = booking->bookedSeats.head;
            while(seatNode) {
                std::cout << seatNode->data << " ";
                seatNode = seatNode->next;
            }
            std::cout << "\n\n";
        }
        
        delete[] bookingsPtrArray; // Giải phóng bộ nhớ mảng con trỏ
    }


        std::cout << "--------------------------------\n";
        std::cout << "1. Dat them ghe\n";
        std::cout << "2. Huy ghe\n";
        std::cout << "0. Quay lai menu chinh\n";
        std::cout << "Lua chon: ";
        std::string line;
        std::getline(std::cin, line);
        if (line == "1") processMovieSelectionForExistingCustomer(customer);
        else if (line == "2") cancelSeat(customer);
        else if (line == "0") return;
        else {
            std::cout << "Lua chon khong hop le, vui long nhan Enter nhap lai.";std::cin.ignore();
            displayCustomerInfo(customer); 
        }
    }
    
    void processMovieSelectionForExistingCustomer(Customer* customer) {
        while (true) {
            clearScreen();
            std::cout << "===== CHON PHIM (DAT THEM) =====\n";
            std::cout << "1. Sap xep theo ten (A->Z)\n";
            std::cout << "2. Sap xep theo rating (cao->thap)\n";
            std::cout << "0. Quay lai\n";
            std::cout << "Lua chon sap xep: ";
            
            std::string sortChoice;
            std::getline(std::cin, sortChoice);

            if (sortChoice == "0") {
                displayCustomerInfo(customer);
                return;
            }

            if (sortChoice == "2") displayMovieListSortedByRating();
            else if (sortChoice == "1") displayMovieList();
            else {
                std::cout << "Thong tin khong hop le, vui long nhap lai.\n";
                std::cin.ignore();
                continue;
            }

            // --- Vòng lặp chọn phim ---
            while (true) {
                clearCurrentLine();
                std::cout << "\nChon phim ban muon dat them ve (0 de quay lai): ";
                std::string line;
                std::getline(std::cin, line);

                if (line == "0") {
                    displayCustomerInfo(customer);
                    return; // quay lại menu khách hàng
                }

                bool isNumber = !line.empty() && std::all_of(line.begin(), line.end(), ::isdigit);
                if (!isNumber || std::stoi(line) < 1 || std::stoi(line) > 5) {
                    std::cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.";
                    std::cin.ignore();
                    clearCurrentLine();

                    if (sortChoice == "2")
                        displayMovieListSortedByRating();
                    else
                        displayMovieList();

                    continue;
                }

                int movieIndex = std::stoi(line);
                bool booked = processShowtimeSelection(&movieList[movieIndex - 1], customer);
                if (booked) {
                    displayCustomerInfo(customer);
                    return; // về menu khách hàng sau khi đặt xong
                }

                // nếu người dùng chọn 0 ở bước chọn suất chiếu -> quay lại chọn phim
                continue;
            }
        }
    }

    // === HUY GHE ===
    int findSeatIndexInBooking(const Booking& booking, const std::string& seatCodeUpper) {
        Node<std::string>* node = booking.bookedSeats.head;
        int idx = 0;
        while (node) {
            if (toUpper(node->data) == seatCodeUpper) return idx;
            node = node->next; idx++;
        }
        return -1;
    }

    bool parseSeatCode(const std::string& codeUpper, int& row, int& col) {
        if (codeUpper.length() < 2 || !isalpha(codeUpper[0])) return false;
        for (size_t i = 1; i < codeUpper.length(); ++i) if (!isdigit(codeUpper[i])) return false;
        row = codeUpper[0] - 'A';
        col = std::stoi(codeUpper.substr(1)) - 1;
        if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) return false;
        return true;
    }

void cancelSeat(Customer* customer) {
    while (true) {
        clearScreen();
        std::cout << "===== HUY GHE =====\n\n";
        if (customer->bookings.isEmpty()) {
            std::cout << "Ban chua co ve nao de huy.\n";
            std::cout << "Nhan Enter de quay lai..."; std::cin.ignore();
            displayCustomerInfo(customer);
            return;
        }

        // Hien thi tat ca cac ve da dat voi index
        std::cout << "Cac ve da dat:\n";
        std::cout << "----------------------------------------\n";
        
        // Chuyển LinkedList sang mảng CON TRỎ để sắp xếp và hiển thị
        int bookingCount = customer->bookings.size();
        // SỬ DỤNG MẢNG CON TRỎ (Booking**)
        Booking** bookingsPtrArray = new Booking*[bookingCount]; 
        Node<Booking>* current = customer->bookings.head;
        for (int i = 0; i < bookingCount; ++i) {
            bookingsPtrArray[i] = &current->data; // Lưu CON TRỎ tới Booking gốc
            current = current->next;
        }

        // Sắp xếp theo thời gian suất chiếu (gần nhất trước)
        mergeSortBookingPtrs(bookingsPtrArray, 0, bookingCount - 1);

        // Hiển thị danh sách vé với index
        for (int i = 0; i < bookingCount; ++i) {
            Booking* booking = bookingsPtrArray[i];
            std::cout << " " << (i + 1) << ". Phim: " << booking->movie->title << "\n";
            std::cout << "    Suat chieu: " << formatTime(booking->showtime->time) << "\n";
            std::cout << "    Ghe: ";
            Node<std::string>* seatNode = booking->bookedSeats.head;
            while(seatNode) {
                std::cout << seatNode->data << " ";
                seatNode = seatNode->next;
            }
            std::cout << "(" << booking->bookedSeats.size() << " ghe)\n\n";
        }

        std::cout << " 0. Quay lai\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Chon ve muon huy (nhap so thu tu): ";
        
        std::string choice; std::getline(std::cin, choice);
        if (choice == "0") {
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }

        int ticketIndex;
        try { 
            ticketIndex = std::stoi(choice); 
        } catch (...) { 
            std::cout << "Vui long nhap so hop le.\n"; std::cin.ignore();
            delete[] bookingsPtrArray;
            continue; 
        }

        if (ticketIndex < 1 || ticketIndex > bookingCount) {
            std::cout << "Lua chon khong hop le. Vui long nhap lai.\n";std::cin.ignore();
            delete[] bookingsPtrArray;
            continue;
        }

        // Lấy con trỏ VÉ được chọn
        Booking* selectedBookingPtr = bookingsPtrArray[ticketIndex - 1]; // Lấy con trỏ
        int seatCount = selectedBookingPtr->bookedSeats.size();

        // Xử lý hủy toàn bộ vé (1 ghế) hoặc chọn ghế (nhiều ghế)
        // ... (Logic tương tự như code cũ, nhưng sử dụng 'selectedBookingPtr->' thay cho 'selectedBooking.')
        std::string movieTitle = selectedBookingPtr->movie->title;
        std::string showtimeStr = formatTime(selectedBookingPtr->showtime->time);
        std::string seatCodeUpper = toUpper(selectedBookingPtr->bookedSeats.head->data);
        // Nếu vé chỉ có 1 ghế, hủy luôn
        if (seatCount == 1) {
            std::cout << "\nVe nay chi co 1 ghe. Ban co chac muon huy ve nay? (y/n): ";
            std::string confirm; std::getline(std::cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0]) != 'y' && tolower((unsigned char)confirm[0]) != 'n')) {
                std::cout << "Lua chon khong hop le. Vui long nhap y/n.\n";std::cin.ignore();
                delete[] bookingsPtrArray;
                continue;
            }
            if (tolower((unsigned char)confirm[0]) == 'n') {
                std::cout << "Da huy thao tac.\n";std::cin.ignore();
                delete[] bookingsPtrArray;
                continue;
            }
            int pr = -1, pc = -1;
            if (parseSeatCode(seatCodeUpper, pr, pc)) {
                selectedBookingPtr->showtime->seats[pr][pc].state = AVAILABLE;
                selectedBookingPtr->showtime->seats[pr][pc].bookedByCCCD.clear();
            }

            // Tìm và xóa booking khỏi danh sách (Dùng con trỏ để so sánh)
            Node<Booking>* node = customer->bookings.head;
            int index = 0;
            while (node) {
                // So sánh địa chỉ con trỏ để đảm bảo xóa đúng Booking
                if (&node->data == selectedBookingPtr) {
                    customer->bookings.removeAt(index);
                    break;
                }
                node = node->next;
                index++;
            }
            
            // ... (Phần hiển thị kết quả)
            clearScreen();
            std::cout << "===== HUY VE THANH CONG =====\n\n";
            std::cout << "Phim: " << movieTitle  << "\n";
            std::cout << "Suat chieu: " << showtimeStr  << "\n";
            std::cout << "Ghe da huy: " << seatCodeUpper  << "\n";
            std::cout << "So tien hoan lai: " << 75000 << " VND\n";
            std::cout << "Ve da duoc xoa khoi danh sach.\n\n";
            std::cout << "Nhan Enter de quay lai..."; std::cin.ignore();
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }
        // Nếu vé có nhiều ghế, cho chọn ghế cụ thể
        else {
            clearScreen();
            std::cout << "===== CHON GHE CAN HUY =====\n\n";
            std::cout << "Phim: " << selectedBookingPtr->movie->title << "\n";
            std::cout << "Suat chieu: " << formatTime(selectedBookingPtr->showtime->time) << "\n\n";
            std::cout << "Cac ghe da dat:\n";
            
            // Hiển thị các ghế với index
            Node<std::string>* seatNode = selectedBookingPtr->bookedSeats.head;
            int seatIndex = 0;
            while(seatNode) {
                std::cout << " " << (seatIndex + 1) << ". " << seatNode->data << "\n";
                seatNode = seatNode->next;
                seatIndex++;
            }
            
            // ... (Phần chọn ghế, kiểm tra index)
            std::cout << "\n 0. Quay lai\n";
            std::cout << "----------------------------------------\n";
            std::cout << "Chon ghe muon huy (nhap so thu tu): ";
            
            std::string seatChoice; std::getline(std::cin, seatChoice);
            if (seatChoice == "0") {
                delete[] bookingsPtrArray;
                cancelSeat(customer);
                return;
            }

            // Parse multiple tokens: indices (1-based) or seat codes (e.g., A1)
            std::stringstream ss(seatChoice);
            std::vector<int> indices;
            auto containsIndex = [&](int v){ for(int x: indices) if (x==v) return true; return false; };
            auto toUpperStr = [](std::string s){ for(char& c: s) c = (char)toupper((unsigned char)c); return s; };
            std::string tok;
            bool anyInvalid = false;
            while (ss >> tok) {
                // Try as integer
                bool accepted = false;
                try {
                    size_t pos = 0;
                    int asIndex = std::stoi(tok, &pos);
                    if (pos == tok.size() && asIndex >= 1 && asIndex <= seatCount) {
                        if (!containsIndex(asIndex)) indices.push_back(asIndex);
                        accepted = true;
                    }
                } catch (...) {}
                if (accepted) continue;

                // Try as seat code
                std::string codeUpper = toUpperStr(tok);
                int row=-1,col=-1;
                if (parseSeatCode(codeUpper, row, col)) {
                    int foundIdx = findSeatIndexInBooking(*selectedBookingPtr, codeUpper);
                    if (foundIdx != -1) {
                        int oneBased = foundIdx + 1;
                        if (!containsIndex(oneBased)) indices.push_back(oneBased);
                        continue;
                    }
                }
                std::cout << "\n[LOI]: \"" << tok << "\" khong phai la so thu tu hop le (1-" << seatCount << ") hoac ma ghe da dat.\n"; // THÔNG BÁO LỖI
                anyInvalid = true;
                break;
            }

            if (anyInvalid) {
                std::cout << "Nhan Enter de thu lai..."; std::cin.ignore();
                delete[] bookingsPtrArray; 
                continue; 
            }
            
            if (indices.empty()) { 
                // Trường hợp người dùng nhập các token hợp lệ nhưng đã bị trùng (đã được thêm vào 'indices') 
                // hoặc chỉ nhập khoảng trắng/không nhập gì.
                std::cout << "\n[LOI]: Vui long nhap it nhat mot so thu tu hoac ma ghe de huy.\n";
                std::cout << "Nhan Enter de thu lai..."; std::cin.ignore();
                delete[] bookingsPtrArray;
                continue; 
            }

            // Build list of codes to cancel for confirmation
            std::vector<std::string> toCancelCodes;
            for (int oneBasedIdx : indices) {
                Node<std::string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i) sn = sn->next;
                toCancelCodes.push_back(toUpperStr(sn->data));
            }
            std::cout << "\nCac ghe se huy: "; for (auto& c: toCancelCodes) std::cout << c << " ";
            std::cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n";
            std::cout << "Xac nhan (y/n): ";
            std::string confirm; std::getline(std::cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0])!='y' && tolower((unsigned char)confirm[0])!='n')) { delete[] bookingsPtrArray; continue; }
            if (tolower((unsigned char)confirm[0])=='n') { delete[] bookingsPtrArray; continue; }

            // Sort desc to remove safely
            std::sort(indices.begin(), indices.end(), std::greater<int>());

            // Cancel seats: update showtime then remove from booking
            for (int oneBasedIdx : indices) {
                Node<std::string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i) sn = sn->next;
                std::string codeUpper = toUpperStr(sn->data);
                int pr=-1, pc=-1;
                if (parseSeatCode(codeUpper, pr, pc)) {
                    selectedBookingPtr->showtime->seats[pr][pc].state = AVAILABLE;
                    selectedBookingPtr->showtime->seats[pr][pc].bookedByCCCD.clear();
                }
                selectedBookingPtr->bookedSeats.removeAt(oneBasedIdx - 1);
            }

            // Show result
            clearScreen();
            std::cout << "===== HUY GHE THANH CONG =====\n\n";
            std::cout << "Phim: " << selectedBookingPtr->movie->title << "\n";
            std::cout << "Suat chieu: " << formatTime(selectedBookingPtr->showtime->time) << "\n";
            std::cout << "Cac ghe da huy: "; for (auto& c: toCancelCodes) std::cout << c << " ";
            std::cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n\n";

            int remaining = selectedBookingPtr->bookedSeats.size();
            if (remaining > 0) {
                std::cout << "Ghe con lai: ";
                Node<std::string>* remainingSeatNode = selectedBookingPtr->bookedSeats.head;
                while (remainingSeatNode) { std::cout << remainingSeatNode->data << " "; remainingSeatNode = remainingSeatNode->next; }
                std::cout << "\nSo ghe con lai: " << remaining << "\n";
            } else {
                Node<Booking>* nodeToDelete = customer->bookings.head;
                int indexToDelete = 0;
                while (nodeToDelete) {
                    if (&nodeToDelete->data == selectedBookingPtr) { customer->bookings.removeAt(indexToDelete); std::cout << "Ve da duoc xoa khoi danh sach (khong con ghe nao).\n"; break; }
                    nodeToDelete = nodeToDelete->next; indexToDelete++;
                }
            }

            std::cout << "\nNhan Enter de quay lai..."; std::cin.ignore();
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }
    }
}
};

#endif // CINEMA_SYSTEM_H