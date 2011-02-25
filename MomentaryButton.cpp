#include "MomentaryButton.h"


MomentaryButton::MomentaryButton(int inputPin)
	: wasClosed(false), pin(inputPin)
{}

void MomentaryButton::setup()
{
	pinMode(pin, INPUT);
	digitalWrite(pin, HIGH);
}

void MomentaryButton::check()
{
	unsigned long currentTimeMillis = millis();
	boolean isClosed = (digitalRead(pin) == LOW);

	if (isClosed != wasClosed)
	{
		if (isClosed)
		{
			closeTimeMillis = currentTimeMillis;
		}
	}

	clicked = !isClosed && wasClosed;
	wasClosed = isClosed;
}

boolean MomentaryButton::isPressed() {
	return wasClosed;
}

boolean MomentaryButton::wasClicked() {
	return clicked;
}

