#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <optional>
#include "linkedList.h"
#include "models.h"

// Item trong bảng băm, lưu key (CCCD) và con trỏ đến dữ liệu khách hàng
struct HashItem {
    std::string key;
    Customer* customerData;

    HashItem() : customerData(nullptr) {}
    HashItem(std::string key, Customer* data) : key(key), customerData(data) {}
};

struct HashTable {
    static const int TABLE_SIZE = 1009; 
    LinkedList<HashItem> table[TABLE_SIZE];

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
        // Dùng hàm add O(1) mới của LinkedList
        table[index].add(HashItem(key, customer)); 
    }

    // Tìm kiếm khách hàng bằng key (CCCD)
    // std::optional giúp trả về giá trị một cách an toàn (có thể có hoặc không)
    std::optional<Customer*> get(std::string key) {
        unsigned int index = hash(key);
        // Phải duyệt LinkedList trong bucket đó
        Node<HashItem>* current = table[index].head;
        while (current) {
            if (current->data.key == key) {
                return current->data.customerData;
            }
            current = current->next;
        }
        return std::nullopt;
    }

    // Kiểm tra xem một key đã tồn tại chưa
    bool has(std::string key) {
        return get(key).has_value();
    }
};

#endif // HASHTABLE_H
