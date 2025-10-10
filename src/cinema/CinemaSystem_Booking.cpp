#include "CinemaSystem.h"
#include <algorithm>
using namespace std;
    void CinemaSystem::processMovieSelection() {
        while (true) {
            clearScreen();
            cout << "\033[1;36mCHON PHIM - CINEMA SYSTEM\033[0m\n\n";
    cout << "-------------------------------------\n";
    cout << "\033[31m1.\033[0m  Sap xep theo \033[35mTen (A -> Z)\033[0m\n";
    cout << "\033[31m2.\033[0m  Sap xep theo \033[35mRating (cao -> thap)\033[0m\n";
    cout << "\033[31m0.\033[0m  Quay lai\n";
    cout << "-------------------------------------\n";
    cout << "\033[36mNhap lua chon cua ban: \033[0m";
            string sortChoice; getline(cin, sortChoice);

            if (sortChoice == "0") return;
            else if (sortChoice == "2") displayMovieListSortedByRating(); 
            else if(sortChoice=="1") displayMovieList();
            else {
                cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.\n";
                cin.ignore();
                continue;
            }
            while(true){
                clearCurrentLine();
                cout << "\033[34m";
                cout << "\nChon phim ban muon xem (0 de quay lai): ";
                cout << "\033[0m";
                string line; getline(cin, line);
                if (line == "0") return; // back to main menu
                bool isNumber = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
                if(!isNumber || stoi(line) < 0 || stoi(line) > 5) { 
                    cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.";
                    cin.ignore(); 
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
                int movieIndex = stoi(line);
                if (movieIndex > 0 && movieIndex <= movieList.size()) {
                    Node<Movie>* node = movieList.getNode(movieIndex - 1);
                    if (node) {
                        bool booked = processShowtimeSelection(&node->data, nullptr);
                        if (booked) return;
                    }
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
            cout << "\033[1;36m";
            cout << "PHIM: " << movie->title << "\n";
            cout << "===== VUI LONG CHON SUAT CHIEU =====\n\n";
            cout << "\033[0m";
            int validShowtimeCount = 0;
            for (int i = 0; i < movie->showtimeCount; ++i) {
                if (movie->showtimes[i].time > now) {
                    cout << " " << "\033[31m" << validShowtimeCount + 1 << ". " << "\033[0m" << formatTime(movie->showtimes[i].time) << "\n";
                    validShowtimeIndices[validShowtimeCount] = i;
                    validShowtimeCount++;
                }
            }

            if (validShowtimeCount == 0) {
                cout << "Phim nay da het cac suat chieu trong hom nay.\n";
                return false;
            }

            cout << " \033[31m0.\033[0m" << "Quay lai\n";
            cout << "\033[34m";
            cout << "\nChon suat chieu ban muon xem: ";
            cout << "\033[0m\n";
            string line; getline(cin, line);
            if (line == "0") return false; 
            bool isNumber = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
            if (!isNumber || stoi(line)<0 || stoi(line)>validShowtimeCount) { 
                cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.\n"; cin.ignore();continue; 
            }
            int showtimeChoice = stoi(line);
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
        string customerCCCD = "";
        
        while (true) {
            displaySeatMap(movie->title, *showtime);
            cout << "\nNhap cac ghe can dat, cach nhau boi dau cach (vi du: A1 B2 C3).\n";
            cout << "Nhap 0 de quay lai.\n> ";
            string seatInput; getline(cin, seatInput);
            if (seatInput == "0") {
                // Xóa tất cả đặt tạm trước khi quay lại
                clearReservations(*showtime);
                return false;
            }
            
            string seatCodes[MAX_SEATS_PER_BOOKING];
            int seatCount = 0;
            stringstream ss(toUpper(seatInput));
            // Dùng mảng đã parse để kiểm tra trùng lặp
            auto isDuplicate = [&](const string& checkCode, int currentCount) -> bool {
                for(int k = 0; k < currentCount; ++k) {
                    if (seatCodes[k] == checkCode) return true;
                }
                return false;
            };
            string token;
            while (ss >> token && seatCount < MAX_SEATS_PER_BOOKING) {
                if (isDuplicate(token, seatCount)) { // Kiểm tra trùng lặp trong mảng đã parse
                    cout << "Ma ghe nhap bi trung ('" << token << "'). Vui long nhap lai toan bo danh sach ghe.\n"; 
                    cin.ignore();
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
                const string& code = seatCodes[i];
                
                if (code.length() < 2 || !isalpha(code[0]) || !isdigit(code[1])) {
                    cout << "Ma ghe '" << code << "' khong hop le.\n"; 
                    cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                int row = code[0] - 'A', col = stoi(code.substr(1)) - 1;
                if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) {
                    cout << "Ma ghe '" << code << "' khong ton tai.\n"; cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                Seat& seat = showtime->seats[row][col];
                if (seat.state == BOOKED) {
                    cout << "Ghe '" << code << "' da co nguoi dat. Vui long nhap lai danh sach ghe.\n"; cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                if (seat.state == RESERVED && seat.reservedByCCCD != customerCCCD) {
                    cout << "Ghe '" << code << "' da co nguoi khac dat tam. Vui long nhap lai danh sach ghe.\n"; cin.ignore();
                    allSeatsValid = false; 
                    break;
                }
                
                validSeatRows[validSeatCount] = row;
                validSeatCols[validSeatCount] = col;
                validSeatCount++;
            }

            if (!allSeatsValid) { 
                cout << "Vui long nhap lai danh sach ghe.\n"; cin.ignore();
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
            cout << "\nXac nhan thanh toan " << totalCost << " VND cho " << validSeatCount << " ve? (\033[32my\033[0m/\033[31mn\033[0m): ";
            string confirm; getline(cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0]) != 'y' && tolower((unsigned char)confirm[0]) != 'n')) {
                cout << "Lua chon khong hop le. Vui long nhap \033[32my\033[0m/\033[31mn\033[0m.\n"; 
                cin.ignore();
                // Xóa đặt tạm nếu không xác nhận
                clearReservations(*showtime);
                continue;
            }
            
            if (tolower((unsigned char)confirm[0]) == 'n') {
                cout << "Da huy thanh toan. Cac ghe da dat tam se duoc giai phong.\n";cin.ignore();
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
                string name, cccd;
                cout << "Vui long nhap ten: ";
                name = toLower(promptValidatedName());
                cout << "Vui long nhap CCCD: ";
                cccd = promptValidatedCCCD();
                auto foundCustomer = customerTable.get(cccd);
                if (foundCustomer.has_value()) {
                    if ((*foundCustomer)->name != name) {
                        cout << "Loi: CCCD nay da duoc dang ky voi ten khac!\n"; 
                        cin.ignore();
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

    void CinemaSystem::printReceipt(const Customer& customer, const Movie* movie, const Showtime* showtime, const string seatCodes[], int seatCount) {
        clearScreen();
        cout << "\033[1;32m";
        cout << "====== HOA DON DAT VE ======\n\n";
        cout << "\033[0m\n";
        cout << "  Khach hang: " << customer.name << "\n";
        cout << "  CCCD:       " << customer.cccd << "\n";
        cout << "  Phim:       "  << " \033[1;33m"<< movie->title << "\n";
        cout << "\033[0m";
        cout << "  Suat chieu: " << " \033[1;35m" << formatTime(showtime->time) << "\n";
        cout << "\033[0m";
        cout << "  Ghe da dat: ";
        cout << "\033[1;32m";
        for (int i = 0; i < seatCount; ++i) cout << seatCodes[i] << " ";
        cout << "\033[0m";
        cout << "\n";
        cout << "  So luong:   " << seatCount << " ve\n";
        cout << "  Tong tien:  " << seatCount * 75000 << " VND\n\n";
        cout << "\033[1;32m";
        cout << "  CAM ON QUY KHACH!\n\n";
        cout << "\033[0m";
        cout << "Nhan Enter de quay lai menu chinh...";
        cin.ignore();
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
                cout << "===== TIM KIEM KHACH HANG =====\n\n";
                cout << "Hien tai chua co khach hang nao dat ve.\n";
                cout << "Vui long dat ve truoc khi tim kiem thong tin khach hang.\n\n";
            cout << "Nhan Enter de quay lai menu chinh...";
            cin.ignore();
                return;
            }
            
            cout << "Nhap ten khach hang (0 de quay lai): ";
            string nameQuery; getline(cin, nameQuery);
            if (nameQuery == "0") return;
            if (!isValidName(nameQuery)) { cout << "Ten khong hop le, vui long nhap lai.\n";cin.ignore(); continue; }
            string nameQueryNorm = toLower(trim(nameQuery));

            const int MAX_NAME_MATCHES = 50;
            Customer* foundCustomers[MAX_NAME_MATCHES];
            int foundCount = 0;
            // Duyệt qua tất cả các bucket (O(TABLE_SIZE))
            for (int i = 0; i < HashTable::TABLE_SIZE; ++i) {
                // Duyệt qua Linked List trong bucket hiện tại (O(collision_chain_length))
                Node<HashItem>* currentItemNode = customerTable.table[i].head; 
                
                while (currentItemNode != nullptr) {
                    // currentItemNode->data là một HashItem { string key, Customer* customerData }
                    if (toLower(currentItemNode->data.customerData->name) == nameQueryNorm) { 
                        if (foundCount < MAX_NAME_MATCHES) {
                            foundCustomers[foundCount++] = currentItemNode->data.customerData;
                        }
                    }
                    currentItemNode = currentItemNode->next;
                }
            }

            if (foundCount == 0) {
                cout << "Khong tim thay khach hang nao voi ten '" << nameQuery << "'.\n";cin.ignore();
                continue;
            } else if (foundCount == 1) {
                displayCustomerInfo(foundCustomers[0]);
                return;
            } else {
                cout << "Tim thay nhieu khach hang co ten trung nhau. Vui long nhap CCCD de xac dinh:\n";cin.ignore();
                for (int i = 0; i < foundCount; ++i)
                    cout << " - Ten: " << foundCustomers[i]->name << endl;
                cout << "Nhap CCCD (12 chu so, 0 de quay lai): ";
                string cccd; 
                getline(cin, cccd);
                if (cccd == "0") continue; // go back to name input
                if (!isValidCCCD(cccd)) { cout << "CCCD khong hop le.\n";cin.ignore(); continue; }
                auto foundByCCCD = customerTable.get(cccd);
                if (foundByCCCD.has_value()) { displayCustomerInfo(*foundByCCCD); return; }
                else { cout << "Khong tim thay theo CCCD da nhap.\n";cin.ignore(); continue; }
            }
        }
    }
    
    void CinemaSystem::displayCustomerInfo(Customer* customer) {
        clearScreen();
        cout << "\033[1;32m";
        cout << "===== THONG TIN KHACH HANG =====\n\n";
        cout << "\033[0m";
        cout << "Ten: " << customer->name << "\n";
        cout << "CCCD: " << customer->cccd << "\n\n";
        cout << "--- Cac ve da dat (da sap xep theo suat chieu moi nhat) ---\n";

            if(customer->bookings.isEmpty()) {
        cout << "Khach hang chua dat ve nao.\n";
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
            cout << " > Phim: " << "\033[1;33m"<< booking->movie->title << "\n";
            cout << "\033[0m";
            cout << "   Suat chieu: " << "\033[35m"<<formatTime(booking->showtime->time) << "\n";
            cout << "\033[0m";
            cout << "   Ghe: ";
            Node<string>* seatNode = booking->bookedSeats.head;
            while(seatNode) {
                cout << "\033[1;32m";
                cout << seatNode->data << " ";
                cout << "\033[0m";
                seatNode = seatNode->next;
            }
            cout << "\n\n";
        }
        
        delete[] bookingsPtrArray; // Giải phóng bộ nhớ mảng con trỏ
    }


        cout << "--------------------------------\n";
        cout << "\033[31m1.\033[0m Dat them ghe\n";
        cout << "\033[31m2\033[0m. Huy ghe\n";
        cout << "\033[31m0\033[0m. Quay lai menu chinh\n";
        cout << "Lua chon: ";
        string line;
        getline(cin, line);
        if (line == "1") processMovieSelectionForExistingCustomer(customer);
        else if (line == "2") cancelSeat(customer);
        else if (line == "0") return;
        else {
            cout << "Lua chon khong hop le, vui long nhan Enter nhap lai.";cin.ignore();
            displayCustomerInfo(customer); 
        }
    }
    
    void CinemaSystem::processMovieSelectionForExistingCustomer(Customer* customer) {
        while (true) {
            clearScreen();
            cout << "\033[1;36m===== CHON PHIM (DAT THEM) =====\033[0m\n";
            cout << "\033[31m1.\033[0m Sap xep theo \033[35mTen (A->Z)\033[0m\n";
            cout << "\033[31m2.\033[0m Sap xep theo \033[35mRating (cao->thap)\033[0m\n";
            cout << "\033[31m0.\033[0m Quay lai\n";
            cout << "\033[34mLua chon sap xep:\033[0m ";
            string sortChoice;
            getline(cin, sortChoice);

            if (sortChoice == "0") {
                displayCustomerInfo(customer);
                return;
            }

            if (sortChoice == "2") displayMovieListSortedByRating();
            else if (sortChoice == "1") displayMovieList();
            else {
                cout << "Thong tin khong hop le, vui long nhap lai.\n";
                cin.ignore();
                continue;
            }

            // --- Vòng lặp chọn phim ---
            while (true) {
                clearCurrentLine();
                cout << "\nChon phim ban muon dat them ve (0 de quay lai): ";
                string line;
                getline(cin, line);

                if (line == "0") {
                    displayCustomerInfo(customer);
                    return; // quay lại menu khách hàng
                }

                bool isNumber = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
                if (!isNumber || stoi(line) < 1 || stoi(line) > 5) {
                    cout << "Thong tin khong hop le, vui long nhan Enter de nhap lai.";
                    cin.ignore();
                    clearCurrentLine();

                    if (sortChoice == "2")
                        displayMovieListSortedByRating();
                    else
                        displayMovieList();

                    continue;
                }

                int movieIndex = stoi(line);
                Movie* movie = movieList.getData(movieIndex - 1);
                if (movie) {
                    bool booked = processShowtimeSelection(movie, customer);
                    if (booked) {
                        displayCustomerInfo(customer);
                        return;
                    }
                } else {
                    cout << "Phim không hợp lệ!\n";
                }

                // nếu người dùng chọn 0 ở bước chọn suất chiếu -> quay lại chọn phim
                continue;
            }
        }
    }