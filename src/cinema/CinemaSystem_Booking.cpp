#include "CinemaSystem.h"
#include <algorithm>

    // === Các hàm xử lý logic ===
    void CinemaSystem::processMovieSelection() {
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
    
    bool CinemaSystem::processShowtimeSelection(Movie* movie, Customer* existingCustomer) {
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
    
    bool CinemaSystem::handleBooking(Movie* movie, Showtime* showtime, Customer* existingCustomer) {
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
            // Dùng mảng đã parse để kiểm tra trùng lặp
            auto isDuplicate = [&](const std::string& checkCode, int currentCount) -> bool {
                for(int k = 0; k < currentCount; ++k) {
                    if (seatCodes[k] == checkCode) return true;
                }
                return false;
            };
            std::string token;
            while (ss >> token && seatCount < MAX_SEATS_PER_BOOKING) {
                if (isDuplicate(token, seatCount)) { // Kiểm tra trùng lặp trong mảng đã parse
                    std::cout << "Ma ghe nhap bi trung ('" << token << "'). Vui long nhap lai toan bo danh sach ghe.\n"; 
                    std::cin.ignore();
                    seatCount = 0; 
                    break; 
                }
                seatCodes[seatCount++] = token;
            }
            if (seatCount == 0) continue;

            int validSeatRows[MAX_SEATS_PER_BOOKING], validSeatCols[MAX_SEATS_PER_BOOKING];
            int validSeatCount = 0; 
            bool allSeatsValid = true;
            // Không cần validatedSeen nữa vì đã kiểm tra trùng lặp trong vòng lặp parse

            for (int i = 0; i < seatCount; ++i) {
                const std::string& code = seatCodes[i];
                
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

    void CinemaSystem::printReceipt(const Customer& customer, const Movie* movie, const Showtime* showtime, const std::string seatCodes[], int seatCount) {
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

    void CinemaSystem::processCustomerSearch() {
        while (true) {
            clearScreen();
            
            // Check if customer table is completely empty
            bool hasAnyCustomers = false;
            for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
                if (customerTable.table[i].listSize > 0) { 
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
            // Duyệt qua tất cả các bucket (O(TABLE_SIZE))
            for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
                // Duyệt qua Linked List trong bucket hiện tại (O(collision_chain_length))
                Node<HashItem>* currentItemNode = customerTable.table[i].head; 
                
                while (currentItemNode != nullptr) {
                    // currentItemNode->data là một HashItem { std::string key, Customer* customerData }
                    if (toLower(currentItemNode->data.customerData->name) == nameQueryNorm) { 
                        if (foundCount < MAX_NAME_MATCHES) {
                            foundCustomers[foundCount++] = currentItemNode->data.customerData;
                        }
                    }
                    currentItemNode = currentItemNode->next;
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
    
    void CinemaSystem::displayCustomerInfo(Customer* customer) {
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
    
    void CinemaSystem::processMovieSelectionForExistingCustomer(Customer* customer) {
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