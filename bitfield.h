#ifndef _BITFIELD_H
#define _BITFIELD_H

#define _BUCKETS 2 // 128 / sizeof(unsigned long long);
#define _HIGH_SHIFT 31// sizeof(unsigned long long) - 1;
#define _DIV_SHIFT 5 //sizeof (unsigned long long) - __builtin_clz (sizeof(unsigned long long));
#define _MOD_AND 31 // (_MOD_SHIFT << 1) - 1;

class BitField {
  public:
    BitField();
    BitField(int);
    BitField(const BitField& rhs);
    BitField& operator<<= (int pos);
    BitField& operator|= (const BitField& rhs);
    BitField& set (int pos, bool val);
    bool operator[] (int pos) const;
    bool operator== (const BitField& rhs) const;
    unsigned long long temp(int p);
  private:
    int size;
    void shift1();
    unsigned long long buffer[_BUCKETS];
};

#endif