#include "../include/CDB.h"

void CDB::update() {
    entries = next_entries;
    next_entries = CDBEntry();
}

void CDB::clear() {
    entries = CDBEntry();
    next_entries = CDBEntry();
}
