#pragma once

/**
 * Encapsulate retreiving IDs from the Innovation ID-12.
 *
 * Innovation ID-12 RFID Reader example/reference:
 *	http://slab.concordia.ca/2008/arduino/rfid/
 *	http://www.arduino.cc/playground/Code/ID12
 *	http://blog.formatlos.de/2008/12/08/arduino-id-12/
 * ID-12 Connections (ascii serial output, no external antenna):
 *	1	Ground
 *	2	Reset (strap to +5V, drop to ground to reset?)
 *	7	Format selector (ground for ascii)
 *	8	Data 1 (CMOS, unused)
 *	9	Data 0 (inverted TTL)
 *	10	Buzzer/LED (unused)
 *			ex, connect to base of transistor via resistor
 *	11	+5V (4.6-5.4V, 30mA)
 *
 * NewSoftSerial:
 *	http://arduiniana.org/libraries/NewSoftSerial/
 */

#include "WProgram.h"

#define PIN_ID12_DATA		5
#define PIN_ID12_RESET		4
#define PIN_ID12_TX_NC		12	// unused
#define ID12_SERIAL_BAUD	9600

#define ID12_TAG_LENGTH		5	// 5 bytes per ID for 10 hex characters

//#define ID12_DEBUG_PRINT

namespace ID12 {
	/** Initialize the Serial connection to the ID-12. */
	void setup();

	/**
	 * Was an RFID scanned?
	 * (Check whether the Serial connection has data waiting.)
	 */
	boolean hasID();

	/**
	 * Get the latest-scanned RFID, placing it in the given buffer.
	 * @return whether retrieval was successful; on failure,
	 *	the idBuffer may be partially modified
	 */
	boolean getID(byte idBuffer[ID12_TAG_LENGTH]);

	boolean equal(const byte idBufferA[ID12_TAG_LENGTH],
		const byte idBufferB[ID12_TAG_LENGTH]);
	void copy(byte dest[ID12_TAG_LENGTH], const byte src[ID12_TAG_LENGTH]);
	void clear(byte idBuffer[ID12_TAG_LENGTH]);
	void print(const byte idBuffer[ID12_TAG_LENGTH]);
};

