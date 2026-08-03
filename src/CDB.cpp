#include "../include/CDB.h"

void CDB::update() {
    entries = next_entries;
    next_entries = CDBEntry();
}

void CDB::clear() {
    entries = CDBEntry();
    next_entries = CDBEntry();
}

void CDB::broadcast(int rob_id, uint32_t value, uint32_t target_pc) {
    next_entries.active = true;
    next_entries.rob = rob_id;
    next_entries.value = value;
    next_entries.target_pc = target_pc;
}

bool CDB::is_active() const {
    return entries.active;
}

int CDB::get_rob() const {
    return entries.rob;
}

uint32_t CDB::get_value() const {
    return entries.value;
}

uint32_t CDB::get_target_pc() const {
    return entries.target_pc;
}

bool CDB::is_next_free() const {
    return !next_entries.active;
}




