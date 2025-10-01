#pragma once
#include "MovieManager.h"
#include "BookingManager.h"
#include <string>

class UIManager {
private:
    MovieManager movieManager;
    BookingManager bookingManager;
    // Context from last successful search result
    std::string contextName;
    std::string contextCCCD;

public:
    void run(); // Hàm chính để chạy vòng lặp chương trình

private:
    void displayMainMenu();
    void flowListAndBook();
    void searchUserAndAction();
    void postSearchActions();
};