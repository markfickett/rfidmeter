#pragma once

/**
 * Switch an LED on or off (gradually and avoiding flickering)
 *	based on sensed ambient light.
 */

#define PIN_SENSOR_PHOTO	A0
#define PIN_LED_LIGHT		9	// with PWM
#define PHOTO_SENSOR_TURN_ON	900
#define PHOTO_SENSOR_STAY_ON	800
// Sensor values:
//	direct sunlight		12
//	bright room, sunny day	156-123
//	indoor lights		245
//	dim room, sunny day	560	
//	basement, sunny day	975-880

namespace NightLight {
	void setup();
	void updateLight();
};

