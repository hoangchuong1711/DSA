#include "IndexManager.h"
#include <cctype>

static IndexManager g_index;

IndexManager &getIndex() { return g_index; }

unsigned long IndexManager::hashString(const std::string &s) const {
    // djb2
    unsigned long h = 5381UL;
    for (size_t i=0;i<s.size();i++) {
        h = ((h << 5) + h) + static_cast<unsigned char>(s[i]);
    }
    return h;
}

int IndexManager::findSlot(KeyEntry table[], const std::string &key) const {
    unsigned long h = hashString(key);
    int idx = (int)(h % MAX_INDEX_BUCKETS);
    int start = idx;
    while (true) {
        if (!table[idx].used || table[idx].key == key) return idx;
        idx = (idx + 1) % MAX_INDEX_BUCKETS;
        if (idx == start) return -1; // full
    }
}

bool IndexManager::addToTable(KeyEntry table[], const std::string &key, const Position &p) {
    int slot = findSlot(table, key);
    if (slot < 0) return false;
    if (!table[slot].used) {
        table[slot].used = true;
        table[slot].key = key;
        table[slot].posCount = 0;
    }
    if (table[slot].posCount >= MAX_POSITIONS_PER_KEY) return false;
    table[slot].positions[table[slot].posCount++] = p;
    return true;
}

bool IndexManager::removeFromTable(KeyEntry table[], const std::string &key, const Position &p) {
    int slot = findSlot(table, key);
    if (slot < 0) return false;
    if (!table[slot].used || table[slot].key != key) return false;
    int n = table[slot].posCount;
    for (int i=0;i<n;i++) {
        Position &q = table[slot].positions[i];
        if (q.movieIndex==p.movieIndex && q.showIndex==p.showIndex && q.rowIndex==p.rowIndex && q.colIndex==p.colIndex) {
            table[slot].positions[i] = table[slot].positions[n-1];
            table[slot].posCount--;
            return true;
        }
    }
    return false;
}

void IndexManager::clear() {
    for (int i=0;i<MAX_INDEX_BUCKETS;i++) {
        nameTable[i] = KeyEntry();
        cccdTable[i] = KeyEntry();
    }
}

void IndexManager::build(Movie movies[], int movieCount) {
    clear();
    for (int mi=0; mi<movieCount; mi++) {
        for (int si=0; si<movies[mi].showCount; si++) {
            Show &sh = movies[mi].shows[si];
            for (int r=0;r<sh.rows;r++)
                for (int c=0;c<sh.cols;c++)
                    if (sh.seats[r][c].booked) {
                        Position p{mi,si,r,c};
                        addToTable(nameTable, sh.seats[r][c].user.name, p);
                        addToTable(cccdTable, sh.seats[r][c].user.cccd, p);
                    }
        }
    }
}

bool IndexManager::add(const std::string &name, const std::string &cccd, const Position &p) {
    bool a = addToTable(nameTable, name, p);
    bool b = addToTable(cccdTable, cccd, p);
    return a && b;
}

bool IndexManager::remove(const std::string &name, const std::string &cccd, const Position &p) {
    bool a = removeFromTable(nameTable, name, p);
    bool b = removeFromTable(cccdTable, cccd, p);
    return a && b;
}

void IndexManager::queryByName(const std::string &name, Position out[], int &outCount, int maxOut) const {
    outCount = 0;
    int slot = findSlot((KeyEntry*)nameTable, name);
    if (slot < 0) return;
    if (!nameTable[slot].used || nameTable[slot].key != name) return;
    int n = nameTable[slot].posCount;
    for (int i=0;i<n && outCount<maxOut;i++) out[outCount++] = nameTable[slot].positions[i];
}

void IndexManager::queryByCCCD(const std::string &cccd, Position out[], int &outCount, int maxOut) const {
    outCount = 0;
    int slot = findSlot((KeyEntry*)cccdTable, cccd);
    if (slot < 0) return;
    if (!cccdTable[slot].used || cccdTable[slot].key != cccd) return;
    int n = cccdTable[slot].posCount;
    for (int i=0;i<n && outCount<maxOut;i++) out[outCount++] = cccdTable[slot].positions[i];
}


