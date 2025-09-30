#pragma once
#include "models.h"

class BookingManager {
public:
    void displaySeats(const Show& sh);
    int chooseShowForMovie(Movie &m);
    bool isFull(const Show& sh);
    void bookMultipleSeats(Show& sh);
    void cancelSeat(Show& sh);
    void postSearchActions(Movie movies[], int movieCount);
};