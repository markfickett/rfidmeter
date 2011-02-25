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

#include "Config.h"

#define PIN_BUTTON		6

#define PIN_SPEAKER		8
//#define USE_SPEAKER

#define PIN_LED_GREEN		10
#define PIN_LED_RED		11

#define PIN_STATUS		13

// Pre-include for ID12, since only the main .pde is pre-processed.
#include <NewSoftSerial.h>

#include "Meters.h"

// RFID pins #defined in ID12.h
#include "ID12.h"

// LED and ambient light sensor pins #defined in NightLight.h
#include "NightLight.h"

#include "MomentaryButton.h"

byte currentID[ID12_TAG_LENGTH];

MomentaryButton addButton(PIN_BUTTON);
boolean adding;
boolean gotAddID;
#define INTERVAL_COUNT	3
unsigned int intervalIndex;
const byte INTERVALS[] = {24, 12, 4};

void setup()
{
	pinMode(PIN_STATUS, OUTPUT);
	digitalWrite(PIN_STATUS, HIGH);

	pinMode(PIN_BUTTON, INPUT);

	digitalWrite(PIN_BUTTON, HIGH);

	pinMode(PIN_SPEAKER, OUTPUT);

	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	#ifndef USE_SPEAKER
	digitalWrite(PIN_SPEAKER, LOW);
	#endif

	ID12::setup();
	ID12::clear(currentID);

	Meters::setup();

	addButton.setup();
	adding = false;

	Serial.begin(28800);
	Serial.println("Setup complete.");

	digitalWrite(PIN_STATUS, LOW);
}

void loop()
{
	NightLight::updateLight();

	Meters::checkClock();

	addButton.check();
	if (addButton.wasClicked())
	{
		adding = !adding;
		if (adding)
		{
			Serial.println("Adding...");
			gotAddID = false;
			intervalIndex = 0;
		}
		else if (gotAddID)
		{
			Meters::add(currentID, INTERVALS[intervalIndex]);
			Serial.println("Add complete.");
		}
		else
		{
			Serial.println("Add cancelled.");
		}
	}

	// Something was scanned.
	if (ID12::hasID())
	{
		Serial.println("Reading ID... ");
		boolean gotID;

		digitalWrite(PIN_STATUS, HIGH);
		gotID = ID12::getID(currentID);
		delay(100);
		digitalWrite(PIN_STATUS, LOW);

		if (gotID)
		{
			ID12::print(currentID);
			Serial.println();

			if (adding)
			{
				if (!gotAddID)
				{
					Serial.println("Ready to add.");
					gotAddID = true;
				}
				else
				{
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
			}
			else
			{
				boolean allowed;
				boolean found = Meters::checkAndUpdate(
					currentID,
					&allowed);
				if (found)
				{
					if (allowed)
					{
						announceYes();
					}
					else
					{
						announceNo();
					}
				}
				else
				{
					Serial.print("No record of ");
					ID12::print(currentID);
					Serial.println();
					announceError();
				}
			}
		}
		else
		{
			Serial.println("Error.");
			announceError();
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

