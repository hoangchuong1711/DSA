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

// Kiểm tra tên chỉ chứa chữ cái và khoảng trắng
bool isValidName(const std::string &name);

// Kiểm tra CCCD chỉ chứa chữ số, độ dài >= 6 (có thể điều chỉnh)
bool isValidCCCD(const std::string &cccd);

// Đếm số ghế trống trong một suất chiếu
int countFreeSeats(const Show &sh);

// Lấy thời gian hiện tại (giờ:phút) theo local time, trả về phút tính từ 00:00
int getCurrentMinutesLocal();