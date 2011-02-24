#include "ID12.h"

#include <NewSoftSerial.h>
NewSoftSerial rfidSerial(PIN_RFID_DATA, PIN_RFID_TX_NC);

void ID12::setup()
{
	rfidSerial.begin(RFID_SERIAL_BAUD);
}

boolean ID12::hasID()
{
	return rfidSerial.available() > 0;
	return false;
}

/**
 * Copied from
 *	Martin Radlinger's implementation (readID12 in
 *	http://blog.formatlos.de/wp-content/uploads/2008/12/id12.pde )
 * which in turn credits
 *	BARRAGAN, HC Gilje, djmatic, and Martijn
 *	http://www.arduino.cc/playground/Code/ID12
 */
boolean ID12::getID(byte code[RFID_TAG_LENGTH])
{
  boolean result = false;
  byte val = 0;
  byte bytesIn = 0;
  byte tempbyte = 0;
  byte checksum = 0;
  
  // read 10 digit code + 2 digit checksum
  while (bytesIn < RFID_TAG_INPUT) 
  {                        
    if( rfidSerial.available() > 0) 
    { 
      val = rfidSerial.read();

      // if CR, LF, ETX or STX before the 10 digit reading -> stop reading
      if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) break;
      
      // Do Ascii/Hex conversion:
      if ((val >= '0') && (val <= '9')) 
        val = val - '0';
      else if ((val >= 'A') && (val <= 'F'))
        val = 10 + val - 'A';


      // Every two hex-digits, add byte to code:
      if (bytesIn & 1 == 1) 
      {
        // make some space for this hex-digit by
        // shifting the previous hex-digit with 4 bits to the left:
        code[bytesIn >> 1] = (val | (tempbyte << 4));
        
        // If we're at the checksum byte, Calculate the checksum... (XOR)
        if (bytesIn >> 1 != RFID_TAG_LENGTH) checksum ^= code[bytesIn >> 1]; 
      } 
      else 
      {
        // Store the first hex digit first...
        tempbyte = val;                           
      }

      // ready to read next digit
      bytesIn++;                                
    } 
  }

  // read complete
  if (bytesIn == RFID_TAG_INPUT) 
  { 
    // valid tag
    if(code[5] == checksum) result = true; 
  }

  // reset id-12
  //updateID12(true);


  return result;
}

boolean ID12::equal(const byte idBufferA[RFID_TAG_LENGTH],
	const byte idBufferB[RFID_TAG_LENGTH])
{
	for(int i = 0; i < RFID_TAG_LENGTH; ++i)
	{
		if (idBufferA[i] != idBufferB[i]) { return false; }
	}
	return true;
}

void ID12::copy(byte dest[RFID_TAG_LENGTH], const byte src[RFID_TAG_LENGTH])
{
	memcpy(dest, src, sizeof(dest));
}

void ID12::clear(byte idBuffer[RFID_TAG_LENGTH])
{
	memset(idBuffer, 0, sizeof(idBuffer));
}

