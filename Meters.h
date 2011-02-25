#pragma once

#include "ID12.h"

#define EEPROM_SIZE	512

/**
 * Provide data structures for metering RFIDs,
 * and functions for saving to and reading from EEPROM.
 */

namespace Meters {
	/**
	 * Read from EEPROM, setting all last-taken times to the current time.
	 * (This is the avoid-overdose assumption.)
	 * Record an initial value for the system clock.
	 */
	void setup(boolean reset=false);

	/**
	 * Check whether the system clock has wrapped, and if so update
	 * the stored meters' information.
	 */
	void checkClock();

	/**
	 * If the given ID is known, set the 'allowed' value according to
	 *	its interval and the elapsed time; and if allowed,
	 *	set the last-taken time to the current time.
	 * @return whether the given ID is known
	 */
	boolean checkAndUpdate(const byte id[ID12_TAG_LENGTH],
		boolean *allowedPtr);

	/**
	 * Add the given ID, with the given interval (seconds).
	 * This will replace an older entry for the same ID,
	 *	and may bump out the least-recently-used ID if necessary.
	 * A new item may be taken immediately.
	 */
	void add(const byte id[ID12_TAG_LENGTH], byte intervalHours);

	/**
	 * Remove all saved entries (including unlinking EEPROM entries).
	 */
	void clear();

};
