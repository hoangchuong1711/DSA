// utils.h
#pragma once

#include "models.h" // Cần models.h để biết Movie là gì
#include <string>

// ===== Khai báo các hàm tiện ích =====

// Chuyển đổi thời gian "HH:MM" sang phút
int timeToMinutes(const std::string &t);

// Tính tổng số ghế trống của một phim
int totalFreeSeats(const Movie &m);

// Lấy thời gian suất chiếu sớm nhất của một phim
int earliestShowMinutes(const Movie &m);

// Sắp xếp danh sách phim bằng QuickSort
void quickSortMovies(Movie movies[], int l, int r, int mode);

// Phân tích mã ghế "A10" thành tọa độ hàng, cột
bool parseSeatCode(const std::string &id_orig, int &r, int &c, const Show &sh);

// Tách một chuỗi thành các token dựa trên dấu cách
int splitTokens(const std::string &line, std::string tokens[], int maxTokens);