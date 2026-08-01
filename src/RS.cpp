#include "../include/RS.h"

void RS::update() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = next_entries[i];
    }
}

bool RS::is_full() const {
    for (int i = 0; i < 32; ++i) {
        if (!entries[i].busy) {
            return false;
        }
    }
    return true;
}

int RS::allocate() const {
    for (int i = 0; i < 32; ++i) {
        if (!entries[i].busy) {
            return i;
        }
    }
    return 0;
}

void RS::clear() {

}
