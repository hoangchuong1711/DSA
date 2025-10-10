#include "CinemaSystem.h"
#include <algorithm>

    // === HUY GHE ===
    int CinemaSystem::findSeatIndexInBooking(const Booking& booking, const std::string& seatCodeUpper) {
        Node<std::string>* node = booking.bookedSeats.head;
        int idx = 0;
        while (node) {
            if (toUpper(node->data) == seatCodeUpper) return idx;
            node = node->next; idx++;
        }
        return -1;
    }

    bool CinemaSystem::parseSeatCode(const std::string& codeUpper, int& row, int& col) {
        if (codeUpper.length() < 2 || !isalpha(codeUpper[0])) return false;
        for (size_t i = 1; i < codeUpper.length(); ++i) if (!isdigit(codeUpper[i])) return false;
        row = codeUpper[0] - 'A';
        col = std::stoi(codeUpper.substr(1)) - 1;
        if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) return false;
        return true;
    }

void CinemaSystem::cancelSeat(Customer* customer) {
    while (true) {
        clearScreen();
        std::cout << "\033[31m===== HUY GHE =====\033[0m\n\n";
        if (customer->bookings.isEmpty()) {
            std::cout << "Ban chua co ve nao de huy.\n";
            std::cout << "Nhan Enter de quay lai..."; std::cin.ignore();
            displayCustomerInfo(customer);
            return;
        }

        // Hien thi tat ca cac ve da dat voi index
        std::cout << "\033[32mCac ve da dat:\033[0m\n";
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
            std::cout << " \033[31m" << (i + 1) << ".\033[0m Phim: " <<"\033[1;33m "<< booking->movie->title << "\n";
            std::cout << "\033[0m";
            std::cout << "    Suat chieu: " << " \033[35m" << formatTime(booking->showtime->time) << "\n";
            std::cout << "\033[0m";
            std::cout << "    Ghe: ";
            Node<std::string>* seatNode = booking->bookedSeats.head;
            while(seatNode) {
                std::cout << "\033[1;32m"<< seatNode->data << "\033[0m" << " ";
                seatNode = seatNode->next;
            }
            std::cout << "(" << booking->bookedSeats.size() << " ghe)\n\n";
        }

        std::cout << "\033[31m0.\033[0m Quay lai\n";
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
            std::cout << "\nVe nay chi co 1 ghe. Ban co chac muon huy ve nay? (\033[32my\033[0m/\033[31mn\033[0m): ";
            std::string confirm; std::getline(std::cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0]) != 'y' && tolower((unsigned char)confirm[0]) != 'n')) {
                std::cout << "Lua chon khong hop le. Vui long nhap \033[32my\033[0m/\033[31mn\033[0m.\n";std::cin.ignore();
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
            std::cout << "\033[32m===== HUY VE THANH CONG =====\033[0m\n\n";
            std::cout << "Phim: " << "\033[1;33m"<< movieTitle <<"\033[0m"  << "\n";
            std::cout << "Suat chieu: " << "\033[1;35m"<< showtimeStr <<"\033[0m" << "\n";
            std::cout << "\033[0m";
            std::cout << "Ghe da huy: " << "\033[1;32m" <<seatCodeUpper  <<"\033[0m" <<  "\n";
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
            std::cout << "\033[1;36m\n";
            std::cout << "===== CHON GHE CAN HUY =====\n\n";
            std::cout << "\033[0m";
            std::cout << "Phim: " << "\033[1;33m"<<selectedBookingPtr->movie->title << "\n";
            std::cout << "\033[0m";
            std::cout << "Suat chieu: " << "\033[35m" << formatTime(selectedBookingPtr->showtime->time) << "\n\n";
            std::cout << "\033[0m";
            std::cout << "Cac ghe da dat:\n";
            
            // Hiển thị các ghế với index
            Node<std::string>* seatNode = selectedBookingPtr->bookedSeats.head;
            int seatIndex = 0;
            while(seatNode) {
                std::cout << " \033[31m" << (seatIndex + 1) << ".\033[0m " << "\033[1;32m"<< seatNode->data << "\n";
                std::cout << "\033[0m";
                seatNode = seatNode->next;
                seatIndex++;
            }
            
            // ... (Phần chọn ghế, kiểm tra index)
            std::cout << "\n \033[31m0.\033[0m Quay lai\n";
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
            LinkedList<int> indices;
            auto containsIndex = [&](int v) {
                Node<int>* cur = indices.head;
                while (cur) {
                    if (cur->data == v) return true;
                    cur = cur->next;
                }
                return false;
            };
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
                        if (!containsIndex(asIndex)) indices.add(asIndex);
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
                        if (!containsIndex(oneBased)) indices.add(oneBased);
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
            
            if (indices.isEmpty()) { 
                // Trường hợp người dùng nhập các token hợp lệ nhưng đã bị trùng (đã được thêm vào 'indices') 
                // hoặc chỉ nhập khoảng trắng/không nhập gì.
                std::cout << "\n[LOI]: Vui long nhap it nhat mot so thu tu hoac ma ghe de huy.\n";
                std::cout << "Nhan Enter de thu lai..."; std::cin.ignore();
                delete[] bookingsPtrArray;
                continue; 
            }

            // Build list of codes to cancel for confirmation
            LinkedList<std::string> toCancelCodes;
            Node<int>* idxNode = indices.head;
            while (idxNode) {
                int oneBasedIdx = idxNode->data;
                Node<std::string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i)
                    sn = sn->next;
                toCancelCodes.add(toUpperStr(sn->data));
                idxNode = idxNode->next;
            }
            std::cout << "\nCac ghe se huy: ";
            Node<std::string>* cNode = toCancelCodes.head;
            while (cNode) {
                std::cout << "\033[1;32m" << cNode->data << "\033[0m" << " ";
                cNode = cNode->next;
            }
            std::cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n";
            std::cout << "Xac nhan (\033[31my\033[0m/\033[32mn\033[0m): ";
            std::string confirm; std::getline(std::cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0])!='y' && tolower((unsigned char)confirm[0])!='n')) { delete[] bookingsPtrArray; continue; }
            if (tolower((unsigned char)confirm[0])=='n') { delete[] bookingsPtrArray; continue; }

            // Sort desc to remove safely
            indices.sortDesc();

            // Cancel seats: update showtime then remove from booking
            Node<int>* idxNode2 = indices.head;
            while (idxNode2) {
                int oneBasedIdx = idxNode2->data;
                Node<std::string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i)
                    sn = sn->next;

                std::string codeUpper = toUpperStr(sn->data);
                int pr=-1, pc=-1;
                if (parseSeatCode(codeUpper, pr, pc)) {
                    selectedBookingPtr->showtime->seats[pr][pc].state = AVAILABLE;
                    selectedBookingPtr->showtime->seats[pr][pc].bookedByCCCD.clear();
                }

                selectedBookingPtr->bookedSeats.removeAt(oneBasedIdx - 1);
                idxNode2 = idxNode2->next;
            }
            // Show result
            clearScreen();
            std::cout << "\033[32m===== HUY GHE THANH CONG =====\033[0m\n\n";
            std::cout << "Phim: \033[1;33m" << selectedBookingPtr->movie->title << "\033[0m\n";
            std::cout << "Suat chieu: \033[1;35m" << formatTime(selectedBookingPtr->showtime->time) << "\033[0m\n";
            std::cout << "Cac ghe da huy: ";
            Node<std::string>* canceledNode = toCancelCodes.head;
            while (canceledNode) {
                std::cout <<"\033[1;32m"<< canceledNode->data << "\033[0m ";
                canceledNode = canceledNode->next;
            }
            std::cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n\n";

            int remaining = selectedBookingPtr->bookedSeats.size();
            if (remaining > 0) {
                std::cout << "Ghe con lai: ";
                Node<std::string>* remainingSeatNode = selectedBookingPtr->bookedSeats.head;
                while (remainingSeatNode) { std::cout << "\033[1;32m"<<remainingSeatNode->data << "\033[0m "; remainingSeatNode = remainingSeatNode->next; }
                std::cout << "\nSo ghe con lai: \033[1;32m" << remaining << "\033[0m\n";
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