#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "node.h"
#include <iostream>

template <typename T>
struct LinkedList {
    Node<T>* head = nullptr;
    int listSize = 0;

    // Lấy kích thước danh sách
    int size() {
        return listSize;
    }
    
    // Kiểm tra danh sách có rỗng không
    bool isEmpty() {
        return listSize == 0;
    }

    // Thêm một phần tử vào cuối danh sách
    void add(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (!head) {
            head = newNode;
        } else {
            Node<T>* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        listSize++;
    }

    // Lấy con trỏ đến Node tại một chỉ số index
    Node<T>* getNode(int index) {
        if (index < 0 || index >= listSize) {
            return nullptr;
        }
        Node<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current;
    }

    // Xóa một phần tử tại chỉ số index
    void removeAt(int index) {
        if (index < 0 || index >= listSize) {
            return;
        }

        Node<T>* toDelete;
        if (index == 0) {
            toDelete = head;
            head = head->next;
        } else {
            Node<T>* prev = getNode(index - 1);
            if (prev && prev->next) {
                toDelete = prev->next;
                prev->next = toDelete->next;
            } else {
                return;
            }
        }
        delete toDelete;
        listSize--;
    }
    
    // Xóa toàn bộ danh sách để giải phóng bộ nhớ
    void clear() {
        while (head) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        }
        listSize = 0;
    }
    
    // Destructor để tự động giải phóng bộ nhớ
    ~LinkedList() {
        clear();
    }

    // =================================================================
    // >>> PHẦN SỬA LỖI QUAN TRỌNG ĐƯỢC THÊM VÀO <<<
    // Copy Constructor: Đảm bảo khi copy một LinkedList, nó tạo ra một bản sao sâu (deep copy)
    LinkedList(const LinkedList<T>& other) : head(nullptr), listSize(0) {
        Node<T>* current = other.head;
        while (current != nullptr) {
            add(current->data); // Tái sử dụng hàm add để tạo node mới
            current = current->next;
        }
    }

    // Assignment Operator: Đảm bảo gán giá trị an toàn
    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) { // Chống tự gán
            clear(); // Xóa dữ liệu cũ
            Node<T>* current = other.head;
            while (current != nullptr) {
                add(current->data);
                current = current->next;
            }
        }
        return *this;
    }
    // =================================================================

    // Constructor mặc định (cần thiết khi đã định nghĩa constructor khác)
    LinkedList() = default;
};

#endif // LINKEDLIST_H