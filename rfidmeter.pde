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
 * To add a new ID (pill bottle), or to set a new interval, press the 'Add'
 * button (red and green blink), then scan the item; repeat scanning to cycle
 * the interval (green blinks: once for daily, twice for twice daily, thrice
 * for every 4h). Press 'Add' again to finish (or cancel).
 *
 * Press 'Reset' to clear saved IDs (which are persistent over power cycling,
 * saved to EEPROM); or hold 'Reset' when turning on power to avoid reading
 * saved entries (for example, the first time, when EEPROM has garbage data).
 */

// Pre-includes for ID12 and Meters, since only the main .pde is pre-processed.
#include <NewSoftSerial.h>
#include <EEPROM.h>

#include "Config.h"
#include "MomentaryButton.h"

// some pin #s defined in their respective headers
#include "Meters.h"
#include "ID12.h"
#include "NightLight.h"

#define PIN_BUTTON_ADD		5
#define PIN_BUTTON_RESET	6

#define PIN_SPEAKER		8
#define USE_SPEAKER

#define PIN_LED_GREEN		10
#define PIN_LED_RED		11

#define PIN_STATUS		13

unsigned int currentTimeMillis;
byte currentID[ID12_TAG_LENGTH];

MomentaryButton resetButton(PIN_BUTTON_RESET);

MomentaryButton addButton(PIN_BUTTON_ADD);
boolean adding;
boolean gotAddID;
#define INTERVAL_COUNT	3
unsigned int intervalIndex;
const byte INTERVALS[] = {24, 12, 4};
unsigned int lastAddFeedbackMillis;

void setup()
{
	pinMode(PIN_STATUS, OUTPUT);
	digitalWrite(PIN_STATUS, HIGH);
	Serial.begin(28800);

	pinMode(PIN_SPEAKER, OUTPUT);
	#ifndef USE_SPEAKER
	digitalWrite(PIN_SPEAKER, LOW);
	#endif

	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	resetButton.setup();
	resetButton.check();

	addButton.setup();
	adding = false;
	lastAddFeedbackMillis = millis();

	ID12::setup();
	ID12::clear(currentID);

	Meters::setup(resetButton.isPressed());

	Serial.println("Setup complete.");
	digitalWrite(PIN_STATUS, LOW);
}

void loop()
{
	currentTimeMillis = millis();

	addButton.check();
	resetButton.check();

	NightLight::updateLight();

	if (resetButton.wasClicked()) {
		Meters::clear();
	}

	Meters::checkClock();

	if (addButton.wasClicked()) {
		adding = !adding;
		if (adding) {
			Serial.println("Adding...");
			gotAddID = false;
			intervalIndex = 0;
		} else if (gotAddID) {
			Meters::add(currentID, INTERVALS[intervalIndex]);
			Serial.println("Add complete.");
		} else {
			Serial.println("Add cancelled.");
		}
	}

	// Something was scanned.
	if (ID12::hasID()) {
		digitalWrite(PIN_STATUS, HIGH);
		boolean gotID = ID12::getID(currentID);
		digitalWrite(PIN_STATUS, LOW);

		if (gotID) {
			if (adding) {
				if (!gotAddID) {
					Serial.println("Ready to add.");
					gotAddID = true;
				} else {
					intervalIndex = (intervalIndex + 1)
						% INTERVAL_COUNT;
				}

				Serial.print("Interval #");
				Serial.print(intervalIndex);
				Serial.print(": ");
				Serial.print((int)INTERVALS[intervalIndex]);
				#ifdef TEST_WITH_SMALL_VALUES
				Serial.println("s");
				#else
				Serial.println("h");
				#endif
			} else {
				boolean allowed;
				boolean found = Meters::checkAndUpdate(
					currentID,
					&allowed);

				if (found) {
					if (allowed) {
						announceYes();
					} else {
						announceNo();
					}
				} else {
					Serial.print("No record of ");
					ID12::print(currentID);
					Serial.println();
					announceError();
				}
			}
		} else {
			Serial.println("Error getting ID.");
			announceError();
		}
	}

	// Blink for feedback during adding.
	if (adding && (currentTimeMillis - lastAddFeedbackMillis) > 1000) {
		lastAddFeedbackMillis = currentTimeMillis;
		if (gotAddID) {
			for(int i = 0; i <= intervalIndex; i++)
			{
				digitalWrite(PIN_LED_GREEN, HIGH);
				delay(100);
				digitalWrite(PIN_LED_GREEN, LOW);
				delay(100);
			}
		} else {
			digitalWrite(PIN_LED_GREEN, HIGH);	delay(100);
			digitalWrite(PIN_LED_GREEN, LOW);	delay(100);
			digitalWrite(PIN_LED_RED, HIGH);	delay(100);
			digitalWrite(PIN_LED_RED, LOW);
		}
	}
}


void beepOrWait(int frequency, int duration)
{
	#ifdef USE_SPEAKER
	tone(PIN_SPEAKER, frequency);
	#endif
	delay(duration);
	#ifdef USE_SPEAKER
	noTone(PIN_SPEAKER);
	#endif
}

void announceYes()
{
	// C, F, A: 261, 349, 440
	digitalWrite(PIN_LED_GREEN, HIGH);
	beepOrWait(261, 100);
	beepOrWait(349, 100);
	beepOrWait(440, 100);
	digitalWrite(PIN_LED_GREEN, LOW);
}

void announceNo()
{
	// D, C#, D, C#: 293, 277
	digitalWrite(PIN_LED_RED, HIGH);
	beepOrWait(293, 100);
	beepOrWait(277, 100);
	beepOrWait(293, 100);
	beepOrWait(277, 100);
	digitalWrite(PIN_LED_RED, LOW);
}

void announceError()
{
	digitalWrite(PIN_LED_RED, HIGH);
	beepOrWait(277, 500);
	delay(100);
	beepOrWait(277, 500);
	digitalWrite(PIN_LED_RED, LOW);
}

