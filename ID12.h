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

#define PIN_RFID_DATA		5
#define PIN_RFID_RESET		4
#define PIN_RFID_TX_NC		12	// unused
#define RFID_SERIAL_BAUD	9600

#define RFID_TAG_LENGTH		5	// 5 bytes per ID
// ASCII input buffer: 10 data + 2 checksum
#define RFID_TAG_INPUT		12

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
	boolean getID(byte idBuffer[RFID_TAG_LENGTH]);

	boolean equal(const byte idBufferA[RFID_TAG_LENGTH],
		const byte idBufferB[RFID_TAG_LENGTH]);
	void copy(byte dest[RFID_TAG_LENGTH], const byte src[RFID_TAG_LENGTH]);
	void clear(byte idBuffer[RFID_TAG_LENGTH]);
};

