#pragma once
#include "models.h"
#include "utils.h"

class MovieManager {
private:
    Movie movies[MAX_MOVIES];
    int movieCount;

    void initMovies(); // Khởi tạo dữ liệu phim mẫu
    bool isFull(const Show &sh);

public:
    MovieManager(); // Constructor
    // Các chức năng con
    void listMoviesDefault();
    void listMoviesSortedMenu();
    void listMoviesWithMode(int mode);
    Movie* chooseMovieByIndex(int idx);
    Movie* getMoviesArray() { return movies; }
    int getMovieCount() const { return movieCount; }
};