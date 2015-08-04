#ifndef _BITFIELD_H
#define _BITFIELD_H

class BitField {
  public:
    BitField();
    BitField(int);
    BitField& operator<<= (int pos);
    BitField& operator|= (const BitField& rhs);
    BitField& set (int pos, bool val);
    bool operator[] (int pos) const;
    bool operator== (const BitField& rhs) const;
    int temp();
  private:
    unsigned int buffer;
};

#endif