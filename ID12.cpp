#include "ID12.h"

#include <NewSoftSerial.h>
NewSoftSerial rfidSerial(PIN_ID12_DATA, PIN_ID12_TX_NC);

void ID12::setup()
{
	pinMode(PIN_ID12_DATA, INPUT);

	#ifdef ID12_DEBUG_PRINT
	Serial.begin(28800);
	#endif
	rfidSerial.begin(ID12_SERIAL_BAUD);
}

boolean ID12::hasID()
{
	return rfidSerial.available() > 0;
}

namespace {
	void printHexChar(byte hexChar)
	{
		if(hexChar <= 9) {
			Serial.write('0' + hexChar);
		} else {
			Serial.write('A' + (hexChar - 10));
		}
	}
	void printHex(const byte *packedHex, unsigned int numBytes)
	{
		for(unsigned int i = 0; i < numBytes; ++i)
		{
			byte data = packedHex[i];
			printHexChar(data >> 4);
			printHexChar(data & 0xF);
		}
	}
}

void ID12::print(const byte idBuffer[ID12_TAG_LENGTH])
{
	printHex(idBuffer, ID12_TAG_LENGTH);
}

#define STX	2
#define ETX	3
#define CR	13
#define LF	10

/**
 * Read the ASCII output from the ID-12, expected to be:
 *	(see http://slab.concordia.ca/2008/arduino/rfid/ )
 *	STX,	10 chars,	2 chars,	CR,	LF,	ETX
 *	start,	data,		checksum,	line endings,	end
 *	02,	uppercase/#  ASCII (hex),	13,	10,	03
 *	0	1-10		11,12		13	14	15
 */
boolean ID12::getID(byte idBuffer[ID12_TAG_LENGTH])
{
	#ifdef ID12_DEBUG_PRINT
	Serial.println("Getting data from RFID.");
	#else
	delay(10); // Let the ID-12 finish queueing data?
	#endif
	int v, i = -1;
	byte data, checksum;

	while(rfidSerial.available() > 0)
	{
		i++;
		v = rfidSerial.read();
		// Verify the expected non-data characters.
		boolean isData = false;
		switch(i) {
			case 0:
				if (v != STX) {
					#ifdef ID12_DEBUG_PRINT
					Serial.print("Fail: ");
					Serial.print(v);
					Serial.println("not STX");
					#endif
					return false;
				}
				break;
			case 13:
				if (v != CR) {
					#ifdef ID12_DEBUG_PRINT
					Serial.println("Fail: not CR");
					#endif
					return false;
				}
				break;
			case 14:
				if (v != LF) {
					#ifdef ID12_DEBUG_PRINT
					Serial.println("Fail: not LF");
					#endif
					return false;
				}
				break;
			case 15:
				if (v != ETX) {
					#ifdef ID12_DEBUG_PRINT
					Serial.println("Fail: not ETX");
					#endif
					return false;
				}
				break;
			default:
				if (i > 15) { return false; }
				isData = true;
				break;
		}
		if (!isData) { continue; }

		// Process data characters.

		if (v >= '0' && v <= '9') {
			data = v - '0';
		} else if (v >= 'A' && v <= 'Z') {
			data = 10 + (v - 'A');
		} else {
			#ifdef ID12_DEBUG_PRINT
			Serial.println("Fail: not a hex ASCII char");
			#endif
			return false;
		}

		if (i <= 10) {
			int outIndex = (i-1)/2;
			if (i % 2 == 1) {
				idBuffer[outIndex] = data << 4;
			} else {
				idBuffer[outIndex] |= data;
			}
		} else {
			if (i == 11) {
				checksum = data << 4;
			} else { // 12
				checksum |= data;
			}
		}
	}

	#ifdef ID12_DEBUG_PRINT
	Serial.print("Got ID: ");
	ID12::print(idBuffer);
	Serial.print(", Checksum: ");
	printHex(&checksum, 1);
	Serial.println();
	#endif

	// Verify the checksum.
	byte checksumActual = 0;
	for(int i = 0; i < ID12_TAG_LENGTH; i++) {
		checksumActual ^= idBuffer[i];
	}
	if (checksum != checksumActual) {
		#ifdef ID12_DEBUG_PRINT
		Serial.println("Fail: checksum mismatch");
		#endif
		return false;
	}

	return true;
}

boolean ID12::equal(const byte idBufferA[ID12_TAG_LENGTH],
	const byte idBufferB[ID12_TAG_LENGTH])
{
	for(int i = 0; i < ID12_TAG_LENGTH; ++i)
	{
		if (idBufferA[i] != idBufferB[i]) { return false; }
	}
	return true;
}

void ID12::copy(byte dest[ID12_TAG_LENGTH], const byte src[ID12_TAG_LENGTH])
{
	memcpy(dest, src, sizeof(byte)*ID12_TAG_LENGTH);
}

void ID12::clear(byte idBuffer[ID12_TAG_LENGTH])
{
	memset(idBuffer, 0, sizeof(idBuffer));
}
