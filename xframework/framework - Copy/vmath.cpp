#include "vmath.h"

double NormalizeAngle180(double angle) {
	angle = NormalizeAngle360(angle);

	/*while (angle > 180.0) {
		angle -= 360.0;
	}
	while (angle < -180.0) {
		angle += 360.0;
	}*/
	if (angle > 180.0) {
		angle -= 360.0;
	}
	if (angle < -180.0) {
		angle += 360.0;
	}

	return angle;
}

double NormalizeAngle360(double angle) {
	//return (360.0 / 65536.0) * ((int)(angle * (65536.0 / 360.0)) & 65535);
	/*double result = fmod(angle, 360.0);

	if (0 > result) {
		result += 360.0;
	}

	return result;*/
	/*while (angle > 360.0) {
		angle -= 360.0;
	}
	while (angle < 0.0) {
		angle += 360.0;
	}*/
	if (angle > 360.0) {
		angle -= 360.0;
	}
	if (angle < 0.0) {
		angle += 360.0;
	}
	return angle;
}

double Lerp(double from, double to, double frac) {
	return from + (frac * (to - from));
}