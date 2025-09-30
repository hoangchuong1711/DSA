// CinemaSystem.h
#pragma once

#include "models.h"

class CinemaSystem {
private:
    Movie movies[MAX_MOVIES];
    int movieCount;

    void initMovies(); // Khởi tạo dữ liệu phim mẫu
    bool isFull(const Show &sh);

public:
    CinemaSystem(); // Constructor

    // Các chức năng chính
    void displayMainMenu();
    void flowListAndBook();
    void searchUserAndAction();

    // Các chức năng con
    void listMoviesDefault();
    void listMoviesSortedMenu();
    void listMoviesWithMode(int mode);
    Movie* chooseMovieByIndex(int idx);
    int chooseShowForMovie(Movie &m);
    void displaySeats(const Show &sh);
    void bookMultipleSeats(Show &sh);
    void cancelSeat(Show &sh);
    void postSearchActions();
};