#pragma once
#include "models.h"

// Stack node for booking history
struct BookingAction {
    int movieIndex;
    int showIndex;
    int rowIndex;
    int colIndex;
    User user;
    bool wasBooked; // true if this was a booking action, false if cancellation
    BookingAction() : movieIndex(-1), showIndex(-1), rowIndex(-1), colIndex(-1), wasBooked(false) {}
    BookingAction(int mi, int si, int ri, int ci, const User& u, bool booked) 
        : movieIndex(mi), showIndex(si), rowIndex(ri), colIndex(ci), user(u), wasBooked(booked) {}
};

const int MAX_STACK_SIZE = 100;

class BookingStack {
private:
    BookingAction stack[MAX_STACK_SIZE];
    int top;

public:
    BookingStack() : top(-1) {}
    
    bool push(const BookingAction& action) {
        if (top >= MAX_STACK_SIZE - 1) return false;
        stack[++top] = action;
        return true;
    }
    
    bool pop(BookingAction& action) {
        if (top < 0) return false;
        action = stack[top--];
        return true;
    }
    
    bool isEmpty() const {
        return top < 0;
    }
    
    bool isFull() const {
        return top >= MAX_STACK_SIZE - 1;
    }
    
    int size() const {
        return top + 1;
    }
};
