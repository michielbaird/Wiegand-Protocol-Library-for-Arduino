#include "bitfield.h"

BitField::BitField(int size) {
    for (int i = 0; i < _BUCKETS; ++i) {
        buffer[i] = 0;
    }
}

BitField::BitField() {
    for (int i = 0; i < _BUCKETS; ++i) {
        buffer[i] = 0;
    }
}

void BitField::shift1() {
    bool carry = (bool) ((buffer[0] >> _HIGH_SHIFT) & 1);
    buffer[0] <<= 1;
    for (int i = 1; i < _BUCKETS; ++i) {
        bool new_carry = (bool) ((buffer[i] >> _HIGH_SHIFT) & 1);
        buffer[i] <<= 1;
        buffer[i] |= carry ? 1 : 0;
        carry = new_carry;
    }
}

BitField& BitField::operator<<= (int pos) {
    for (int i = 0; i < pos; ++i) {
        shift1();
    }
    return *this;
}

BitField& BitField::operator|= (const BitField& rhs) {
    for (int i = 0; i < _BUCKETS; ++i) {
        buffer[i] |= rhs.buffer[i];
    }
    return *this;
}

BitField& BitField::set (int pos, bool val) {
    int h = pos >> _DIV_SHIFT;
    int l = pos & _MOD_AND;
    buffer[h] &= ~(1ll << l);
    buffer[h] |= (((unsigned long long)val) << l);
    return *this;
}

bool BitField::operator[] (int pos) const {
    int h = pos >> _DIV_SHIFT;
    int l = pos & _MOD_AND;
    return ((bool) (((unsigned long long) buffer[h] >> l) & 1));
}

bool BitField::operator== (const BitField& rhs) const {
    for (int i = 0; i < _BUCKETS; ++i) {
        if (buffer[i] != rhs.buffer[i]) {
            return false;
        }
    }
    return true;
}

unsigned long long BitField::temp(int p) {
    return buffer[p];
}