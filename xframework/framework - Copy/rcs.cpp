#include "framework.h"
#include "game.h"
#include "aim.h"
#include "rcs.h"

int rcsEnabled = 0, rcsDelay = 50;
float rcsMinLength = 0.3f, rcsStartSpeed = 0.01f, rcsAccel = 0.02f, rcsMaxSpeed = 0.75f, rcsRandom = 2.5f;

float rcsFactor1, rcsFactor2;
DWORD rcsStartTime;

bool GetRCSAngle(DVector3 *res) {
	DVector3 recoil;
	GetRecoil(&recoil);
	if (recoil.Length() > rcsMinLength) {
		DWORD currentTime = GetTickCount();
		if (rcsStartTime == 0) {
			rcsStartTime = currentTime;
			float h = rcsRandom * 0.5f;
			rcsFactor1   = (-h) + GenerateFrac() * (rcsRandom * 2);
			rcsFactor2   = (-h) + GenerateFrac() * (rcsRandom * 2);
		} else {
			DWORD deltaTime = currentTime - rcsStartTime;
			if (deltaTime > rcsDelay) {
				double rcsScalar = rcsStartSpeed + (rcsAccel * deltaTime);
				if (rcsScalar > rcsMaxSpeed) {
					rcsScalar = rcsMaxSpeed;
				}

				double len = recoil.Length();
				recoil.CalculateAngles();
				recoil.x += rcsFactor1;
				recoil.y += rcsFactor2;
				recoil.AngleVectors(&recoil, NULL, NULL);
				recoil.Scale(len);
				/*
				recoil.Scale(rcsScalar / recoil.Length());
				recoil.Scale(-1.0);
				*/
				recoil.Scale(-rcsScalar);
				res->x = recoil.x;
				res->y = recoil.y;
				res->z = 0;
				return true;
			}
		}
	}
	return false;
}
