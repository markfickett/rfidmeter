#include "Meters.h"
#include "Config.h"

#include "WProgram.h"

#include <EEPROM.h>

// 4294967295
const unsigned long MILLIS_MAX = ~0;

namespace Meters {
	/**
	 * Storage in EEPROM is:
	 *	1 byte			number of meters
	 * Followed by meters; repeating:
	 *	ID12_TAG_LENGTH bytes	ID
	 *	1 byte			consumption interval minimum (hours)
	 */
	const int METEREDID_BYTES = (1 + ID12_TAG_LENGTH);
        const int NUM_METERS_MAX
		#ifdef TEST_WITH_SMALL_VALUES
		= 3;
		#else
		= (512 - 1) / METEREDID_BYTES;
		#endif
	unsigned int numMeters;
	unsigned long lastTimeMillis;

	class MeteredID {
		public:
                byte id[ID12_TAG_LENGTH];
                byte intervalHours;
                unsigned long elapsedMillis;    // rollover when millis() wraps
                unsigned long lastTakenMillis;  // from millis()

		void *operator new(size_t size) { return malloc(size); }
		void operator delete(void* ptr) { if (ptr) { free(ptr); } }

		void readFromEEPROM(unsigned int index)
		{
			unsigned int baseAddress = 1 + index*METEREDID_BYTES;
			for(unsigned int i = 0; i < ID12_TAG_LENGTH; i++)
			{
				id[i] = EEPROM.read(baseAddress + i);
			}
			intervalHours = EEPROM.read(baseAddress
				+ ID12_TAG_LENGTH);
                	elapsedMillis = 0;
                	lastTakenMillis = 0;
		}

		void writeToEEPROM(unsigned int index, boolean writeID)
		{
			unsigned int baseAddress = 1 + index*METEREDID_BYTES;
			if (writeID)
			{
				for(unsigned int i=0; i < ID12_TAG_LENGTH; i++)
				{
					EEPROM.write(baseAddress + i, id[i]);
				}
			}
			EEPROM.write(baseAddress + ID12_TAG_LENGTH,
				intervalHours);
		}
        };

	MeteredID *meters[NUM_METERS_MAX] = {NULL};

	/** @return the index of the meter for the given ID, or -1. */
	int getMeterIndex(const byte id[ID12_TAG_LENGTH]);
	/** @return the number of seconds since last taken */
	unsigned long getElapsedSeconds(MeteredID *meteredID);
};

void Meters::setup(boolean doReset)
{
	if (doReset) {
		numMeters = 0;
		EEPROM.write(0, 0);
	} else {
		numMeters = (unsigned int)EEPROM.read(0);
	}

	Serial.print("Reading ");
	Serial.print(numMeters);
	Serial.println(" meters from EEPROM.");
	for(unsigned int i = 0; i < numMeters; i++)
	{
		meters[i] = new MeteredID();
		meters[i]->readFromEEPROM(i);
		Serial.print("\t");
		ID12::print(meters[i]->id);
		Serial.print(": interval ");
		Serial.print((int)meters[i]->intervalHours);
		#ifdef TEST_WITH_SMALL_VALUES
		Serial.println("s");
		#else
		Serial.println("h");
		#endif
	}
	lastTimeMillis = millis();
}

void Meters::checkClock()
{
	// wraps about every 50 days
	unsigned long newTimeMillis = millis();
	if (newTimeMillis < Meters::lastTimeMillis)
	{
		Serial.println("Clock wrapped.");
		for(int i = 0; i < numMeters; ++i)
		{
			unsigned int newElapsed = meters[i]->elapsedMillis
				+ (newTimeMillis - meters[i]->lastTakenMillis);
			meters[i]->elapsedMillis =
				newElapsed > meters[i]->elapsedMillis
				? newElapsed : MILLIS_MAX;
			meters[i]->lastTakenMillis = newTimeMillis;
		}
	}
	lastTimeMillis = newTimeMillis;
}

int Meters::getMeterIndex(const byte id[ID12_TAG_LENGTH])
{
	for(int i = 0; i < numMeters; ++i)
	{
		if(ID12::equal(id, meters[i]->id)) { return i; }
	}
	return -1;
}

unsigned long Meters::getElapsedSeconds(MeteredID *meteredID)
{
	Serial.print("Elapsed for ");
	ID12::print(meteredID->id);
	Serial.println(":");
	unsigned long newElapsedMillis =
		lastTimeMillis - meteredID->lastTakenMillis;
	Serial.print("\tnewElapsed:\t");
	Serial.print(newElapsedMillis);
	Serial.println("ms");
	Serial.print("\told elapsed:\t");
	Serial.print(meteredID->elapsedMillis);
	Serial.println("ms");
	
	return (lastTimeMillis - meteredID->lastTakenMillis)
		/1000
	+ meteredID->elapsedMillis/1000;
}

boolean Meters::checkAndUpdate(const byte id[ID12_TAG_LENGTH],
	boolean *allowedPtr)
{
	Serial.print("Allow ");
	ID12::print(id);
	Serial.print("? ");
	int i = getMeterIndex(id);
	if (i == -1)
	{
		Serial.println("Unknown.");
		return false;
	}

	unsigned long elapsedSecs = getElapsedSeconds(meters[i]);
	unsigned long intervalSecs =
		#ifndef TEST_WITH_SMALL_VALUES
		60*60*
		#endif
		(unsigned long)meters[i]->intervalHours;

	Serial.print("Elapsed: ");
	Serial.print(elapsedSecs);
	Serial.print("s >=? Interval: ");
	Serial.print(intervalSecs);
	Serial.print("s -> ");

	if (elapsedSecs >= intervalSecs)
	{
		*allowedPtr = true;
		meters[i]->elapsedMillis = 0;
		meters[i]->lastTakenMillis = lastTimeMillis;
		// TODO If we had RTC, we would also
		//	write last-taken time to EEPROM.
		Serial.println("Yes (updated).");
	}
	else
	{
		Serial.println("No.");
		*allowedPtr = false;
	}

	return true;
}

void Meters::add(const byte id[ID12_TAG_LENGTH], byte intervalHours)
{
	Serial.print("Adding MeteredID for ");
	ID12::print(id);
	Serial.print(": ");
	int addIndex = getMeterIndex(id);
	boolean writeID = false;
	if (addIndex == -1)
	{
		if (numMeters < NUM_METERS_MAX)
		{
			addIndex = numMeters++;
			EEPROM.write(0, (byte)numMeters);
			meters[addIndex] = new MeteredID();
			Serial.println("New entry.");
		}
		else
		{
			unsigned long maxElapsedSecs = 0, elapsedSecs;
			for(int i = 0; i < numMeters; ++i)
			{
				elapsedSecs =
					Meters::getElapsedSeconds(meters[i]);
				if (elapsedSecs > maxElapsedSecs)
				{
					maxElapsedSecs = elapsedSecs;
					addIndex = i;
				}
			}
			Serial.print("Kicking out entry for ");
			ID12::print(meters[addIndex]->id);
			Serial.print(", last used ");
			Serial.print(maxElapsedSecs);
			Serial.println(" ago.");
		}
		ID12::copy(meters[addIndex]->id, id);
		writeID = true;
	}
	else
	{
		Serial.println("Overwriting matching old entry.");
	}

	meters[addIndex]->intervalHours = intervalHours;
	meters[addIndex]->writeToEEPROM(addIndex, writeID);
	meters[addIndex]->lastTakenMillis = lastTimeMillis;
	meters[addIndex]->elapsedMillis = MILLIS_MAX;
}

void Meters::clear()
{
	Serial.println("Clearing saved meters.");
	if (numMeters <= 0) {
		return;
	}

	for(unsigned int i = 0; i < numMeters; i++)
	{
		delete meters[i];
	}
	numMeters = 0;
	EEPROM.write(0, numMeters);
}

