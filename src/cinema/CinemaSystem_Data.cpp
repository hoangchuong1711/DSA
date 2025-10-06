#include "CinemaSystem.h"
#include <iostream>

    time_t CinemaSystem::createTodayShowtime(int hour, int minute) {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);
        localTime.tm_hour = hour;
        localTime.tm_min = minute;
        localTime.tm_sec = 0;
        return mktime(&localTime);
    }

    void CinemaSystem::initializeMovies() {
        time_t showtimes[] = {
            createTodayShowtime(23, 59), createTodayShowtime(14, 30),
            createTodayShowtime(23, 30), createTodayShowtime(23, 15)
        };
        int numShowtimes = 4;
        const char* titles[] = {
            "DAO KIEM VUNG DAT QUY", "DORAEMON: NOBITA VA BAN GIAO HUONG",
            "LAT MAT 7: MOT DIEU UOC", "HANH TINH KHI: VUONG QUOC MOI", "TAROT"
        };
        double ratings[] = {7.6, 8.3, 6.8, 8.0, 5.9};
        for (int i = 0; i < 5; ++i) {
            movieList[i].title = titles[i];
            movieList[i].rating = ratings[i];
            for (int j = 0; j < numShowtimes; ++j) {
                movieList[i].showtimes[j].time = showtimes[j];
            }
            movieList[i].showtimeCount = numShowtimes;
        }
    }