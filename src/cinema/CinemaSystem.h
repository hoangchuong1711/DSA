#ifndef CINEMA_SYSTEM_H
#define CINEMA_SYSTEM_H

#include "../linkedList.h"
#include "../models.h"
#include "../hashTable.h"
#include "../utils.h"
#include <string>
#include <ctime>

class CinemaSystem {
public:
    CinemaSystem();
    void run();

private:
    LinkedList<Movie> movieList;
    HashTable customerTable;

    // ===== HÀM KHỞI TẠO DỮ LIỆU =====
    void initializeMovies();
    time_t createTodayShowtime(int hour, int minute);

    // ===== HÀM SẮP XẾP =====
    void sortMoviesByTitle();             // Sắp xếp tên (A-Z)
    void sortMoviesByRatingDesc();
    void quickSortMovieByRaing();        // Sắp xếp rating giảm dần

    // ===== HÀM MERGE SORT =====
    bool compareBookingPtrs(const Booking* a, const Booking* b);
    void mergeBookingPtrs(Booking* arr[], int left, int mid, int right);
    void mergeSortBookingPtrs(Booking* arr[], int left, int right);

    // ===== HIỂN THỊ GIAO DIỆN =====
    void displayMainMenu();
    void displayMovieList();
    void displayMovieListSortedByRating();
    void displaySeatMap(const std::string& movieTitle, Showtime& showtime);
    void clearReservations(Showtime& showtime);

    // ===== XỬ LÝ NGHIỆP VỤ =====
    void processMovieSelection();
    bool processShowtimeSelection(Movie* movie, Customer* existingCustomer);
    bool handleBooking(Movie* movie, Showtime* showtime, Customer* existingCustomer);
    void printReceipt(const Customer& customer, const Movie* movie, const Showtime* showtime, const std::string seatCodes[], int seatCount);
    void processCustomerSearch();
    void displayCustomerInfo(Customer* customer);
    void processMovieSelectionForExistingCustomer(Customer* customer);
    
    // === HUY GHE ===
    int findSeatIndexInBooking(const Booking& booking, const std::string& seatCodeUpper);
    bool parseSeatCode(const std::string& codeUpper, int& row, int& col);
    void cancelSeat(Customer* customer);
};

#endif // CINEMA_SYSTEM_H
