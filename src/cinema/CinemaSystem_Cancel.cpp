#include "CinemaSystem.h"
#include <algorithm>
using namespace std;
    // === HUY GHE ===
    int CinemaSystem::findSeatIndexInBooking(const Booking& booking, const string& seatCodeUpper) {
        Node<string>* node = booking.bookedSeats.head;
        int idx = 0;
        while (node) {
            if (toUpper(node->data) == seatCodeUpper) return idx;
            node = node->next; idx++;
        }
        return -1;
    }

    bool CinemaSystem::parseSeatCode(const string& codeUpper, int& row, int& col) {
        if (codeUpper.length() < 2 || !isalpha(codeUpper[0])) return false;
        for (size_t i = 1; i < codeUpper.length(); ++i) if (!isdigit(codeUpper[i])) return false;
        row = codeUpper[0] - 'A';
        col = stoi(codeUpper.substr(1)) - 1;
        if (row < 0 || row >= SEAT_ROWS || col < 0 || col >= SEAT_COLS) return false;
        return true;
    }

void CinemaSystem::cancelSeat(Customer* customer) {
    while (true) {
        clearScreen();
        cout << "\033[31m===== HUY GHE =====\033[0m\n\n";
        if (customer->bookings.isEmpty()) {
            cout << "Ban chua co ve nao de huy.\n";
            cout << "Nhan Enter de quay lai..."; cin.ignore();
            displayCustomerInfo(customer);
            return;
        }

        // Hien thi tat ca cac ve da dat voi index
        cout << "\033[32mCac ve da dat:\033[0m\n";
        cout << "----------------------------------------\n";
        
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
            cout << " \033[31m" << (i + 1) << ".\033[0m Phim: " <<"\033[1;33m "<< booking->movie->title << "\n";
            cout << "\033[0m";
            cout << "    Suat chieu: " << " \033[35m" << formatTime(booking->showtime->time) << "\n";
            cout << "\033[0m";
            cout << "    Ghe: ";
            Node<string>* seatNode = booking->bookedSeats.head;
            while(seatNode) {
                cout << "\033[1;32m"<< seatNode->data << "\033[0m" << " ";
                seatNode = seatNode->next;
            }
            cout << "(" << booking->bookedSeats.size() << " ghe)\n\n";
        }

        cout << "\033[31m0.\033[0m Quay lai\n";
        cout << "----------------------------------------\n";
        cout << "Chon ve muon huy (nhap so thu tu): ";
        
        string choice; getline(cin, choice);
        if (choice == "0") {
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }

        int ticketIndex;
        try { 
            ticketIndex = stoi(choice); 
        } catch (...) { 
            cout << "Vui long nhap so hop le.\n"; cin.ignore();
            delete[] bookingsPtrArray;
            continue; 
        }

        if (ticketIndex < 1 || ticketIndex > bookingCount) {
            cout << "Lua chon khong hop le. Vui long nhap lai.\n";cin.ignore();
            delete[] bookingsPtrArray;
            continue;
        }

        // Lấy con trỏ VÉ được chọn
        Booking* selectedBookingPtr = bookingsPtrArray[ticketIndex - 1]; // Lấy con trỏ
        int seatCount = selectedBookingPtr->bookedSeats.size();

        // Xử lý hủy toàn bộ vé (1 ghế) hoặc chọn ghế (nhiều ghế)
        // ... (Logic tương tự như code cũ, nhưng sử dụng 'selectedBookingPtr->' thay cho 'selectedBooking.')
        string movieTitle = selectedBookingPtr->movie->title;
        string showtimeStr = formatTime(selectedBookingPtr->showtime->time);
        string seatCodeUpper = toUpper(selectedBookingPtr->bookedSeats.head->data);
        // Nếu vé chỉ có 1 ghế, hủy luôn
        if (seatCount == 1) {
            cout << "\nVe nay chi co 1 ghe. Ban co chac muon huy ve nay? (y/n): ";
            string confirm; getline(cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0]) != 'y' && tolower((unsigned char)confirm[0]) != 'n')) {
                cout << "Lua chon khong hop le. Vui long nhap y/n.\n";cin.ignore();
                delete[] bookingsPtrArray;
                continue;
            }
            if (tolower((unsigned char)confirm[0]) == 'n') {
                cout << "Da huy thao tac.\n";cin.ignore();
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
            cout << "\033[32m===== HUY VE THANH CONG =====\033[0m\n\n";
            cout << "Phim: " << "\033[1;33m"<< movieTitle <<"\033[0m"  << "\n";
            cout << "Suat chieu: " << "\033[1;35m"<< showtimeStr <<"\033[0m" << "\n";
            cout << "\033[0m";
            cout << "Ghe da huy: " << "\033[1;32m" <<seatCodeUpper  <<"\033[0m" <<  "\n";
            cout << "So tien hoan lai: " << 75000 << " VND\n";
            cout << "Ve da duoc xoa khoi danh sach.\n\n";
            cout << "Nhan Enter de quay lai..."; cin.ignore();
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }
        // Nếu vé có nhiều ghế, cho chọn ghế cụ thể
        else {
            clearScreen();
            cout << "\033[1;36m\n";
            cout << "===== CHON GHE CAN HUY =====\n\n";
            cout << "\033[0m";
            cout << "Phim: " << "\033[1;33m"<<selectedBookingPtr->movie->title << "\n";
            cout << "\033[0m";
            cout << "Suat chieu: " << "\033[35m" << formatTime(selectedBookingPtr->showtime->time) << "\n\n";
            cout << "\033[0m";
            cout << "Cac ghe da dat:\n";
            
            // Hiển thị các ghế với index
            Node<string>* seatNode = selectedBookingPtr->bookedSeats.head;
            int seatIndex = 0;
            while(seatNode) {
                cout << " \033[31m" << (seatIndex + 1) << ".\033[0m " << "\033[1;32m"<< seatNode->data << "\n";
                cout << "\033[0m";
                seatNode = seatNode->next;
                seatIndex++;
            }
            
            // ... (Phần chọn ghế, kiểm tra index)
            cout << "\n \033[31m0.\033[0m Quay lai\n";
            cout << "----------------------------------------\n";
            cout << "Chon ghe muon huy (nhap so thu tu): ";
            
            string seatChoice; getline(cin, seatChoice);
            if (seatChoice == "0") {
                delete[] bookingsPtrArray;
                cancelSeat(customer);
                return;
            }

            // Parse multiple tokens: indices (1-based) or seat codes (e.g., A1)
            stringstream ss(seatChoice);
            LinkedList<int> indices;
            auto containsIndex = [&](int v) {
                Node<int>* cur = indices.head;
                while (cur) {
                    if (cur->data == v) return true;
                    cur = cur->next;
                }
                return false;
            };
            auto toUpperStr = [](string s){ for(char& c: s) c = (char)toupper((unsigned char)c); return s; };
            string tok;
            bool anyInvalid = false;
            while (ss >> tok) {
                // Try as integer
                bool accepted = false;
                try {
                    size_t pos = 0;
                    int asIndex = stoi(tok, &pos);
                    if (pos == tok.size() && asIndex >= 1 && asIndex <= seatCount) {
                        if (!containsIndex(asIndex)) indices.add(asIndex);
                        accepted = true;
                    }
                } catch (...) {}
                if (accepted) continue;

                // Try as seat code
                string codeUpper = toUpperStr(tok);
                int row=-1,col=-1;
                if (parseSeatCode(codeUpper, row, col)) {
                    int foundIdx = findSeatIndexInBooking(*selectedBookingPtr, codeUpper);
                    if (foundIdx != -1) {
                        int oneBased = foundIdx + 1;
                        if (!containsIndex(oneBased)) indices.add(oneBased);
                        continue;
                    }
                }
                cout << "\n[LOI]: \"" << tok << "\" khong phai la so thu tu hop le (1-" << seatCount << ") hoac ma ghe da dat.\n"; // THÔNG BÁO LỖI
                anyInvalid = true;
                break;
            }

            if (anyInvalid) {
                cout << "Nhan Enter de thu lai..."; cin.ignore();
                delete[] bookingsPtrArray; 
                continue; 
            }
            
            if (indices.isEmpty()) { 
                // Trường hợp người dùng nhập các token hợp lệ nhưng đã bị trùng (đã được thêm vào 'indices') 
                // hoặc chỉ nhập khoảng trắng/không nhập gì.
                cout << "\n[LOI]: Vui long nhap it nhat mot so thu tu hoac ma ghe de huy.\n";
                cout << "Nhan Enter de thu lai..."; cin.ignore();
                delete[] bookingsPtrArray;
                continue; 
            }

            // Build list of codes to cancel for confirmation
            LinkedList<string> toCancelCodes;
            Node<int>* idxNode = indices.head;
            while (idxNode) {
                int oneBasedIdx = idxNode->data;
                Node<string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i)
                    sn = sn->next;
                toCancelCodes.add(toUpperStr(sn->data));
                idxNode = idxNode->next;
            }
            cout << "\nCac ghe se huy: ";
            Node<string>* cNode = toCancelCodes.head;
            while (cNode) {
                cout << "\033[1;32m" << cNode->data << "\033[0m" << " ";
                cNode = cNode->next;
            }
            cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n";
            cout << "Xac nhan (\033[31my\033[0m/\033[32mn\033[0m): ";
            string confirm; getline(cin, confirm);
            if (confirm.empty() || (tolower((unsigned char)confirm[0])!='y' && tolower((unsigned char)confirm[0])!='n')) { delete[] bookingsPtrArray; continue; }
            if (tolower((unsigned char)confirm[0])=='n') { delete[] bookingsPtrArray; continue; }

            // Sort desc to remove safely
            indices.sortDesc();

            // Cancel seats: update showtime then remove from booking
            Node<int>* idxNode2 = indices.head;
            while (idxNode2) {
                int oneBasedIdx = idxNode2->data;
                Node<string>* sn = selectedBookingPtr->bookedSeats.head;
                for (int i = 1; i < oneBasedIdx; ++i)
                    sn = sn->next;

                string codeUpper = toUpperStr(sn->data);
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
            cout << "\033[32m===== HUY GHE THANH CONG =====\033[0m\n\n";
            cout << "Phim: \033[1;33m" << selectedBookingPtr->movie->title << "\033[0m\n";
            cout << "Suat chieu: \033[1;35m" << formatTime(selectedBookingPtr->showtime->time) << "\033[0m\n";
            cout << "Cac ghe da huy: ";
            Node<string>* canceledNode = toCancelCodes.head;
            while (canceledNode) {
                cout <<"\033[1;32m"<< canceledNode->data << "\033[0m ";
                canceledNode = canceledNode->next;
            }
            cout << "\nSo tien hoan lai: " << (int)toCancelCodes.size()*75000 << " VND\n\n";

            int remaining = selectedBookingPtr->bookedSeats.size();
            if (remaining > 0) {
                cout << "Ghe con lai: ";
                Node<string>* remainingSeatNode = selectedBookingPtr->bookedSeats.head;
                while (remainingSeatNode) { cout << "\033[1;32m"<<remainingSeatNode->data << "\033[0m "; remainingSeatNode = remainingSeatNode->next; }
                cout << "\nSo ghe con lai: \033[1;32m" << remaining << "\033[0m\n";
            } else {
                Node<Booking>* nodeToDelete = customer->bookings.head;
                int indexToDelete = 0;
                while (nodeToDelete) {
                    if (&nodeToDelete->data == selectedBookingPtr) { customer->bookings.removeAt(indexToDelete); cout << "Ve da duoc xoa khoi danh sach (khong con ghe nao).\n"; break; }
                    nodeToDelete = nodeToDelete->next; indexToDelete++;
                }
            }

            cout << "\nNhan Enter de quay lai..."; cin.ignore();
            delete[] bookingsPtrArray;
            displayCustomerInfo(customer);
            return;
        }
    }
}