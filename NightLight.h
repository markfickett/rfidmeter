#pragma once

/**
 * Control an LED's brightness, making it inversely proportional
 * to sensed ambient light (with a full-off threshold).
 */

#define PIN_SENSOR_PHOTO	A0
#define PIN_LED_LIGHT		9	// with PWM
#define PHOTO_SENSOR_MIN	512
#define PHOTO_SENSOR_MAX	920

namespace NightLight {
	void setup();
	void updateLight();
};
