#include "framework.h"
#include "game.h"
#include "aimbot.h"
#include "autowall.h"
#include "prediction.h"
#include "nospread.h"

CVar z1("Enabled", 0, 0, 1, &aimbot, &aimHeader),
	 z2("FOV", 0, 0.1f, 360.f, &aimFOV, &aimHeader),
	 z3("Z Adjust", 0, -40.f, 40.f, &aimZAdjust, &aimHeader),
	 z4("Local Prediction", 0, -10.f, 10.f, &localPrediction, &aimHeader),
	 z5("Prediction", 0, -10.f, 10.f, &predictionScalar, &aimHeader),
	 z7("Bone", 0, TAG_HEAD, MAX_TAG, &aimBone, &aimHeader),
	 z8("Aim at first target", 0, 0, 1, &aimAtFirstTarget, &aimHeader),
	 z9("Target Lock", 0, 0, 1, &targetLock, &aimHeader),
	 z10("Sort", 0, 0, 2, &aimSort, &aimHeader),
	 z6("Auto Fire", 0, 0, 1, &autoFire, &aimHeader),
	 z11("Fire Start Delay", 0, 0, 500, &fireStartDelay, &aimHeader),
	 z12("Fire Delay", 0, 0, 500, &fireDelay, &aimHeader),
	 z13("Silent Aim", 0, 0, 1, &silentAim, &aimHeader),
	 z21("Aim Key", &aimHeader),
	 z14("Enabled", 0, 0, 1, &aimKeyEnabled, &z21),
	 z15("Key", 0, 0, 255, &aimKey, &z21),
	 z16("Auto Wall", &aimHeader),
	 z17("Enabled", 0, 0, 1, &autoWall, &z16),
	 z18("Threshold", 0, 0.01f, 1.0f, &autoWallThreshold, &z16),
	 removalHeader("Removals"),
	 z19("No Spread", 0, 0, 1, &noSpread, &removalHeader),
	 z20("No Recoil", 0, 0, 1, &noRecoil, &removalHeader);
