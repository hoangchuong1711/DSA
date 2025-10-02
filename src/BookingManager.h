#pragma once
#include "models.h"
#include "IndexManager.h"
#include "Stack.h"

class BookingManager {
private:
    BookingStack bookingHistory;

public:
    void displaySeats(const Show& sh);
    int chooseShowForMovie(Movie &m);
    bool isFull(const Show& sh);
    void bookMultipleSeats(Show& sh, int movieIndex = -1, int showIndex = -1);
    void cancelSeat(Show& sh);
    void postSearchActions(Movie movies[], int movieCount);
    
    // Stack operations for undo functionality
    bool hasUndoHistory() const { return !bookingHistory.isEmpty(); }
    void recordBookingAction(int mi, int si, int ri, int ci, const User& user, bool wasBooked);
    bool undoLastAction(Movie movies[], int movieCount);
};