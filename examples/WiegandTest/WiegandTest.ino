#include <Wiegand.h>

WIEGAND wg;

void setup() {
	Serial.begin(9600);
	wg.begin();
    attachInterrupt(2, d0, FALLING);
    attachInterrupt(3, d1, FALLING);
}

void loop() {
	if(wg.available())
	{
		WiegandReturn code = wg.getCode();
        Serial.print("ID ");
        Serial.println(code.id.to_ulong());
        Serial.print("UserFields:  ");
        Serial.println(code.numberOfUserFields);
        for (int i = 0; i < code.numberOfUserFields; ++i) {
            Serial.println(code.successes[i]);
        }
        Serial.println();
	}
}

void d0() {
  wg.ReadD0();
}

void d1() {
  wg.ReadD1();
}