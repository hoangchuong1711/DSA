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
                        else displayMoviesBySortChoice(sortChoice);
                    }
                    continue;
                }
                
            }
        }
    }
    
    bool CinemaSystem::processShowtimeSelection(Movie* movie, Customer* existingCustomer) {
        time_t now = time(0);
        tm localNowTm;
        localtime_s(&localNowTm, &now);

        int selectedDayOffset = 0; // 0 = hôm nay, 1 = ngày mai
        int validShowtimeIndices[MAX_SHOWTIMES_PER_MOVIE];
        while (true) {
            clearScreen();
            // Hiển thị chi tiết phim
            displayMovieDetails(*movie);
            cout << "\033[1;36m";
            
            cout << "===== VUI LONG CHON SUAT CHIEU =====\n\n";
            cout << "\033[0m";
            // ==== Tính ngày hôm nay và ngày mai ====
            tm today = localNowTm;
            tm tomorrow = localNowTm;
            tomorrow.tm_mday += 1;
            mktime(&tomorrow);

            char todayStr[20], tomorrowStr[20];
            strftime(todayStr, sizeof(todayStr), "%d/%m/%Y", &today);
            strftime(tomorrowStr, sizeof(tomorrowStr), "%d/%m/%Y", &tomorrow);

            cout << "\t";
            if (selectedDayOffset == 0)
                cout << "\033[1;33mHom nay\033[0m";
            else
                cout << "Hom nay";

            cout << "\t\t\t";
            if (selectedDayOffset == 1)
                cout << "\033[1;33mNgay mai\033[0m";
            else
                cout << "Ngay mai";

            cout << "\n\t" << todayStr << "\t\t" << tomorrowStr << "\n\n";
            int validCount = 0;

            // Tính ngày bắt đầu và kết thúc
            tm targetTm = localNowTm;
            targetTm.tm_hour = 0;
            targetTm.tm_min = 0;
            targetTm.tm_sec = 0;
            targetTm.tm_mday += selectedDayOffset;
            time_t targetStart = mktime(&targetTm);

            tm nextTm = targetTm;
            nextTm.tm_mday += 1;
            time_t targetEnd = mktime(&nextTm);

            for (int i = 0; i < movie->showtimeCount; ++i) {
                time_t st = movie->showtimes[i].time;

                if (st >= targetStart && st < targetEnd) {
                    if (selectedDayOffset == 0 && st <= now)
                        continue; // bỏ qua suất chiếu đã qua nếu là hôm nay

                    cout << " " << "\033[31m" << validCount + 1 << ". \033[0m"
                        << formatTime(st) << "\n";
                    validShowtimeIndices[validCount] = i;
                    validCount++;
                }
            }

            if (validCount == 0)
                cout << "Khong co suat chieu nao cho ngay nay.\n";

            cout << "\n \033[31mn.\033[0m ";
            if (selectedDayOffset == 0)
                cout << "Ngay tiep theo\n";
            else
                cout << "Ngay truoc\n";

            cout << " \033[31m0.\033[0m Quay lai\n";

            cout << "\033[34m\nChon suat chieu (hoac 'n' de chuyen ngay): \033[0m";
            string line;
            getline(cin, line);

            if (line == "0") return false;
            if (line == "n" || line == "N") {
                selectedDayOffset = 1 - selectedDayOffset; // đảo qua lại giữa hôm nay và ngày mai
                continue;
            }
            bool isNumber = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
            if (!isNumber || stoi(line) < 1 || stoi(line) > validCount) {
                cout << "Thong tin khong hop le. Nhan Enter de nhap lai.";
                cin.ignore();
                continue;
            }
            int choice = stoi(line);
            int actualIndex = validShowtimeIndices[choice - 1];
            bool booked = handleBooking(movie, &movie->showtimes[actualIndex], existingCustomer);
            if (booked) return true;
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
                    allSeatsValid = false; 
                    break;
                }
                
                int row = code[0] - 'A', col = stoi(code.substr(1)) - 1;
                if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) {
                    cout << "Ma ghe '" << code << "' khong ton tai.\n"; 
                    allSeatsValid = false; 
                    break;
                }
                
                Seat& seat = showtime->seats[row][col];
                if (seat.state == BOOKED) {
                    cout << "Ghe '" << code << "' da co nguoi dat. Vui long nhap lai danh sach ghe.\n"; 
                    allSeatsValid = false; 
                    break;
                }
                if (seat.state == RESERVED && seat.reservedByCCCD != customerCCCD) {
                    cout << "Ghe '" << code << "' da co nguoi khac dat tam. Vui long nhap lai danh sach ghe.\n"; 
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


    void CinemaSystem::processCustomerSearch() {
        while (true) {
            clearScreen();
            
            // Check if customer table is completely empty
            bool isCustomerTableEmpty  = customerTable.isEmpty();           
            if (isCustomerTableEmpty ) {
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
            int foundCount = customerTable.findByName(nameQuery, foundCustomers, MAX_NAME_MATCHES);

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