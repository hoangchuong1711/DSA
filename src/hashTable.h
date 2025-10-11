#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <optional>
#include "utils.h"
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
    int totalItems = 0;

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
        totalItems++;
    }

    bool isEmpty() const {
        return totalItems == 0;
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
    //tim kiem theo ten
    int findByName(const std::string& nameQuery, Customer* foundCustomers[], int maxMatches = 50) const {
        std::string nameQueryNorm = toLower(nameQuery);
        int foundCount = 0;

        // Duyệt toàn bộ bảng băm
        for (int i = 0; i < TABLE_SIZE; ++i) {
            Node<HashItem>* currentItemNode = table[i].head;

            // Duyệt danh sách trong mỗi bucket
            while (currentItemNode != nullptr) {
                // Lấy tên khách hàng trong nút hiện tại
                std::string currentNameNorm = toLower(currentItemNode->data.customerData->name);

                // So sánh (không phân biệt hoa thường)
                if (currentNameNorm == nameQueryNorm) {
                    if (foundCount < maxMatches) {
                        foundCustomers[foundCount++] = currentItemNode->data.customerData;
                    } else {
                        return foundCount; // đủ giới hạn thì dừng luôn
                    }
                }
                currentItemNode = currentItemNode->next;
            }
        }

        return foundCount; // trả về số lượng khách hàng tìm được
    }
};

#endif // HASHTABLE_H
