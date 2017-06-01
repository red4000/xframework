#ifndef AIMBOT_H
#define AIMBOT_H

#include "tzar.h"

void InjectCmd();
bool AimbotActive();
void AimbotPreFrame();
void AimAtEntityFrame(Entity *ent, PlayerInfo *i);
void AimbotPostFrame();
bool GetAimbotAngles(TVec3 *res);

extern int         aimbot, silentAim, autoFire, fireStartDelay, fireDelay, aimBone, targetLock, aimSort, aimKeyEnabled, aimKey, aimAtFirstTarget;
extern float       aimFOV, aimZAdjust, targetFitness, localPrediction;
extern DVector3    targetPosition;
extern TVec3       targetAngles;
extern Entity     *targetEnt;
extern PlayerInfo *targetPi;

#endif
