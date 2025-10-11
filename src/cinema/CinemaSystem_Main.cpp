#include "CinemaSystem.h"
#include <iostream>
using namespace std;

CinemaSystem::CinemaSystem() {
    initializeMovies();
}

void CinemaSystem::run() {
    bool running = true;
    while (running) {
        displayMainMenu();
        string choice;
        cin >> choice;
        cin.ignore(10000, '\n');

        if(choice=="1"){
            processMovieSelection(); 
        }
        else if(choice=="2"){
            processCustomerSearch(); 
        }
        else if(choice=="3"){
            running = false; break;
        }
        else if(choice.empty()){
            cout << "Vui long nhap it nhat 1 ki tu, nhan Enter de nhap lai"; 
            cin.ignore();
            continue;
        }
        else{
            cout << "Lua chon khong hop le, vui long nhan Enter de nhap lai"; 
            cin.ignore();
            continue;
        }
    }
}
