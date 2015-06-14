#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <math.h>
#include <bitset>

#define D0Pin 2			// Arduino Pin 2 Hardware interrupt
#define D1Pin 3			// Arduino Pin 3 Hardware interrupt
#define MAX_BIT_LENGTH 256

const int INT_SIZE = sizeof(int)*8;
const int BUFFER_SIZE = MAX_BIT_LENGTH / INT_SIZE;
#define BitField std::bitset<256>


struct WiegandUserField {
    char fieldName[16];
    int startBit;
    int bitLength;
    BitField successId;
    BitField failureId;
};

class ParityConfig {
  public:
    int bitPosition;
    bool evenParity;
    BitField mask;
};

struct WiegandReturn {
    BitField id;
    int numberOfUserFields;
    bool successes[4];
};

struct WiegandConfig {
    char name[32];
    int bitLength;
    int idFieldStartBit;
    int idFieldLength;
    BitField heartBeatId;
    int numUserFields;
    WiegandUserField userFields[4];
    int numParityBits;
    ParityConfig parityBits[4];
};

class WIEGAND {

public:
	WIEGAND();
	~WIEGAND();
	WIEGAND(unsigned int d0Pin, unsigned int d1Pin);
	void begin();
	bool available();
	WiegandReturn getCode();
	int getWiegandType();
    void ReadD0();
    void ReadD1();
    WiegandConfig   _configs[8];

private:
    void init(unsigned int d0Pin, unsigned int d1Pin);
	void readBit(bool highBit);

	bool DoWiegandConversion ();
	BitField getWiegandCode();

	unsigned int     _d0Pin;
    unsigned int     _d1Pin;
    int              numConfigs;
    BitField         _tempBuffer;
	unsigned long 	 _lastWiegand;
	unsigned long 	 _sysTick;
	int				 _bitCount;
	int				 _wiegandType;
	BitField         _code;
	WiegandReturn    _returnCode;
};

#endif
