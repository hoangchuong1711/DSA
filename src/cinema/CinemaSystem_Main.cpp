#include "CinemaSystem.h"
#include <iostream>

CinemaSystem::CinemaSystem() {
    initializeMovies();
}

void CinemaSystem::run() {
    bool running = true;
    while (running) {
        displayMainMenu();
        char choice;
        std::cin >> choice;
        std::cin.ignore(10000, '\n');

        switch (choice) {
            case '1': processMovieSelection(); break;
            case '2': processCustomerSearch(); break;
            case '0': running = false; break;
            default: std::cout << "Lua chon khong hop le.\n"; break;
        }
    }
}
