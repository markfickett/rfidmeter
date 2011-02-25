#pragma once

#include "WProgram.h"

/**
 * Encapsulate tracking pushes on a normally-open momentary button.
 * (Taken/modified from cyclocomputer, https://github.com/markfickett/bicycle .)
 *
 * After a call to check(), the button may report that it wasClicked().
 * The next call to check() will not report on that previous event.
 */
class MomentaryButton {
private:
	// Was this button pushed (closed) when last checked?
	boolean wasClosed;
	// When was this button pushed (closed)?
	unsigned long closeTimeMillis;
	const int pin;
	boolean clicked; // At last check, was this button clicked?
public:
	MomentaryButton(int inputPin);

	void setup();

	void check();

	boolean wasClicked();
};

