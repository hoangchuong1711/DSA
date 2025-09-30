// utils.cpp
#include "utils.h"
#include <cctype>   // Cho isdigit, toupper
#include <cstring>  // Cho isspace

// Triển khai các hàm đã được khai báo trong utils.h

int timeToMinutes(const std::string &t) {
    if (t.size() < 4) return 24*60;
    int hh = 0, mm = 0;
    size_t colon = t.find(':');
    if (colon != std::string::npos) {
        hh = stoi(t.substr(0, colon));
        mm = stoi(t.substr(colon+1));
    } else {
        hh = stoi(t.substr(0,2));
        mm = stoi(t.substr(2));
    }
    return hh*60 + mm;
}

int totalFreeSeats(const Movie &m) {
    int cnt = 0;
    for (int s=0;s<m.showCount;s++) {
        for (int i=0;i<m.shows[s].rows;i++)
            for (int j=0;j<m.shows[s].cols;j++)
                if (!m.shows[s].seats[i][j].booked) cnt++;
    }
    return cnt;
}

int earliestShowMinutes(const Movie &m) {
    int earliest = 24*60 + 1;
    for (int s=0;s<m.showCount;s++) {
        int mm = timeToMinutes(m.shows[s].time);
        if (mm < earliest) earliest = mm;
    }
    return earliest;
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

bool parseSeatCode(const std::string &id_orig, int &r, int &c, const Show &sh) {
    if (id_orig.size() < 2) return false;
    std::string id = id_orig;
    id[0] = toupper(id[0]);
    char rowChar = id[0];
    if (rowChar < 'A' || rowChar > 'Z') return false;
    r = rowChar - 'A';
    if (r < 0 || r >= sh.rows) return false;
    std::string num = id.substr(1);
    if (num.size() == 0) return false;
    for (size_t i=0;i<num.size();i++) if (!isdigit(num[i])) return false;
    int col = stoi(num) - 1;
    if (col < 0 || col >= sh.cols) return false;
    c = col;
    return true;
}

int splitTokens(const std::string &line, std::string tokens[], int maxTokens) {
    int cnt = 0;
    int n = (int)line.size();
    std::string cur = "";
    for (int i=0;i<n;i++) {
        char ch = line[i];
        if (isspace(ch)) {
            if (!cur.empty()) {
                if (cnt < maxTokens) tokens[cnt++] = cur;
                cur.clear();
            }
        } else {
            cur.push_back(ch);
        }
    }
    if (!cur.empty() && cnt < maxTokens) tokens[cnt++] = cur;
    return cnt;
}