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
    _lastWiegand=0;
    _sysTick=0;
    _bitCount=0;
    _wiegandType=0;
    _d0Pin = d0Pin;
    _d1Pin = d1Pin;

    _configs[0].bitLength = 26;
    _configs[0].idFieldStartBit = 9;
    _configs[0].idFieldLength = 16;
    _configs[0].numUserFields = 1;
    _configs[0].numParityBits = 2;
    _configs[0].parityBits[0].bitPosition = 0;
    _configs[0].parityBits[0].mask = BitField(26);
    _configs[0].parityBits[1].evenParity = false;
    for (int i = 0; i < 26; i++){
        _configs[0].parityBits[0].mask.set(i, false);
    }
    for (int i = 1; i < 13; i++){
        _configs[0].parityBits[0].mask.set(i, true);
    }

    _configs[0].parityBits[1].evenParity = true;
    _configs[0].parityBits[1].mask = BitField(26);
    _configs[0].parityBits[1].bitPosition = 25;
    for (int i = 0; i < 26; i++) {
        _configs[0].parityBits[1].mask.set(i, false);
    }
    for (int i = 13; i < 25; i++) {
        _configs[0].parityBits[1].mask.set(i, true);
    }
    _configs[0].userFields[0].startBit = 1;
    _configs[0].userFields[0].bitLength = 8;
    _configs[0].userFields[0].successId = 1;
    _configs[0].userFields[0].failureId = 54;


}

WIEGAND::~WIEGAND() {
}

WiegandReturn WIEGAND::getCode()
{
	return _returnCode;
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
    _tempBuffer.set(_bitCount++, highBit);
    _lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}


BitField WIEGAND::getWiegandCode ()
{
	return BitField(_tempBuffer);
}


bool WIEGAND::DoWiegandConversion ()
{
	_sysTick=millis();
	if ((_sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		if (_bitCount > 0)
		{
		    Serial.println("here ");
		    Serial.println(_bitCount);

		    bool valid = false;
		    WiegandReturn returnCode;
		    _code = getWiegandCode();
		    for (int i = 0; i < 1; ++i) { //TODO: work for all
		        if (_bitCount != _configs[i].bitLength) {
		            continue;
		        }
		        Serial.println("here 2");

                bool valid_parity = true;
                for (int j = 0; j < _configs[i].numParityBits; j++) {
                    int val = 0;
                    for (int k = 0; k < _bitCount; k++) {
                        if (_configs[i].parityBits[j].mask[k]) {
                            val ^= _code[k];
                            Serial.print(_code[k]);
                        }
                    }

                    if (_configs[i].parityBits[j].evenParity) {
                       val ^= 1;
                    }

                    if (_code[_configs[i].parityBits[j].bitPosition] !=
                        (bool) val) {
                        Serial.print(" Parity Bad ");
                        Serial.println(j + 1);
                        valid_parity = false;
                        continue;
                    }
                    Serial.print(" Parity Ok ");
                    Serial.println(j + 1);
                }
                if (!valid_parity) {
                    continue;
                }
                valid = valid_parity;
                // User Fields
                returnCode.numberOfUserFields = _configs[i].numUserFields;
                for (int j = 0; j < _configs[i].numUserFields; j++) {
                    BitField userField; // TODO: make platform independent
                    int startBit = _configs[i].userFields[j].startBit;
                    int length = _configs[i].userFields[j].bitLength;
                    for (int k = startBit; k < startBit + length; ++k) {
                        userField <<= 1;
                        userField |= (bool)_code[k];
                    }
                    Serial.print("FACILITY CODE ");
                    //Serial.println(userField.to_ulong());
                    if (userField == _configs[i].userFields[j].successId) {
                        returnCode.successes[j] = true;
                    } else if (userField == _configs[i].userFields[j].failureId) {
                        returnCode.successes[j] = false;
                    } else {
                        valid = false;
                        Serial.println("INVALID FACILITY CODE");
                        break;
                    }
                }

                // Identifier
                int id = 0;
                for (int j = _configs[i].idFieldStartBit; j < _configs[i].idFieldStartBit + _configs[i].idFieldLength; ++j) {
                    id <<= 1;
                    id |= _code[j];
                }
                returnCode.id = id;
                _returnCode = returnCode;

                if (valid) {
                    break;
                }
		    }
            _bitCount=0;
            return valid;
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand=_sysTick;
            _bitCount=0;
			return false;
		}	
	}
	else
		return false;
}