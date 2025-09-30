#pragma once
#include "MovieManager.h"
#include "BookingManager.h"

class UIManager {
private:
    MovieManager movieManager;
    Movie movies[MAX_MOVIES];
    int movieCount;
    BookingManager bookingManager;

public:
    void run(); // Hàm chính để chạy vòng lặp chương trình

private:
    void displayMainMenu();
    void flowListAndBook();
    void searchUserAndAction();
    void postSearchActions();
};