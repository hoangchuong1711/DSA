#ifndef NODE_H
#define NODE_H

// Sử dụng template để Node có thể lưu trữ bất kỳ kiểu dữ liệu nào.
// Điều này giúp tái sử dụng code cho các danh sách liên kết khác nhau (danh sách ghế, danh sách booking,...).
template <typename T>
struct Node {
    T data;
    Node<T>* next;

    // Constructor để khởi tạo Node với dữ liệu và con trỏ next
    Node(T data, Node<T>* next = nullptr) : data(data), next(next) {}
};

#endif // NODE_H
