#include "framework.h"
#include "game.h"
#include "aimbot.h"

Entity *target;

int saEnabled  = 0,
	saMinDelay = 40,
	saMaxDelay = 100,
	saDelay,
	saMinTime  = 250,
	saMaxTime  = 400,
	saTime;

DWORD smoothAimTime;

float saMinInitialSpeed = 0.008f,
	  saMaxInitialSpeed = 0.011f,
	  saMaxSpeed        = 1.3f,
	  saSpeed,
	  saMinAcceleration = 0.018f,
	  saMaxAcceleration = 0.021f,
	  saAcceleration,
	  saStartLen,
	  saAmplitude = 0.02f,
	  saDivisions = 15;

double saAngle, saDeltaAngle;

FVector2 saLastKick;

float GetEntityThreat(Entity *ent, PlayerInfo *i) {
	FVector3 delta = viewOrigin, angles;

	delta.Subtract(&i->head);
	delta.CalculateAngles();
	delta.Normalize360();
	ent->GetAngles(&angles);
	angles.z = 0.f;
	angles.Normalize360();
	delta.Subtract(&angles);
	delta.Abs();
	if (delta.x > 270.f) {
		delta.x = 360.f - delta.x;
	}
	if (delta.y > 270.f) {
		delta.y = 360.f - delta.y;
	}
	return delta.Length();
}

float GenerateFrac() {
	return (mainPrng.genrand_int32() % 1000000) * (1 / 1000000.f);
}

bool GetAimbotFrameAngles(DVector3 *res) {
	TVec3 tAngles;
	bool result = GetAimbotAngles(&tAngles);
	TVec3ToDVec3(&tAngles, &res->x);
	if (saEnabled) {
		if (result) {
			if (targetEnt != target) {
				target         = targetEnt;
				smoothAimTime  = GetTickCount();
				saDelay        = saMinDelay + ((mainPrng.genrand_int32() % ((saMaxDelay - saMinDelay) + 1)));
				saTime         = saMinTime + ((mainPrng.genrand_int32() % ((saMaxTime - saMinTime) + 1)));
				saAcceleration = saMinAcceleration + (GenerateFrac() * (saMaxAcceleration - saMinAcceleration));
				saSpeed        = saMinInitialSpeed + (GenerateFrac() * (saMaxInitialSpeed - saMinInitialSpeed));
				saStartLen     = res->Length();
				saAngle        = (-2*M_PI) + (GenerateFrac() * (2 * (2*M_PI)));
				saDeltaAngle   = M_PI / saDivisions;
				goto dontAim;
			} else {
				DWORD currentTime = GetTickCount(), deltaTime = (currentTime - smoothAimTime);
				if (deltaTime >= saDelay) {
					deltaTime -= saDelay;
					float curSpeed = saSpeed + (saAcceleration * deltaTime);
					if (curSpeed > saMaxSpeed) {
						curSpeed = saMaxSpeed;
					}
					float aimLen = res->Length();

					DVector3 frameAngles = *res;
					frameAngles.Scale(curSpeed / aimLen);

					//double angleFrac = ;

					if (frameAngles.Length() < aimLen) {
						(*res) = frameAngles;
					}
				} else {
					dontAim:
					res->ZeroInitialize();
					return false;
				}
			}
		} else {
			target = NULL;
		}
	}
	return result;
}

float GetFOV(FVector3 *from, FVector3 *to) {
	FVector3 vec = *to;
	vec.Subtract(from);
	vec.CalculateAngles();
	vec.Subtract(&viewAngles);
	vec.Normalize180();
	return vec.Length();
}

bool IsInFOV(FVector3 *from, FVector3 *to, float fov) {
	if (fov == 360) {
		return true;
	}
	to->Subtract(from);
	to->CalculateAngles();
	to->Subtract(&viewAngles);
	to->Normalize180();
	return fov >= to->Length();
}
