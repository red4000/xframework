#ifndef NOSPREAD_H
#define NOSPREAD_H

#define CL_SERVERTIME 0xC628D0
#include "tzar.h"

typedef void *weapon_t;

void FirstBulletFix();
weapon_t *GetWeaponByIndex(int i);
void GetSpreadDelta(FVector3 *delta);
bool GetSpreadOffset(FVector3 *delta, TVec3 *res);

extern int noSpread, noRecoil;

#endif
