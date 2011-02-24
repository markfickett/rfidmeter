#include "NightLight.h"

#include "WProgram.h"

void NightLight::setup()
{
	pinMode(PIN_LED_LIGHT, OUTPUT);
}

void NightLight::updateLight()
{
	int v = analogRead(PIN_SENSOR_PHOTO);
	if (v <= PHOTO_SENSOR_MIN) {
		digitalWrite(PIN_LED_LIGHT, LOW);
	} else {
		v = constrain(v, PHOTO_SENSOR_MIN, PHOTO_SENSOR_MAX);
		analogWrite(PIN_LED_LIGHT,
			map(v, PHOTO_SENSOR_MIN, PHOTO_SENSOR_MAX, 0, 1023));
	}
}

