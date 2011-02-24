/**
 * Use RFID to meter the taking of pills.
 * Also control a light-sensing night-light.
 *
 * Each pill bottle will have an RFID tag on it; a new item will have its
 * maximal safe frequency of consumption entered (once daily, twice daily,
 * or every 4h), and then when that item is presented it will either be
 * reported as 'safe to take' (green LED) or 'already taken' (red LED). (An item
 * reported as 'safe to take' is then assumed to be immediately taken.)
 *
 */

#define PIN_SENSOR_PHOTO	A0
#define PIN_LED_LIGHT		9
#define PHOTO_SENSOR_MIN	512
#define PHOTO_SENSOR_MAX	920

#define PIN_BUTTON		6

#define PIN_SPEAKER		8
//#define USE_SPEAKER

#define PIN_LED_GREEN		10
#define PIN_LED_RED		11

// RFID pins #defined in ID12.h

#define PIN_STATUS		13

// Pre-include for ID12, since only the main .pde is pre-processed.
#include <NewSoftSerial.h>

#include "ID12.h"

struct MeteredID {
	byte id[ID12_TAG_LENGTH];
	unsigned long elapsedMillis;	// rollover when millis() wraps
	unsigned long lastTakenMillis;	// from millis()
};

byte currentID[ID12_TAG_LENGTH];

void setup()
{
	pinMode(PIN_LED_LIGHT, OUTPUT);
	pinMode(PIN_BUTTON, INPUT);

	digitalWrite(PIN_BUTTON, HIGH);

	pinMode(PIN_SPEAKER, OUTPUT);

	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	pinMode(PIN_STATUS, OUTPUT);

	#ifndef USE_SPEAKER
	digitalWrite(PIN_SPEAKER, LOW);
	#endif

	// NEXT
	//read known IDs and their intervals from EEPROM
	//set all last-taken times to 'unknown'

	ID12::setup();
	ID12::clear(currentID);

	digitalWrite(PIN_STATUS, HIGH);
	delay(500);
	digitalWrite(PIN_STATUS, LOW);

	Serial.begin(28800);
	Serial.println("Setup complete.");
}

void loop()
{
	updateLight();

	// NEXT
	//if the system clock has wrapped:
	//	set all last-taken times appropriately (to neg. or 'long ago')
	//	millis() -> unsigned long
	//		overflows after about 50 days

	// NEXT
	//if the 'new' switch is pressed:
	//	go into 'waiting for new' mode,
	//	or finish adding a new item: write the ID and interval to EEPROM

	// Something was scanned.
	if (ID12::hasID())
	{
		Serial.println("Retreiving ID...");
		byte newID[ID12_TAG_LENGTH];
		boolean gotID;

		// status double-blink: reading
		digitalWrite(PIN_STATUS, HIGH);
		gotID = ID12::getID(newID);
		delay(100);
		digitalWrite(PIN_STATUS, LOW);
		delay(100);
		digitalWrite(PIN_STATUS, HIGH);
		delay(100);
		digitalWrite(PIN_STATUS, LOW);

		if (gotID)
		{
			Serial.print("New: ");
			ID12::print(newID);
			// 500ms blink: green for new, red for same
			int ledPin;
			if (ID12::equal(newID, currentID))
			{
				ledPin = PIN_LED_RED;
				Serial.print(" (same)");
			} else {
				ledPin = PIN_LED_GREEN;
				Serial.print(" (different)");
			}
			Serial.println();

			digitalWrite(ledPin, HIGH);
			delay(500);
			digitalWrite(ledPin, LOW);

			Serial.print("\tOld:\t");
			ID12::print(currentID);
			ID12::copy(currentID, newID);
			Serial.print("\n\tSaved:\t");
			ID12::print(currentID);
			Serial.println();
		}
		else
		{
			Serial.println("Error.");
			// 2s blink red: fail
			digitalWrite(PIN_LED_RED, HIGH);
			delay(2000);
			digitalWrite(PIN_LED_RED, LOW);
		}

		/* NEXT
		if 'waiting for new':
			- record new, set to once daily (every 24h)
			- set to twice daily (every 12h)
			- set to every 4h
		otherwise:
			look up last-taken time
				- no entry: refuse
				- taken too recently: red LED, 'no' noise
				- ok: green LED, 'yes' noise, record time
			#ifdef USE_SPEAKER
			tone(PIN_SPEAKER, 440);
			noTone(PIN_SPEAKER);
			#endif

			digitalWrite(PIN_LED_GREEN, HIGH);
			digitalWrite(PIN_LED_GREEN, LOW);
			digitalWrite(PIN_LED_RED, HIGH);
			digitalWrite(PIN_LED_RED, LOW);
		*/
	}
}

/**
 * Make a (bright) LED's brightness inversely proportional to sensed ambient
 * light, with a full-off threshold. (Light-sensing night-light.)
 */
void updateLight()
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

