#include "bitfield.h"

BitField::BitField(int size) {
    buffer = 0;
}

BitField::BitField() {
    buffer = 0;
}

BitField& BitField::operator<<= (int pos) {
    buffer <<= 1;
    return *this;
}

BitField& BitField::operator|= (const BitField& rhs){
    buffer |= rhs.buffer;
    return *this;
}

BitField& BitField::set (int pos, bool val) {
    buffer &= ~(1 << pos);
    buffer |= (((int)val) << pos);
    return *this;
}

bool BitField::operator[] (int pos) const {
    return ((bool) ((buffer >> pos) & 1));
}

bool BitField::operator== (const BitField& rhs) const {
    return buffer == rhs.buffer;
}

int BitField::temp() {
    return buffer;
}