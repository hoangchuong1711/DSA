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
            createTodayShowtime(9, 0),
            createTodayShowtime(14, 30),
            createTodayShowtime(18, 0),
            createTodayShowtime(21, 15)
        };

        Movie movies[] = {
            {"DAO KIEM VUNG DAT QUY", 7.6},
            {"DORAEMON: NOBITA VA BAN GIAO HUONG", 8.3},
            {"LAT MAT 7: MOT DIEU UOC", 6.8},
            {"HANH TINH KHI: VUONG QUOC MOI", 8.0},
            {"TAROT", 5.9},
            {"TAM QUOC DIEN NGHIA", 8.7} // 👉 thêm phim chỉ cần thêm dòng này
        };

        int numMovies = sizeof(movies) / sizeof(movies[0]);
        int numShowtimes = sizeof(showtimes) / sizeof(showtimes[0]);

        for (int i = 0; i < numMovies; ++i) {
            Movie m;
            m.title = movies[i].title;
            m.rating = movies[i].rating;
            m.showtimeCount = numShowtimes;

            for (int j = 0; j < numShowtimes; ++j) {
                m.showtimes[j].time = showtimes[j];
            }

            movieList.add(m); // LinkedList<Movie> movieList;
        }
    }