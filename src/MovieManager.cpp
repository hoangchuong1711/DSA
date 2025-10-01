#include "MovieManager.h"
#include <iostream>
#include <iomanip>
#include "utils.h"

using namespace std;

MovieManager::MovieManager() {
    movieCount = 0;
    initMovies();
}

void MovieManager::initMovies() {
    // Extended sample data with multiple shows across the day
    movies[0].title = "Avengers";
    movies[0].rating = 4.6f;
    movies[0].showCount = 4;
    movies[0].shows[0] = Show("08:00");
    movies[0].shows[1] = Show("10:30");
    movies[0].shows[2] = Show("14:00");
    movies[0].shows[3] = Show("23:45");

    movies[1].title = "Inception";
    movies[1].rating = 4.8f;
    movies[1].showCount = 3;
    movies[1].shows[0] = Show("09:15");
    movies[1].shows[1] = Show("13:45");
    movies[1].shows[2] = Show("11:30");

    movies[2].title = "Interstellar";
    movies[2].rating = 4.7f;
    movies[2].showCount = 3;
    movies[2].shows[0] = Show("11:00");
    movies[2].shows[1] = Show("15:30");
    movies[2].shows[2] = Show("21:00");

    movies[3].title = "Joker";
    movies[3].rating = 4.3f;
    movies[3].showCount = 2;
    movies[3].shows[0] = Show("12:00");
    movies[3].shows[1] = Show("19:00");

    movies[4].title = "The Matrix";
    movies[4].rating = 4.5f;
    movies[4].showCount = 2;
    movies[4].shows[0] = Show("16:45");
    movies[4].shows[1] = Show("22:15");

    movieCount = 5;
}

void MovieManager::listMoviesDefault() {
    quickSortMovies(movies, 0, movieCount-1, 0); // sort by earliest show time asc
    cout << "\n===== DANH SACH PHIM (theo gio chieu) =====\n";
    cout << "Goi y: nhap '?' de tim phim theo ten, '/' de sap xep.\n";
    for (int i=0;i<movieCount;i++) {
        bool hasFuture = false;
        int earliestIdx = -1;
        int earliest = 24*60 + 1;
        for (int s=0;s<movies[i].showCount;s++) {
            int mm = timeToMinutes(movies[i].shows[s].time);
            int now = 0; // placeholder, UI filters by time; here we just show all; detailed filter in UI
            if (mm >= now) { hasFuture = true; if (mm < earliest) { earliest = mm; earliestIdx = s; } }
        }
        if (!hasFuture) continue;
        cout << i+1 << ". " << movies[i].title
            << "  | Rating: " << fixed << setprecision(1) << movies[i].rating
            << "  | Earliest: " << (earliestIdx>=0 ? movies[i].shows[earliestIdx].time : "N/A")
            << "  | Free seats total: " << totalFreeSeats(movies[i])
            << "\n";
    }
}

void MovieManager::listMoviesSortedMenu() {
    cout << "\nSap xep phim theo:\n";
    cout << "1. Gio chieu (tang dan)\n";
    cout << "2. So ghe trong (giam dan)\n";
    cout << "3. Danh gia (rating, giam dan)\n";
    cout << "Chon: ";
}

void MovieManager::listMoviesWithMode(int mode) {
    quickSortMovies(movies, 0, movieCount-1, mode);
    cout << "\n===== DANH SACH PHIM =====\n";
    for (int i=0;i<movieCount;i++) {
        cout << i+1 << ". " << movies[i].title
             << "  | Rating: " << fixed << setprecision(1) << movies[i].rating
             << "  | Earliest: " << (movies[i].showCount>0 ? movies[i].shows[0].time : "N/A")
             << "  | Free seats total: " << totalFreeSeats(movies[i])
             << "\n";
    }
}

Movie* MovieManager::chooseMovieByIndex(int idx) {
    if (idx < 1 || idx > movieCount) return nullptr;
    return &movies[idx-1];
}

void swapMovie(Movie &a, Movie &b) {
    Movie tmp = a;
    a = b;
    b = tmp;
}

void quickSortMovies(Movie movies[], int l, int r, int mode) {
    if (l >= r) return;
    int i = l, j = r;
    Movie pivot = movies[(l + r) / 2];

    auto cmpLeft = [&](const Movie &x, const Movie &y)->bool {
        if (mode == 0) return earliestShowMinutes(x) < earliestShowMinutes(y);
        if (mode == 1) return totalFreeSeats(x) > totalFreeSeats(y);
        if (mode == 2) return x.rating > y.rating;
        return earliestShowMinutes(x) < earliestShowMinutes(y);
    };

    while (i <= j) {
        while (cmpLeft(movies[i], pivot)) i++;
        while (cmpLeft(pivot, movies[j])) j--;
        if (i <= j) {
            swapMovie(movies[i], movies[j]);
            i++; j--;
        }
    }
    if (l < j) quickSortMovies(movies, l, j, mode);
    if (i < r) quickSortMovies(movies, i, r, mode);
}