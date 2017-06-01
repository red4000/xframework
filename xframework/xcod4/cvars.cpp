#include "framework.h"
#include "game.h"
#include "aimbot.h"
#include "autowall.h"
#include "prediction.h"
#include "nospread.h"
#include "chams.h"
#include "aimbot.h"
#include "esp.h"

CVar z1("Enabled", 0, 0, 1, &aimbot, &aimHeader),
	 z2("FOV X", 0, 0.1f, 360.f, &aimFOVX, &aimHeader),
	 z38("FOV Y", 0, 0.1f, 360.f, &aimFOVY, &aimHeader),
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
	 z20("No Recoil", 0, 0, 1, &noRecoil, &removalHeader),
	 chamsHeader("Chams"),
	 z37("Players", &chamsHeader),
	 z22("Enabled", 0, 0, 1, &playerChams, &z37),
	 z23("Enemy Only", 0, 0, 1, &onlyEnemyChams, &z37),
	 z24("Vis cham style", 0, 0, 5, &visChamStyle, &z37),
	 z25("Invis cham style", 0, 0, 5, &invisChamStyle, &z37),
	 z26("Wallhack", 0, 0, 1, &wallHack, &z37),
	 z27("Enemy Vis Color", 0, (unsigned int*)&enemyVisCol, &z37),
	 z28("Enemy Invis Color", 0, (unsigned int*)&enemyInvisCol, &z37),
	 z29("Friendly Vis Color", 0, (unsigned int*)&friendlyVisCol, &z37),
	 z30("Friendly Invis Color", 0, (unsigned int*)&friendlyInvisCol, &z37),
	 z31("Weapons", &chamsHeader),
	 z32("Enabled", 0, 0, 1, &weaponChams, &z31),
	 z33("Vis cham style", 0, 0, 5, &visWeaponChamStyle, &z31),
	 z34("Invis cham style", 0, 0, 5, &invisWeaponChamStyle, &z31),
	 z35("Vis Color", 0, (unsigned int*)&weaponVisCol, &z31),
	 z36("Invis Color", 0, (unsigned int*)&weaponInvisCol, &z31);
	 
FVector3 zeroVec(0, 0, 0), tenVec(10, 10, 10);

void InitializeCVars() {
	SetDefaultBoneCfg();

	CVar *aimBones = new CVar("Bones", &aimHeader);

	for (int i = 0; i < MAX_TAG; i++) {
		char tmpName[32], tmpName2[64], *tmpStr, tmpStr2[16];

		if (tagName[i][0] == 'j') {
			strcpy_s(tmpName, sizeof(tmpName), &tagName[i][2]);
		} else if (tagName[i][0] == 't' && tagName[i][1] == 'a' && tagName[i][2] == 'g') {
			strcpy_s(tmpName, sizeof(tmpName), &tagName[i][4]);
		} else {
			strcpy_s(tmpName, sizeof(tmpName), tagName[i]);
		}
		tmpStr2[0] = tmpName[0];
		tmpStr2[1] = 0;
		tmpName[0] = strupr(tmpStr2)[0];
		
		tmpStr = (char*)calloc(1, strlen(tmpName) + 1);
		strcpy(tmpStr, tmpName);
	
		CVar *boneHeader = new CVar(tmpStr, aimBones);
		CVar *boneEnabled = new CVar("Enabled", 0, 0, 1, &boneCfg[i].enabled, boneHeader);
		CVar *boneAutoWall = new CVar("Auto Wall", 0, 0, 1, &boneCfg[i].autoWall, boneHeader);
		CVar *bonePrediction = new CVar("Prediction", 0, 0.f, 10.f, &boneCfg[i].prediction, boneHeader);
		CVar *boneZAdjust = new CVar("Z Adjust", 0, -20.f, 20.f, &boneCfg[i].zAdjust, boneHeader);
		CVar *bonePenetration = new CVar("Penetration", 0, 0.001f, 1.f, &boneCfg[i].penetration, boneHeader);
		
		sprintf_s(tmpName2, sizeof(tmpName2), "%s Random Offset", tmpName, boneHeader);
		tmpStr = (char*)calloc(1, strlen(tmpName2) + 1);
		strcpy(tmpStr, tmpName2);
		
		CVar *boneRandomOffset = new CVar(tmpStr, boneHeader);
		CVar *boneROMinLength = new CVar("Min Length", 0, 0.f, 50.f, &boneCfg[i].randomOffset.minLength, boneRandomOffset);
		CVar *boneROMaxLength = new CVar("Max Length", 0, 0.f, 50.f, &boneCfg[i].randomOffset.minLength, boneRandomOffset);
		CVar *boneROVelocityFrac = new CVar("Velocity Frac", 0, 0.f, 1.f, &boneCfg[i].randomOffset.velocityFrac, boneRandomOffset);
		CVar *boneRODirectionFrac = new CVar("Direction Frac", 0, 0.f, 1.f, &boneCfg[i].randomOffset.directionFrac, boneRandomOffset);
		//CVar *boneROScale = new CVar("Scale", boneRandomOffset);
		
		sprintf_s(tmpName2, sizeof(tmpName2), "%s Points", tmpName);
		tmpStr = (char*)calloc(1, strlen(tmpName2) + 1);
		strcpy(tmpStr, tmpName2);
		CVar *bonePointsHeader = new CVar(tmpStr, boneHeader);
		CVar *bonePointPoints = new CVar("Points", 0, 0, 10, &boneCfg[i].point.points, bonePointsHeader);
		CVar *bonePointMinLength = new CVar("Min Length", 0, 0.f, 50.f, &boneCfg[i].point.minLength, bonePointsHeader);
		CVar *bonePointMaxLength = new CVar("Max Length", 0, 0.f, 50.f, &boneCfg[i].point.maxLength, bonePointsHeader);
		CVar *bonePointVelocityFrac = new CVar("Velocity Frac", 0, 0.f, 1.f, &boneCfg[i].point.velocityFrac, bonePointsHeader);
		CVar *bonePointDirectionFrac = new CVar("Direction Frac", 0, 0.f, 1.f, &boneCfg[i].point.directionFrac, bonePointsHeader);
		//CVar *bonePointScale = new CVar("Scale", bonePointsHeader);
	}
}

void SaveCVars() {
}
