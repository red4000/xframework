#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "game.h"
#include "_game.h"
#include "aim.h"
#include "aimbot.h"
#include "nospread.h"
#include "autowall.h"
#include "prediction.h"
#include "rcs.h"

//#define SETUCMDTIME

float       aimFOV          = 360,
			aimZAdjust      = 1.86f,
			localPrediction = 0;

int         aimbot           = 0,
			autoFire         = 0,
			aimBone          = TAG_HEAD,
			aimAtFirstTarget = 0,
			targetLock,
			aimSort,
			fireStartDelay,
			fireDelay = 100,
			silentAim,
			aimKeyEnabled,
			aimKey;

DWORD       fireTime, lastFireTime;
float       targetFitness;
DVector3    targetPosition;
TVec3       targetAngles;
Entity     *targetEnt;
PlayerInfo *targetPi;

FVector3 headScalar(1.f, 1.f, 1.f),
		 helmetScalar(0.f, 0.f, 0.f),
		 neckScalar(0.f, 0.f, 0.f),
		 eyeScalar(0.f, 0.f, 0.f);

int c1, c2, c3, c4, c5;

/*
oid __usercall sub_1000E1A0(int a1@<edi>, int a2@<esi>)
{
  int v2; // xmm0_4@1
  int v5; // ST14_4@2
  float v6; // xmm1_4@4
  int v8; // ST14_4@6
  __m128 v9; // xmm0@7
  __m128d v10; // xmm0@8
  signed int v11; // xmm0_4@8
  double v12; // ST08_8@10
  float v13; // ST14_4@10
  __m128d v14; // xmm0@11
  float v15; // [sp+4h] [bp-14h]@3
  float v16; // [sp+8h] [bp-10h]@1
  float v17; // [sp+14h] [bp-4h]@10

  v2 = 0;
  __asm { lahf }
  v16 = *(float *)(a2 + 4);
  if ( __SETP__(_AH & 0x44, 0) )
  {
	__asm { lahf }
	v8 = *(_DWORD *)a2;
	if ( !__SETP__(_AH & 0x44, 0) )
	{
	  if ( *(float *)(a2 + 4) <= 0.0 )
		v11 = 1132920832;
	  else
		v11 = 1119092736;
	  goto LABEL_9;
	}
  }
  else
  {
	__asm { lahf }
	v5 = *(_DWORD *)a2;
	if ( !__SETP__(_AH & 0x44, 0) )
	{
	  v15 = 0.0;
	  if ( *(float *)(a2 + 8) <= 0.0 )
		v6 = 270.0;
	  else
		v6 = 90.0;
	  goto LABEL_17;
	}
  }
  sub_1001D76A(v16);
  v15 = v16 * 57.2957763671875;
  v9 = (__m128)LODWORD(v15);
  if ( v15 < 0.0 )
  {
	v10 = _mm_cvtps_pd(v9);
	v10.m128d_f64[0] = v10.m128d_f64[0] + 360.0;
	v11 = (unsigned __int128)_mm_cvtpd_ps(v10);
LABEL_9:
	v15 = *(float *)&v11;
  }
  v12 = *(float *)(a2 + 8);
  sub_10002440();
  sub_1001D76A(v12);
  v13 = v12;
  v17 = v13 * 57.2957763671875;
  if ( v17 >= 0.0 )
  {
	v6 = v17;
  }
  else
  {
	v14 = _mm_cvtps_pd((__m128)LODWORD(v17));
	v14.m128d_f64[0] = v14.m128d_f64[0] + 360.0;
	LODWORD(v6) = (unsigned __int128)_mm_cvtpd_ps(v14);
  }
  v2 = 0;
LABEL_17:
  *(float *)a1 = COERCE_FLOAT(2147483648) - v6;
  *(float *)(a1 + 4) = v15;
  *(_DWORD *)(a1 + 8) = v2;
}
*/

void InjectCmd() {
	c1 = c2 = c3 = c4 = c5 = 0;
	
	InitializeGame();
	if (!localEntity || !localEntity->IsAlive() || !IsInGame()) {
		return;
	}

	c1 = 1;

	FirstBulletFix();

	if (CanFire()) {
		c2 = 1;
		int        currentCmdNumber = *(int*)0xCC5FF8;
		usercmd_s *cmds             = (usercmd_s*)0xCC4FF8;
	
		TVec3 aimAngles, spreadAngles;

		usercmd_s *cmd = &cmds[currentCmdNumber & 0x7F], *lastCmd = &cmds[(currentCmdNumber - 1) & 0x7F];
	
		if (cmd->serverTime != (lastCmd->serverTime + 1)) {
			c3 = 1;
			//InitializeGame();

			/*
			viewAngles[0] = (float)SHORT2ANGLE(cmd->angles[0]);
			viewAngles[1] = (float)SHORT2ANGLE(cmd->angles[1]);
			viewAngles[2] = 0.f;
			*/
			
			FVector3 delta;
			GetSpreadDelta(&delta);

			/*FVector3 angles;
			angles[0] = (float)SHORT2ANGLE(cmd->angles[0]);
			angles[1] = (float)SHORT2ANGLE(cmd->angles[1]);
			angles[2] = (float)SHORT2ANGLE(cmd->angles[2]);
			FVec3ToTVec3(&angles[0], &tAngles);*/
			DVector3 angles;
			angles[0] = (double)SHORT2ANGLE(cmd->angles[0]);
			angles[1] = (double)SHORT2ANGLE(cmd->angles[1]);
			//angles[2] = (double)SHORT2ANGLE(cmd->angles[2]);
			angles[2] = 0;

			ChangeFPUPrecision(64);
			bool gotAimAngles = GetAimbotAngles(&aimAngles);

			if (gotAimAngles) {
				c5 = 1;
			}

			if (true == gotAimAngles || (cmd->buttons & 1)) {
				c4 = 1;
				bool gotSpreadAngles = GetSpreadOffset(&delta, &spreadAngles);
				(*(int*)0xCC5FF8)++;
				currentCmdNumber++;
				lastCmd = cmd;
				cmd     = &cmds[currentCmdNumber & 0x7F];

				memcpy(cmd, lastCmd, sizeof(usercmd_s));
#ifdef SETUCMDTIME
				lastCmd->serverTime = cmd->serverTime = *(int*)CL_SERVERTIME;
#endif
				lastCmd->buttons &= ~1;
				TVec3 tAngles;
				DVec3ToTVec3(&angles[0], &tAngles);
				
				if (gotAimAngles) {
					if (autoFire) {
						if (0 == fireStartDelay) {
							cmd->buttons |= 1;
						} else {
							DWORD currentTime = GetTickCount();
							if ((currentTime - fireTime) >= fireStartDelay) {
								if ((currentTime - lastFireTime) >= fireDelay) {
									cmd->buttons |= 1;
									fireTime      = lastFireTime = currentTime;
								}
							}
						}
					}

					TVec3Add(&tAngles, &aimAngles);
					TVec3Normalize180(&tAngles);

					if (!silentAim) {
						TVec3ToDVec3(&tAngles, &angles[0]);
						cmd->angles[0] = ANGLE2SHORT(angles[0]);
						cmd->angles[1] = ANGLE2SHORT(angles[1]);
						cmd->angles[2] = 0;
					}
				}

				if (gotSpreadAngles) {
					TVec3Add(&tAngles, &spreadAngles);
					TVec3Normalize180(&tAngles);
				}
				TVec3ToDVec3(&tAngles, &angles[0]);
				
				lastCmd->serverTime--;
				//lastCmd->serverTime = (*(int*)CL_SERVERTIME) + 1;

				lastCmd->angles[0] = ANGLE2SHORT(angles[0]);
				lastCmd->angles[1] = ANGLE2SHORT(angles[1]);
				lastCmd->angles[2] = 0;
			}
			ChangeFPUPrecision(24);
		}
	}
}

bool AimbotActive() {
	if (aimKeyEnabled) {
		if (0 == (GetAsyncKeyState(aimKey) & 0x8000)) {
			return false;
		}
		return true;
	}
	return true;
}

bool GetHeadPos(Entity *ent, PlayerInfo *i, DVector3 *res) {
	res->x = headScalar.x * i->head.x;
	res->y = headScalar.y * i->head.y;
	res->z = headScalar.z * i->head.z;

	FVector3 bonePos;
	if (helmetScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_HELMET, &bonePos);
		res->x += helmetScalar.x * bonePos.x;
		res->y += helmetScalar.y * bonePos.y;
		res->z += helmetScalar.z * bonePos.z;
	}
	
	if (eyeScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_EYE, &bonePos);
		res->x += eyeScalar.x * bonePos.x;
		res->y += eyeScalar.y * bonePos.y;
		res->z += eyeScalar.z * bonePos.z;
	}
	
	if (neckScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_NECK, &bonePos);
		res->x += neckScalar.x * bonePos.x;
		res->y += neckScalar.y * bonePos.y;
		res->z += neckScalar.z * bonePos.z;
	}

	return true;
}

bool GetHeadTPos(Entity *ent, PlayerInfo *i, TVec3 *res) {
	TVec3 pos, scale;
	i->head.z += aimZAdjust;
	FVec3ToTVec3(&i->head[0], res);
	i->head.z -= aimZAdjust;
	FVec3ToTVec3(&headScalar[0], &scale);
	TVec3Multiply(res, &scale);

	FVector3 bonePos;
	if (helmetScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_HELMET, &bonePos);
		FVec3ToTVec3(&bonePos[0], &pos);
		FVec3ToTVec3(&helmetScalar[0], &scale);
		TVec3Multiply(&pos, &scale);
		TVec3Add(res, &pos);
	}
	
	if (eyeScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_EYE, &bonePos);
		FVec3ToTVec3(&bonePos[0], &pos);
		FVec3ToTVec3(&eyeScalar[0], &scale);
		TVec3Multiply(&pos, &scale);
		TVec3Add(res, &pos);
	}
	
	if (neckScalar.Sum() > 0.f) {
		ent->GetTagPosition(TAG_NECK, &bonePos);
		FVec3ToTVec3(&bonePos[0], &pos);
		FVec3ToTVec3(&neckScalar[0], &scale);
		TVec3Multiply(&pos, &scale);
		TVec3Add(res, &pos);
	}

	return true;
}

bool GetTargetAimPoint(Entity *ent, PlayerInfo *i, DVector3 *res) {
	FVector3 aimPoint;
	if (aimBone == TAG_HEAD) {
		GetHeadPos(ent, i, res);
		res->z += aimZAdjust;
	} else {
		ent->GetTagPosition(aimBone, &aimPoint);
		res->x = aimPoint.x; res->y = aimPoint.y; res->z = aimPoint.z + aimZAdjust;
	}
	return true;
}

void AimbotPreFrame() {
	if (!targetLock) {
		dropTarget:
		targetFitness = 99999999;
		targetEnt     = NULL;
	} else {
		DVector3 newAimPoint, vel;
		if (NULL == targetEnt || !targetEnt->IsAlive() || !targetEnt->IsHostile() || !GetTargetAimPoint(targetEnt, targetPi, &newAimPoint))  {
			goto dropTarget;
		} else {
			FVector3 fNewAimPoint;
			fNewAimPoint[0] = newAimPoint[0]; fNewAimPoint[1] = newAimPoint[1]; fNewAimPoint[2] = newAimPoint[2];
			if (IsInFOV(&viewOrigin, &fNewAimPoint, aimFOV)) {
				if (IsPointVisible(&viewOrigin, &fNewAimPoint)) {
					goto lockTarget;
				} else {
					if (autoWall) {
						float power = CalculatePower(targetEnt, &viewOrigin, &fNewAimPoint);
						if (power >= autoWallThreshold) {
							lockTarget:
							targetFitness  = 0;
							targetPosition = newAimPoint;
							goto locked;
						} else {
							goto dropTarget;
						}
					} else {
						goto dropTarget;
					}
				}
			} else {
				goto dropTarget;
			}
		}
	}
	locked:;
}

float GetTargetFitness(Entity *ent, PlayerInfo *i) {
	float fitness = 0.f;
	switch (aimSort)
	{
	default:
	case 0:
		fitness = i->threat;
		break;

	case 1:
		fitness = viewOrigin.Distance(&i->head);
		break;

	case 2:
		fitness = i->fov;
		break;
	}
	fitness += 1.0f;
	return fitness;
}

void AimAtEntityFrame(Entity *ent, PlayerInfo *i) {
	bool  damagable;
	float power;
	DVector3 dAimPoint;
	FVector3 aimPoint;

	if (!aimAtFirstTarget || (aimAtFirstTarget && targetEnt == NULL)) {
		GetTargetAimPoint(ent, i, &dAimPoint);
		aimPoint[0] = dAimPoint[0]; aimPoint[1] = dAimPoint[1]; aimPoint[2] = dAimPoint[2];
		FVector3 unpredictedAimPoint = aimPoint;

		if (!i->visible) {
			if (autoWall) {
				power = CalculatePower(ent, &viewOrigin, &aimPoint);
				if (power >= autoWallThreshold) {
					goto _aim;
				}
			}
		} else {
		_aim:
			i->fov = GetFOV(&viewOrigin, &aimPoint);
			float fitness = GetTargetFitness(ent, i);
			if (fitness < targetFitness) {
				if (i->fov <= aimFOV || aimFOV == 360.f) {
					targetFitness  = fitness;
					targetEnt      = ent;
					targetPi       = i;
					//targetPosition = pos;
					targetPosition[0] = unpredictedAimPoint[0]; targetPosition[1] = unpredictedAimPoint[1]; targetPosition[2] = unpredictedAimPoint[2];
				}
			}
		}
	}
}

void AimbotPostFrame() {
	DVector3 angles;
	if (rcsEnabled) {
		if (GetRCSAngle(&angles)) {
			FVector3 fAngles;
			fAngles[0] = angles[0]; fAngles[1] = angles[1]; fAngles[2] = 0.f;
			SetViewAngles(&fAngles);
		}
	}
	if (aimbot && 0 == silentAim) {
		ChangeFPUPrecision(64);
		if (GetAimbotFrameAngles(&angles)) {
			FVector3 fAngles;
			fAngles[0] = angles[0]; fAngles[1] = angles[1]; fAngles[2] = 0.f;
			SetViewAngles(&fAngles);
			/*if (autoFire) {
				void (__cdecl *ConsoleCommand)(int x, int x2, char *cvar) = (void (__cdecl *)(int, int, char *))0x4F9AB0;
				ConsoleCommand(0, 0, "+attack");
				ConsoleCommand(0, 0, "-attack");
			}*/
		}
		ChangeFPUPrecision(24);
	}
}

bool GetAimbotAngles(TVec3 *res) {
	if (aimbot && targetEnt != NULL) {
		TVec3 tViewOrigin;
		if (TAG_HEAD == aimBone) {
			GetHeadTPos(targetEnt, targetPi, &targetAngles);
		} else {
			DVec3ToTVec3(&targetPosition[0], &targetAngles);
		}

		CalculateEntityTVelocity(targetEnt, &tViewOrigin);
		TVec3Add(&targetAngles, &tViewOrigin);

		if (localPrediction > 0.f) {
			CalculateEntityTVelocity(localEntity, &tViewOrigin);
			TVec3MultiplyTF(&tViewOrigin, localPrediction);
			TVec3Subtract(&targetAngles, &tViewOrigin);
		}

		FVec3ToTVec3(&viewOrigin[0], &tViewOrigin);
		TVec3Subtract(&targetAngles, &tViewOrigin);
		TVec3Angles(&targetAngles.x, &targetAngles.y, &targetAngles.z);
		TVec3Normalize360(&targetAngles);
		FVec3ToTVec3(&viewAngles[0], &tViewOrigin);
		TVec3Subtract(&targetAngles, &tViewOrigin);
		TVec3Normalize180(&targetAngles);
		*res = targetAngles;
		return true;
	}

	return false;
}
