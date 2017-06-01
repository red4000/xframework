#include "framework.h"
#include "game.h"
#include "_game.h"
#include "nospread.h"

int   autoWall          = 0;
float autoWallThreshold = 0.45f;

DWORD dwDrawBulletTrajectory = 0x455A80;

DWORD    osv_clientSideBullets;
int      entNum;
bool     hitTarget;
Entity  *traceEnt;
double   length;
FVector3 endPos, tmp;

LONG WINAPI HandleAWException(EXCEPTION_POINTERS *ex) {
	if (0x455A26 == ex->ContextRecord->Eip) {
		if (0x4560FD != *(DWORD*)ex->ContextRecord->Esp) {
			FVector3 *end = *(FVector3**)(ex->ContextRecord->Esp + 16);
			entNum = ex->ContextRecord->Eax;
			endPos = *end;

			if (entNum == traceEnt->GetIndex()) {
				hitTarget = true;
			}

			tmp = endPos;
			tmp.Subtract(&viewOrigin);

			if (tmp.Length() >= length) {
				hitTarget = true;
			}
		}

		ex->ContextRecord->Eip = 0x455A7A;
		return EXCEPTION_CONTINUE_EXECUTION;
	} else if (0x4A69BC == ex->ContextRecord->Eip) {
		ex->ContextRecord->Ecx = osv_clientSideBullets;
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void DrawBulletTrajectory(Entity *entity, int localClientNum, BulletFireParams *bulletTrace, weapon_t *weapon, FVector3 *vieworg, int tracer) {
	__asm {
		push tracer
		mov  edi, entity
		push vieworg
		push weapon
		push bulletTrace
		push localClientNum
		call dwDrawBulletTrajectory
		add  esp, 0x14
	}
}

float CalculatePower(Entity *ent, FVector3 *start, FVector3 *end) {
	BulletFireParams bt;
	DWORD            weaponIndex;
	float            damage = 0.0f;

	//if (cg->predictedPlayerState.weapFlags & 2) { // offhand weapon, like grenade etc
	//	return false;
	//} else {
		weaponIndex = ((centity_t*)localEntity)->nextState.weapon;
	//}

	weapon_t *weapon = GetWeaponByIndex(weaponIndex);

	bt.weaponEntIndex   = 1023;
	bt.ignoreEntIndex   = cg->clientNum;
	bt.damageMultiplier = 1.0f;
	bt.methodOfDeath    = 1/**(int*)(weapon + 0x138)*/;
	bt.origStart        = bt.start = *start;

	bt.dir = *end;
	bt.dir.Subtract(start);
	//double length = 1.0 / (bt.dir[0] * bt.dir[0] + bt.dir[1] * bt.dir[1] + bt.dir[2] * bt.dir[2]);
	//bt.dir[0] *= length; bt.dir[1] *= length; bt.dir[2] *= length;
	double length = bt.dir.Length();
	bt.dir[0] = (float)(((double)bt.dir[0]) / length); bt.dir[1] = (float)(((double)bt.dir[1]) / length); bt.dir[2] = (float)(((double)bt.dir[2]) / length);

	bt.end = *end;

	hitTarget = false;
	traceEnt  = ent;

	osv_clientSideBullets = *(DWORD*)0x185C434;
	BYTE odynEnt_active   = *(BYTE*)((*(DWORD*)0x11015EC) + 12);
	*((DWORD*)0x185C434)                = NULL; // sv_clientSideBullets
	*(BYTE*)((*(DWORD*)0x11015EC) + 12) = 0; // dynEnt_active
	//DWORD oWeaponType                   = *(DWORD*)(((DWORD)weapon) + 0x12C);
	//*(DWORD*)(((DWORD)weapon) + 0x12C)  = 1;

	DrawBulletTrajectory((Entity*)0x7973D8, cg->clientNum, &bt, weapon, &viewOrigin, 0); // predictedPlayerEntity

	*((DWORD*)0x185C434)                = osv_clientSideBullets;
	*(BYTE*)((*(DWORD*)0x11015EC) + 12) = odynEnt_active;
	//*(DWORD*)(((DWORD)weapon) + 0x12C)  = oWeaponType;

	if (bt.ignoreEntIndex >= 0 && bt.ignoreEntIndex < MAX_PLAYERS) {
		Entity *hitEntity = GetEntityByIndex(bt.ignoreEntIndex);

		if (hitEntity->IsHostile() && hitEntity->IsAlive() && (hitEntity->IsLocalEntity() == false)) {
			hitTarget = true;
		}
	}

	if (bt.damageMultiplier < 0.0f) {
		bt.damageMultiplier = 0.0f;
	}

	if (hitTarget && bt.damageMultiplier > 0.0f && bt.damageMultiplier != 144.0f) {
		return bt.damageMultiplier;
	}

	return 0.f;
}

/*
float GetPenetrationPower(weapon_t *wep, int materialType) {
	float result;
	float *penetration_table = *(float**)0x007373B8;
	int penetrationType = *(int*)(wep + 0x134);

	if (materialType) {
		result = penetration_table[materialType + 0x1D * penetrationType];
  } else {
		result = 0.0f;
  }

	return result;
}

bool TraceBullet(trace_t* trace, BulletFireParams* bulletTrace, centity_t* cent, int material) {
	bool result = false;
	DWORD address = 0x004558E0;
	__asm {
		push    [material];
		push    [cent];
		push    [bulletTrace];
		lea     eax, [trace];
		call[address];
		mov[result], al;
		add esp, 0x0C;
	}
	return result;
}

bool StepForward(BulletFireParams* bulletTrace, trace_t* trace, float factor) {
	int result = 0;
	short hitEntNum = 0;

	int traceContents = trace->contents;
	if (traceContents == 2 || traceContents == 3) {
		hitEntNum = 1022;
	} else {
		if (traceContents == 1) {
			hitEntNum = trace->hitId;
	} else {
			hitEntNum = 1023;
	}
	}

	bulletTrace->ignoreEntIndex = hitEntNum;

	if (hitEntNum != 1022 || factor <= 0.0f) {
		bulletTrace->start = trace->end;

		return true;
	} else {
		float dotProduct = -(trace->normal.DotProduct(&bulletTrace->dir));
		if (dotProduct >= 0.125f) {
			bulletTrace->start[0] = trace->end[0] + bulletTrace->dir[0] * (factor / dotProduct);
			bulletTrace->start[1] = trace->end[1] + bulletTrace->dir[1] * (factor / dotProduct);
			bulletTrace->start[2] = trace->end[2] + bulletTrace->dir[2] * (factor / dotProduct);

			return true;
		} else {
			bulletTrace->start[0] = trace->end[0] + bulletTrace->dir[0] * (factor * 8.0f);
			bulletTrace->start[1] = trace->end[1] + bulletTrace->dir[1] * (factor * 8.0f);
			bulletTrace->start[2] = trace->end[2] + bulletTrace->dir[2] * (factor * 8.0f);

			return false;
		}
	}
}

float CalculatePower(Entity *ent, FVector3 *start, FVector3 *end) {
	BulletFireParams	bulletTrace;
	BulletFireParams	bulletTrace_2;
	trace_t		trace_2;
	trace_t		trace;
	FVector3	traceEndpos;
	bool hit = false;
	bool hit2 = false;
	bool v12 = false;
	int hitCount = 0;

	weapon_t *weapon = GetWeaponByIndex(cg_entities[cg->clientNum].nextState.weapon);

	memset(&bulletTrace, 0x00, sizeof(BulletFireParams));
	bulletTrace.weaponEntIndex = 1022;
	bulletTrace.ignoreEntIndex = cg->clientNum;
	bulletTrace.damageMultiplier = 1.0f;
	bulletTrace.methodOfDeath = 1;
	bulletTrace.origStart = bulletTrace.start = *start;
	bulletTrace.end = *end;

	FVector3 delta;
	delta[0] = bulletTrace.end[0] - bulletTrace.start[0];
	delta[1] = bulletTrace.end[1] - bulletTrace.start[1];
	delta[2] = bulletTrace.end[2] - bulletTrace.start[2];

	delta.CalculateAngles();
	delta.AngleVectors(&bulletTrace.dir, NULL, NULL);

	hit = TraceBullet(&trace, &bulletTrace, &cg_entities[cg->clientNum], 0);

	if (!hit) {
		return bulletTrace.damageMultiplier;//1.0f;
	}

	if (trace.startsolid) {
		return -1.0f;
	}

	while (bulletTrace.damageMultiplier > 0.0f) {
		float penetrationPower1 = GetPenetrationPower(weapon, trace.materialType);

		if (penetrationPower1 <= 0.0f) {
			break;
		}

		//traceEndpos = *(FVector3*)((&trace) + 0x30);
		traceEndpos.x = bulletTrace.start.x + ((bulletTrace.end.x - bulletTrace.start.x) * trace.fraction);
		traceEndpos.y = bulletTrace.start.y + ((bulletTrace.end.y - bulletTrace.start.y) * trace.fraction);
		traceEndpos.z = bulletTrace.start.z + ((bulletTrace.end.z - bulletTrace.start.z) * trace.fraction);

		if (!StepForward(&bulletTrace, &trace, 0.135000005364418)) {
			break;
		}

		hit = TraceBullet(&trace, &bulletTrace, &cg_entities[cg->clientNum], trace.materialType);

		memcpy(&bulletTrace_2, &bulletTrace, sizeof(BulletFireParams));

		bulletTrace_2.dir[0] = bulletTrace.dir[0] * -1.0f;
		bulletTrace_2.dir[1] = bulletTrace.dir[1] * -1.0f;
		bulletTrace_2.dir[2] = bulletTrace.dir[2] * -1.0f;

		bulletTrace_2.start = bulletTrace.end;

		bulletTrace_2.end[0] = traceEndpos[0] + bulletTrace_2.dir[0] * 0.009999999776482582;
		bulletTrace_2.end[1] = traceEndpos[1] + bulletTrace_2.dir[1] * 0.009999999776482582;
		bulletTrace_2.end[2] = traceEndpos[2] + bulletTrace_2.dir[2] * 0.009999999776482582;

		memcpy(&trace_2, &trace, sizeof(trace_t));

		trace_2.normal.Scale(-1.0f);

		if (hit) {
			StepForward(&bulletTrace_2, &trace_2, 0.009999999776482582);
		}

		hit2 = TraceBullet(&trace_2, &bulletTrace_2, &cg_entities[cg->clientNum], trace_2.materialType);

		v12 = (hit2 && trace_2.allsolid || trace.startsolid && trace_2.startsolid);

		if (!hit2 && !v12) {
			if (!hit) {
				break;
			} else {
				goto LABEL_1;
			}

			continue;
		}

		float wallDepth = 0.0f;
		if (v12) {
			wallDepth = bulletTrace_2.start.Distance(&bulletTrace_2.end);
		} else {
			wallDepth = traceEndpos.Distance(&trace_2.end);
		}

		if (wallDepth < 1.0f) {
			wallDepth = 1.0f;
		}

		float penetrationPower2 = GetPenetrationPower(weapon, trace_2.materialType);

		float penetrationPower = 0.0f;
		if (hit2) {
			penetrationPower = min(penetrationPower1, penetrationPower2);
		} else {
			penetrationPower = penetrationPower1;
		}

		if (penetrationPower <= 0.0f) {
			return -1.0f;
		}

		bulletTrace.damageMultiplier -= wallDepth / penetrationPower;

	LABEL_1:
		if (!hit) {
			break;
		} else {
			if (trace.hitId != 1022 && trace.hitId != 1023) {
				return bulletTrace.damageMultiplier;
			}

			if (++hitCount >= 5) {
				break;
			}
		}
	}

	if (!hit) {
		return bulletTrace.damageMultiplier;
	} else {
		return -1.0f;
	}
}

LONG WINAPI HandleAWException(EXCEPTION_POINTERS *ex) {
	return EXCEPTION_CONTINUE_SEARCH;
}
*/
