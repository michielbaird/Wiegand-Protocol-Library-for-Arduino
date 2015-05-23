#include "Wiegand.h"

WIEGAND::WIEGAND()
{
   init(D0Pin, D1Pin);
}

WIEGAND::WIEGAND(unsigned int d0Pin, unsigned int d1Pin)
{
   init(d0Pin, d1Pin);
}

void WIEGAND::init(unsigned int d0Pin, unsigned int d1Pin)
{
    _tempBuffer = new int[BUFFER_SIZE];
    _tempIndexHigh = 0;
    _tempIndexLow = 0;
    _lastWiegand=0;
    _sysTick=0;
    _bitCount=0;
    _wiegandType=0;
    _d0Pin = d0Pin;
    _d1Pin = d1Pin;
}

WIEGAND::~WIEGAND() {
    delete _tempBuffer;
}

WiegandCode WIEGAND::getCode()
{
	return _code;
}

int WIEGAND::getWiegandType()
{
	return _wiegandType;
}

bool WIEGAND::available()
{
	return DoWiegandConversion();
}

void WIEGAND::begin()
{
	_lastWiegand = 0;
    _tempIndexHigh = 0;
    _tempIndexLow = 0;
	_wiegandType = 0;
	_bitCount = 0;  
	_sysTick=millis();
	pinMode(_d0Pin, INPUT);					// Set D0 pin as input
	pinMode(_d1Pin, INPUT);					// Set D1 pin as input
}

void WIEGAND::ReadD0 ()
{
    readBit(false);
}

void WIEGAND::ReadD1()
{
    readBit(true);
}

void WIEGAND::readBit(bool highBit)
{
    _bitCount++;
    _tempBuffer[_tempIndexHigh] &= !(1 << _tempIndexLow);
    _tempBuffer[_tempIndexHigh] |= highBit << _tempIndexLow;
    _tempIndexLow++;
    if (_tempIndexLow == INT_SIZE)
    {
        _tempIndexLow = 0;
        _tempIndexHigh++;
    }

    _lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}


WiegandCode WIEGAND::getWiegandCode ()
{
	// Bits were read in reverse: So they need to switch back

	WiegandCode code;
	code.bitCount = _bitCount;
	int bufferIndexH = 0;
	int bufferIndexL = 0;
	for (int hIndex = _tempIndexHigh; hIndex <= 0; hIndex--)
	{
	    for (int lIndex= _tempIndexLow; lIndex <= 0; lIndex--)
	    {
	        code.buffer[bufferIndexH] |= ((_tempBuffer[hIndex] >> lIndex) & 1) << bufferIndexL;
	        bufferIndexL++;
	        if (bufferIndexL == INT_SIZE)
	        {
	            bufferIndexL = 0;
	            bufferIndexH++;
	        }
	    }
	    _tempIndexLow = INT_SIZE - 1;
	}
    return code;
}

bool WIEGAND::DoWiegandConversion ()
{
	unsigned long cardID;
	
	_sysTick=millis();
	if ((_sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		if (_bitCount > 0)
		{
		    _code = getWiegandCode();
            _bitCount=0;
            _tempIndexHigh = 0;
            _tempIndexLow = 0;
            return true;
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand=_sysTick;
            _bitCount=0;
            _tempIndexHigh = 0;
            _tempIndexLow = 0;
			return false;
		}	
	}
	else
		return false;
}