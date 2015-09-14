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

    config.bitLength = 26;
    config.idFieldStartBit = 9;
    config.idFieldLength = 16;
    config.numUserFields = 1;
    config.numParityBits = 2;
    config.parityBits[0].bitPosition = 0;
    config.parityBits[0].mask = BitField(26);
    config.parityBits[1].evenParity = false;
    for (int i = 0; i < 26; i++){
        config.parityBits[0].mask.set(i, false);
    }
    for (int i = 1; i < 13; i++){
        config.parityBits[0].mask.set(i, true);
    }

    config.parityBits[1].evenParity = true;
    config.parityBits[1].mask = BitField(26);
    config.parityBits[1].bitPosition = 25;
    for (int i = 0; i < 26; i++) {
        config.parityBits[1].mask.set(i, false);
    }
    for (int i = 13; i < 25; i++) {
        config.parityBits[1].mask.set(i, true);
    }
    config.userFields[0].startBit = 1;
    config.userFields[0].bitLength = 8;
    config.userFields[0].successId = 1;
    config.userFields[0].failureId = 54;

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
		    //Serial.println("here ");
		    Serial.println(_bitCount);

		    bool valid = false;
		    WiegandReturn returnCode;
		    _code = getWiegandCode();
//		    for (int i = 0; i < 1; ++i) { //TODO: work for all
		        if (_bitCount != config.bitLength) {
		            continue;
		        }
		        //Serial.println("here 2");

                bool valid_parity = true;
                for (int j = 0; j < config.numParityBits; j++) {
                    int val = 0;
                    for (int k = 0; k < _bitCount; k++) {
                        if (config.parityBits[j].mask[k]) {
                            val ^= _code[k];
                            Serial.print(_code[k]);
                        }
                    }

                    if (config.parityBits[j].evenParity) {
                       val ^= 1;
                    }

                    if (_code[config.parityBits[j].bitPosition] !=
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
                returnCode.numberOfUserFields = config.numUserFields;
                for (int j = 0; j < config.numUserFields; j++) {
                    BitField userField(config.userFields[j].bitLength); // TODO: make platform independent
                    int startBit = config.userFields[j].startBit;
                    int length = config.userFields[j].bitLength;
                    for (int k = startBit; k < startBit + length; ++k) {
                        userField <<= 1;
                        userField |= (bool)_code[k];
                    }
                    Serial.print("FACILITY CODE ");
                    //Serial.println(userField.to_ulong());
                    if (userField == config.userFields[j].successId) {
                        returnCode.successes[j] = true;
                    } else if (userField == config.userFields[j].failureId) {
                        returnCode.successes[j] = false;
                    } else {
                        valid = false;
                        Serial.println("INVALID FACILITY CODE");
                        break;
                    }
                }

                // Identifier
                returnCode.id = 0;
                for (int j = config.idFieldStartBit; j < config.idFieldStartBit + config.idFieldLength; ++j) {
                    returnCode.id <<= 1;
                    returnCode.id.set(0, _code[j]);
                }
                Serial.println();

                _returnCode = returnCode;

                if (valid) {
                    break;
                }
//		    }
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