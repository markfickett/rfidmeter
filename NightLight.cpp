#include "NightLight.h"

#include "WProgram.h"

#define SWITCH_DELAY	1000	// Wait SWITCH_DELAY ms before switching.
#define SWITCH_TIME	2000	// Gradually change over SWITCH_TIME ms.
#define VALUE_ON	255	// analog value for full-on
#define VALUE_OFF	0	// analog value for full-off
boolean isOn, shouldBeOn;
unsigned long lastChangeMillis;
int currentValue;

void NightLight::setup()
{
	pinMode(PIN_LED_LIGHT, OUTPUT);
	analogWrite(PIN_LED_LIGHT, VALUE_OFF);
	isOn = shouldBeOn = false;
	lastChangeMillis = millis();
	currentValue = 0;
}

void NightLight::updateLight()
{
	int v = analogRead(PIN_SENSOR_PHOTO);

	unsigned long currentMillis = millis();
	unsigned long elapsedMillis = currentMillis - lastChangeMillis;

	if(v >= PHOTO_SENSOR_THRESHOLD != shouldBeOn) {
		shouldBeOn = !shouldBeOn;
		lastChangeMillis = currentMillis;
	} else if (isOn != shouldBeOn && elapsedMillis >= SWITCH_DELAY) {
		isOn = shouldBeOn;
	} else if (currentValue != (isOn ? VALUE_ON : VALUE_OFF)) {
		elapsedMillis = constrain(elapsedMillis, 0, SWITCH_TIME);
		currentValue = map(elapsedMillis, SWITCH_DELAY, SWITCH_TIME,
			isOn ? VALUE_OFF : VALUE_ON,
			isOn ? VALUE_ON : VALUE_OFF);

		//Serial.print("NightLight new value: ");
		//Serial.println(currentValue);
		switch(currentValue) {
			case 0:
				digitalWrite(PIN_LED_LIGHT, LOW);
				break;
			case 1023:
				digitalWrite(PIN_LED_LIGHT, HIGH);
				break;
			default:
				analogWrite(PIN_LED_LIGHT, currentValue);
				break;
		}
	}
}

