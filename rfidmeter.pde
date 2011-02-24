/**
 * Use RFID to meter the taking of pills.
 *
 * Each pill bottle will have an RFID tag on it; a new item will have its
 * maximal safe frequency of consumption entered (once v. twice daily), and then
 * when presented will either be reported as 'safe to take' or 'already taken'.
 *
 * Innovation ID-12 RFID Reader reference:
 *	http://blog.formatlos.de/2008/12/08/arduino-id-12/
 *
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

#define PIN_SENSOR_PHOTO	A0
#define PIN_LED_LIGHT		9
#define PHOTO_SENSOR_MIN	512
#define PHOTO_SENSOR_MAX	920

#define PIN_BUTTON		6

#define PIN_SPEAKER		8
//#define USE_SPEAKER

#define PIN_LED_GREEN		10
#define PIN_LED_RED		11

#define PIN_RFID_DATA		5
#define PIN_RFID_RESET		4

#define PIN_STATUS		13

void setup()
{
	pinMode(PIN_LED_LIGHT, OUTPUT);
	pinMode(PIN_BUTTON, INPUT);

	digitalWrite(PIN_BUTTON, HIGH);

	pinMode(PIN_SPEAKER, OUTPUT);

	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	pinMode(PIN_RFID_DATA, INPUT);
	pinMode(PIN_RFID_RESET, OUTPUT);
	digitalWrite(PIN_RFID_RESET, HIGH);

	pinMode(PIN_STATUS, OUTPUT);

	#ifndef USE_SPEAKER
	digitalWrite(PIN_SPEAKER, LOW);
	#endif
}

void loop()
{
	updateLight();
	#ifdef USE_SPEAKER
	tone(PIN_SPEAKER, 440);
	#endif
	digitalWrite(PIN_LED_GREEN, HIGH);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_STATUS, HIGH);
	delay(500);

	updateLight();
	#ifdef USE_SPEAKER
	noTone(PIN_SPEAKER);
	#endif
	digitalWrite(PIN_LED_GREEN, LOW);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_STATUS, LOW);
	delay(1000);
}

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

