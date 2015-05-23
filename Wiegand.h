#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define D0Pin 2			// Arduino Pin 2 Hardware interrupt
#define D1Pin 3			// Arduino Pin 3 Hardware interrupt
#define MAX_BIT_LENGTH 256

const int INT_SIZE = sizeof(int)*8;
const int BUFFER_SIZE = MAX_BIT_LENGTH / INT_SIZE;

struct WiegandCode
{
    int bitCount;
    int* buffer;
    WiegandCode()
    {
        bitCount = 0;
        buffer = new int[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
    }

    WiegandCode(const WiegandCode& code)
    {
        bitCount = 0;
        buffer = new int[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        for (int index; index < BUFFER_SIZE; ++index)
        {
            buffer[index] = code.buffer[index];
        }
    }

    ~WiegandCode()
    {
        delete buffer;
    }
};

class WIEGAND {

public:
	WIEGAND();
	~WIEGAND();
	WIEGAND(unsigned int d0Pin, unsigned int d1Pin);
	void begin();
	bool available();
	WiegandCode getCode();
	int getWiegandType();
    void ReadD0();
    void ReadD1();

private:
    void init(unsigned int d0Pin, unsigned int d1Pin);
	void readBit(bool highBit);

	bool DoWiegandConversion ();
	WiegandCode getWiegandCode();

	unsigned int    _d0Pin;
    unsigned int    _d1Pin;
    int*            _tempBuffer;
    int             _tempIndexHigh;
    int             _tempIndexLow;
	unsigned long 	_lastWiegand;
	unsigned long 	_sysTick;
	int				_bitCount;
	int				_wiegandType;
	WiegandCode     _code;
};

#endif
