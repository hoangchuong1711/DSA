#ifndef NODE_H
#define NODE_H

template <typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;

    Node(T d) : data(d), next(nullptr), prev(nullptr) {}
};

#endif