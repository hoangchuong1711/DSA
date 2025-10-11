#include "CinemaSystem.h"
#include <algorithm>
using namespace std;

    // Các hàm hiển thị giao diện 
    void CinemaSystem::displayMainMenu() {
        clearScreen();
        std::cout << "\033[1;36m"; 
        std::cout << "===== MENU RAP CHIEU PHIM =====\n\n";
        std::cout << "\033[0m"; 
        std::cout << "\033[31m" << "1. " << "\033[0m" <<"Chon phim\n";
        std::cout << "\033[31m" << "2. " << "\033[0m" <<"Tim kiem nguoi dat\n";
        std::cout << "\033[31m" << "3. " << "\033[0m" <<"Thoat\n";
        std::cout << "\033[36m"; 
        std::cout << "Lua chon cua ban: ";
        std::cout << "\033[0m"; 
    }

    void CinemaSystem::displayMovieList() {
        clearScreen();
        std::cout << "\033[1;36m";
        std::cout << "===== DANH SACH PHIM =====\n\n";
        std::cout << "\033[0m";

        sortMoviesByTitle(); // nếu hàm này đã hỗ trợ LinkedList, giữ lại

        int index = 1;
        Node<Movie>* current = movieList.head;
        while (current) {
            std::cout << " " << "\033[31m" << index++ << ". " << "\033[0m"
                    << current->data.title << "\n";
            current = current->next;
        }

        std::cout << "\n\033[36m0. Quay lai\033[0m\n";
    }

    void CinemaSystem::displayMovieListSortedByRating() {
        clearScreen();
        std::cout << "\033[1;36m";
        std::cout << "===== DANH SACH PHIM (Rating giam dan) =====\n\n";
        std::cout << "\033[0m";

        LinkedList<Movie> sortedList = movieList;

        Node<Movie>* i = sortedList.head;
        while (i) {
            Node<Movie>* j = i->next;
            while (j) {
                if (i->data.rating < j->data.rating)
                    std::swap(i->data, j->data);
                j = j->next;
            }
            i = i->next;
        }

        int index = 1;
        Node<Movie>* current = sortedList.head;
        while (current) {
            std::cout << " " << "\033[31m" << index++ << ". " << "\033[0m"
                    << current->data.title << " (" << current->data.rating << ")\n";
            current = current->next;
        }

        std::cout << "\n\033[36m0. Quay lai\033[0m\n";
    }

    void CinemaSystem::displayMoviesBySortChoice(const string& sortChoice) {
        clearScreen();
        if (sortChoice == "1")
            displayMovieList();
        else if (sortChoice == "2")
            displayMovieListSortedByRating();
    }

    void printWrapped(const string& text, int width) {
        istringstream words(text);
        string word;
        int lineLen = 0;
        while (words >> word) {
            if (lineLen + word.length() + 1 > width) {
                cout << "\n"; // xuống dòng khi vượt quá width
                lineLen = 0;
            }
            if (lineLen > 0) cout << " "; // cách giữa các từ
            cout << word;
            lineLen += word.length() + 1;
        }
        cout << "\n";
    }

    void CinemaSystem::displayMovieDetails(const Movie& movie) {
        const int width = 70; // độ rộng khung

        // Khung trên
        cout << "\033[1;34m"; // xanh đậm
        for (int i = 0; i < width; ++i) cout << "=";
        cout << "\033[0m\n";

        // Tiêu đề
        cout << "\033[1;36m"; // xanh dương nhạt
        cout << "Ten: " << movie.title << "\033[0m\n";

        // Giới hạn tuổi và đánh giá
        cout << "\033[1;33m"; // vàng nhạt
        cout << "Gioi han tuoi: " << movie.ageLimit
            << "    Danh gia: " << movie.rating << "\033[0m\n";

        // Thời lượng và đạo diễn
        cout << "\033[1;33m";
        cout << "Thoi luong: " << movie.duration << " phut"
            << "    Dao dien: " << movie.director << "\033[0m\n";

        // Nội dung chính với gói chữ
        cout << "\033[0;37m"; // trắng
        cout << "\nNoi dung chinh:\n";
        printWrapped(movie.synopsis, width);

        // Khung dưới
        cout << "\033[1;34m";
        for (int i = 0; i < width; ++i) cout << "=";
        cout << "\033[0m\n\n";
    }

    void CinemaSystem::displaySeatMap(const string& movieTitle, Showtime& showtime ) {
        clearScreen();

        std::cout << "\033[1;36m";
        std::cout << "SO DO GHE - Phim: " << movieTitle << "\n";
        std::cout << "Suat chieu: " << formatTime(showtime.time) << "\n\n";
        gotoXY(14, 5); std::cout << "--- MAN HINH ---\n\n";
        std::cout << "\033[0m\n";

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

    void CinemaSystem::displayBookingList(Booking** arr, int count) {      
        for (int i = 0; i < count; ++i) {
            Booking* booking = arr[i];
            cout << " \033[31m" << (i + 1) << ".\033[0m Phim: " << "\033[1;33m" << booking->movie->title << "\033[0m\n";
            cout << "    Suat chieu: \033[35m" << formatTime(booking->showtime->time) << "\033[0m\n";
            cout << "    Ghe: ";

            Node<string>* seatNode = booking->bookedSeats.head;
            while (seatNode) {
                cout << "\033[1;32m" << seatNode->data << "\033[0m ";
                seatNode = seatNode->next;
            }
            cout << "(" << booking->bookedSeats.size() << " ghe)\n\n";
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
            Booking** bookingsPtrArray = customer->bookings.listToPtrArray();

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