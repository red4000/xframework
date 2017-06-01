#ifndef AIMBOT_H
#define AIMBOT_H

#include "tzar.h"
#include "game.h"

class RandomOffsetCfg {
public:
	int      enabled;
	float    minLength, maxLength, velocityFrac, directionFrac;
	FVector3 scale;
};

class MultiPointCfg {
public:
	int      points;
	float    minLength, maxLength, velocityFrac, directionFrac;
	FVector3 scale;
};

class BoneCfg {
public:
	int             id, enabled, autoWall, priority;
	RandomOffsetCfg randomOffset;
	MultiPointCfg   point;
	float           prediction, zAdjust, penetration;
};

void SetDefaultBoneCfg();
void InjectCmd();
bool AimbotActive();
void AimbotPreFrame();
void AimAtEntityFrame(Entity *ent, PlayerInfo *i);
void AimbotPostFrame();
bool GetAimbotAngles(TVec3 *res);

extern int         aimbot, silentAim, autoFire, minAfterburst, maxAfterburst, fireStartDelay, fireDelay, aimBone, targetLock, aimSort, aimKeyEnabled, aimKey, aimAtFirstTarget, randomOffsetEnable;
extern float       aimFOVX, aimFOVY, aimZAdjust, targetFitness, targetPower, localPrediction;
extern BoneCfg     boneCfg[MAX_TAG];

extern DVector3    targetPosition;
extern TVec3       targetAngles;
extern Entity     *targetEnt;
extern PlayerInfo *targetPi;

#endif
