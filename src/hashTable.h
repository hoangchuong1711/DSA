#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <optional>
#include "models.h" // Cần struct Customer

// optional<> yêu cầu forward declaration hoặc include đầy đủ
// Vì Customer là một kiểu hoàn chỉnh, ta include models.h
// Forward declaration cho Customer nếu Item chỉ chứa con trỏ Customer*
// struct Customer; 

// Item trong bảng băm, lưu key (CCCD) và con trỏ đến dữ liệu khách hàng
struct HashItem {
    std::string key;
    Customer* customerData;

    HashItem() : customerData(nullptr) {}
    HashItem(std::string key, Customer* data) : key(key), customerData(data) {}
};

// Mỗi bucket trong bảng băm là một danh sách (mảng tĩnh) các Item
// để xử lý xung đột (chaining with array)
struct ItemList {
    HashItem values[10]; // Giả sử mỗi bucket chứa tối đa 10 khách hàng có hash key trùng nhau
    int count = 0;

    void add(HashItem item) {
        if (count < 10) {
            values[count++] = item;
        }
    }
};

struct HashTable {
    static const int TABLE_SIZE = 13; // Chọn một số nguyên tố để tối ưu việc băm
    ItemList table[TABLE_SIZE];

    // Hàm băm đơn giản dựa trên mã ASCII của key (CCCD)
    unsigned int hash(std::string key) {
        unsigned int hashValue = 0;
        for (char ch : key) {
            hashValue = 37 * hashValue + ch;
        }
        return hashValue % TABLE_SIZE;
    }

    // Thêm một cặp key-value (CCCD - Customer) vào bảng băm
    void add(std::string key, Customer* customer) {
        unsigned int index = hash(key);
        table[index].add(HashItem(key, customer));
    }

    // Tìm kiếm khách hàng bằng key (CCCD)
    // std::optional giúp trả về giá trị một cách an toàn (có thể có hoặc không)
    std::optional<Customer*> get(std::string key) {
        unsigned int index = hash(key);
        for (int i = 0; i < table[index].count; ++i) {
            if (table[index].values[i].key == key) {
                return table[index].values[i].customerData;
            }
        }
        return {}; // Trả về empty optional nếu không tìm thấy
    }

    // Kiểm tra xem một key đã tồn tại chưa
    bool has(std::string key) {
        return get(key).has_value();
    }
};

#endif // HASHTABLE_H
