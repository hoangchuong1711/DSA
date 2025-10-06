#include "CinemaSystem.h"
#include <algorithm>

    // === THUẬT TOÁN SẮP XẾP 1: INSERTION SORT (cho danh sách phim) ===
    void CinemaSystem::sortMoviesByTitle() {
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
    int CinemaSystem::partitionByRating(Movie arr[], int left, int right) {
        double pivot = arr[(left + right) / 2].rating; // pivot rating
        int i = left, j = right;
        while (i <= j) {
            while (arr[i].rating > pivot) i++;      // rating cao đứng trước
            while (arr[j].rating < pivot) j--;      // rating thấp đứng sau
            if (i <= j) { std::swap(arr[i], arr[j]); i++; j--; }
        }
        return i;
    }
    void CinemaSystem::quickSortMoviesByRating(Movie arr[], int left, int right) {
        if (left >= right) return;
        int idx = partitionByRating(arr, left, right);
        if (left < idx - 1) quickSortMoviesByRating(arr, left, idx - 1);
        if (idx < right) quickSortMoviesByRating(arr, idx, right);
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