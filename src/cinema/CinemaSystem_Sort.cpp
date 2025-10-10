#include "CinemaSystem.h"
#include <algorithm>
using namespace std;
    // === THUẬT TOÁN SẮP XẾP 1: INSERTION SORT (cho danh sách phim) ===
    void CinemaSystem::sortMoviesByTitle() {
        if (!movieList.head || !movieList.head->next) return;
        Node<Movie>* i = movieList.head->next;
        while (i) {
            Movie key = i->data;
            Node<Movie>* j = i->prev;
            while (j && j->data.title > key.title) {
                j->next->data = j->data;
                j = j->prev;
            }
            if (j)
                j->next->data = key;
            else
                movieList.head->data = key;
            i = i->next;
        }
    }

    // === THUẬT TOÁN SẮP XẾP 3: QUICK SORT (sắp xếp phim theo rating giảm dần) ===
    void CinemaSystem::sortMoviesByRatingDesc() {
        if (!movieList.head || !movieList.head->next) return;

        for (Node<Movie>* i = movieList.head; i; i = i->next) {
            Node<Movie>* maxNode = i;
            for (Node<Movie>* j = i->next; j; j = j->next) {
                if (j->data.rating > maxNode->data.rating)
                    maxNode = j;
            }
            if (maxNode != i)
                std::swap(i->data, maxNode->data);
        }
    }
    void CinemaSystem::quickSortMovieByRaing() {
        clearScreen();
        std::cout << "\033[1;36m";
        std::cout << "===== DANH SACH PHIM (Rating giam dan) =====\n\n";
        std::cout << "\033[0m";

        LinkedList<Movie> sortedList = movieList;
        sortMoviesByRatingDesc(); // hoặc: gọi riêng cho sortedList nếu tách ra

        int index = 1;
        Node<Movie>* current = sortedList.head;
        while (current) {
            std::cout << " " << "\033[31m" << index++ << ". \033[0m"
                    << current->data.title << " (" << current->data.rating << ")\n";
            current = current->next;
        }

        std::cout << "\n\033[36m0. Quay lai\033[0m\n";
    }
    // Thêm hàm so sánh cho các con trỏ Booking
    bool CinemaSystem::compareBookingPtrs(const Booking* a, const Booking* b) {
    // Sắp xếp theo thời gian suất chiếu tăng dần (gần nhất trước)
    return a->showtime->time < b->showtime->time; 
    }
    // === THUẬT TOÁN SẮP XẾP 2: MERGE SORT (cho lịch sử đặt vé) ===
    // Hàm trộn hai mảng con đã sắp xếp
    void CinemaSystem::mergeBookingPtrs(Booking* arr[], int left, int mid, int right) {
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
    void CinemaSystem::mergeSortBookingPtrs(Booking* arr[], int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortBookingPtrs(arr, left, mid);
            mergeSortBookingPtrs(arr, mid + 1, right);
            mergeBookingPtrs(arr, left, mid, right);
        }
    }