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
        // Suất chiếu hôm nay
        time_t todayShowtimes[] = {
            createTodayShowtime(9, 0),
            createTodayShowtime(14, 30),
            createTodayShowtime(18, 0),
            createTodayShowtime(21, 15)
        };

        // Suất chiếu ngày mai
        time_t tomorrowShowtimes[4];
        for (int i = 0; i < 4; ++i) {
            tm localTime;
            localtime_s(&localTime, &todayShowtimes[i]);
            localTime.tm_mday += 1; // sang ngày mai
            tomorrowShowtimes[i] = mktime(&localTime);
        }

        Movie movies[] = {
            Movie("INCEPTION", 8.8, 148, "Hanh dong/Khoa hoc", 13,
                "Dom Cobb la mot ke trom giac mo chuyen nghiep. Anh nhan nhiem vu xuyen qua nhung giac mo de cat xau thong tin va doi mat voi nhung nguy hiem khong lo.", "Christopher Nolan"),
            
            Movie("AVATAR: THE WAY OF WATER", 8.2, 192, "Khoa hoc/Phieu luu", 13,
                "Jake Sully va Neytiri cung nhung con cua ho phai doi mat voi nguy co moi khi ben canh dai duong xanh cua Pandora, tranh danh cho sinh ton va tinh yeu.", "James Cameron"),
            
            Movie("THE DARK KNIGHT", 9.0, 152, "Hanh dong/Chieu phap", 13,
                "Batman doi dau voi Joker, mot ke toi pham tinh nghiem trong voi nhung ke hoach phuc tap de gieo rac ham hai o Gotham.", "Christopher Nolan"),
            
            Movie("SPIDER-MAN: NO WAY HOME", 8.1, 148, "Phieu luu/Hanh dong", 13,
                "Peter Parker phai doi mat voi nhung ke thua nguoi tu cac vung dong van da bi xuyen khong gian, khi bi cong chung biet danh tinh.", "Jon Watts"),
            
            Movie("JOJO RABBIT", 7.9, 108, "Chien tranh/Hai huoc", 13,
                "Mot cau be nguoi Duc thich Hitler phai doi mat voi nhung thuc te cua chien tranh khi phat hien me nuoi mot co gai Do Tai nha.", "Taika Waititi"),
            
            Movie("THE LORD OF THE RINGS: THE FELLOWSHIP OF THE RING", 8.8, 178, "Phieu luu/Phim co trang", 13,
                "Frodo va nhung nguoi ban dong hanh phai tieu diet nhan vat toi ac Sauron va mang nhan nhan vat ring ve Mordor truoc khi bi ke xau so huu.", "Peter Jackson")
        };

        int numMovies = sizeof(movies) / sizeof(movies[0]);
        int numShowtimes = sizeof(todayShowtimes) / sizeof(todayShowtimes[0]);

        for (int i = 0; i < numMovies; ++i) {
            Movie m = movies[i]; // copy tất cả trường
            m.showtimeCount = numShowtimes * 2; // hôm nay + ngày mai
            // Gán hôm nay
            for (int j = 0; j < numShowtimes; ++j)
                m.showtimes[j].time = todayShowtimes[j];
            // Gán ngày mai
            for (int j = 0; j < numShowtimes; ++j)
                m.showtimes[numShowtimes + j].time = tomorrowShowtimes[j];

            movieList.add(m);
        }
    }