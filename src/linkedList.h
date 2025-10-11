#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "node.h"
#include <iostream>

template <typename T>
struct LinkedList {
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;
    int listSize = 0;

    LinkedList() = default;

    int size() const {
        return listSize;
    }

    bool isEmpty() const {
        return listSize == 0;
    }

    void add(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        listSize++;
    }

    T* getData(int index) const {
        Node<T>* node = getNode(index);
        return node ? &node->data : nullptr;
    }

    Node<T>* getNode(int index) const {
        if (index < 0 || index >= listSize) return nullptr;

        Node<T>* current;
        if (index < listSize / 2) {
            current = head;
            for (int i = 0; i < index; i++)
                current = current->next;
        } else {
            current = tail;
            for (int i = listSize - 1; i > index; i--)
                current = current->prev;
        }
        return current;
    }

    void removeAt(int index) {
        if (index < 0 || index >= listSize) return;

        Node<T>* toDelete;
        if (index < listSize / 2) {
            toDelete = head;
            for (int i = 0; i < index; i++)
                toDelete = toDelete->next;
        } else {
            toDelete = tail;
            for (int i = listSize - 1; i > index; i--)
                toDelete = toDelete->prev;
        }

        if (toDelete->prev)
            toDelete->prev->next = toDelete->next;
        else
            head = toDelete->next;

        if (toDelete->next)
            toDelete->next->prev = toDelete->prev;
        else
            tail = toDelete->prev;

        delete toDelete;
        listSize--;
    }

    void clear() {
        Node<T>* current = head;
        while (current) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = tail = nullptr;
        listSize = 0;
    }

    bool contains(const T& v) const {
        Node<T>* cur = head;
        while (cur) {
            if (cur->data == v) return true;
            cur = cur->next;
        }
        return false;
    }

    void swap(Node<T>* a, Node<T>* b) {
        if (a && b)
            std::swap(a->data, b->data);
    }

    int findIndexByPointer(T* ptr) const {
        Node<T>* cur = head;
        int idx = 0;
        while (cur) {
            if (&cur->data == ptr) return idx;
            cur = cur->next;
            ++idx;
        }
        return -1;
    }

    T** listToPtrArray() {
        int n = size();
        if (n == 0) return nullptr;

        T** arr = new T*[n];
        Node<T>* cur = head;
        for (int i = 0; i < n; ++i) {
            arr[i] = &cur->data;
            cur = cur->next;
        }
        return arr;
    }

    void sortDesc() {
        if (!head || !head->next) return;
        for (Node<T>* i = head; i->next; i = i->next) {
            for (Node<T>* j = i->next; j; j = j->next) {
                if (i->data < j->data)
                    swap(i, j);
            }
        }
    }

    LinkedList(const LinkedList<T>& other) {
        Node<T>* current = other.head;
        while (current) {
            add(current->data);
            current = current->next;
        }
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            clear();
            Node<T>* current = other.head;
            while (current) {
                add(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    ~LinkedList() {
        clear();
    }
};

#endif // LINKEDLIST_H
