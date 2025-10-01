#pragma once

#include "models.h"
#include <string>

// Position of a booked seat in the system
struct Position {
    int movieIndex;
    int showIndex;
    int rowIndex;
    int colIndex;
};

// Configuration for hash tables
const int MAX_INDEX_BUCKETS = 4096;          // number of buckets for open addressing
const int MAX_POSITIONS_PER_KEY = 64;        // max seats per same key (name or CCCD)

struct KeyEntry {
    std::string key;
    Position positions[MAX_POSITIONS_PER_KEY];
    int posCount;
    bool used;
    KeyEntry() : key(""), posCount(0), used(false) {}
};

class IndexManager {
private:
    KeyEntry nameTable[MAX_INDEX_BUCKETS];
    KeyEntry cccdTable[MAX_INDEX_BUCKETS];

    int findSlot(KeyEntry table[], const std::string &key) const;
    unsigned long hashString(const std::string &s) const;
    bool addToTable(KeyEntry table[], const std::string &key, const Position &p);
    bool removeFromTable(KeyEntry table[], const std::string &key, const Position &p);

public:
    void clear();
    void build(Movie movies[], int movieCount);

    bool add(const std::string &name, const std::string &cccd, const Position &p);
    bool remove(const std::string &name, const std::string &cccd, const Position &p);

    // Query functions. outCount returns number of results written (<= maxOut)
    void queryByName(const std::string &name, Position out[], int &outCount, int maxOut) const;
    void queryByCCCD(const std::string &cccd, Position out[], int &outCount, int maxOut) const;
};

// Global accessor (singleton-like)
IndexManager &getIndex();


