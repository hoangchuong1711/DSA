// main.cpp
#include <iostream>
#include "CinemaSystem.h" // Include lớp hệ thống rạp chiếu phim
// #include "CinemaSystem.cpp"
// #include "models.h"
// #include "utils.h"
// #include "utils.cpp"

int main() {
    CinemaSystem cs;
    while (true) {
        cs.displayMainMenu();
        int choice;
        std::cin >> choice;
        if (choice == 0) {
            std::cout << "Thoat chuong trinh.\n";
            break;
        } else if (choice == 1) {
            cs.flowListAndBook();
        } else if (choice == 2) {
            cs.searchUserAndAction();
        } else {
            std::cout << "Lua chon khong hop le. Thu lai.\n";
        }
    }
    return 0;
}