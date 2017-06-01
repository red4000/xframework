#include "framework.h"
#include "nospread.h"
#include "game.h"
#include "_game.h"

int noSpread = 1, noRecoil;
double d255  = 255.0;

DWORD  dwGetRandomFloats       = 0x4A66E0;
DWORD  dwCalculateWeaponSpread = 0x416C70;
DWORD  dwTan                   = 0x68BC70;

float (__cdecl *GetRandomFloats)(int time, float *right, float *up) = (float (__cdecl *)(int, float*, float*))dwGetRandomFloats;

void FirstBulletFix() {
	if (*(float*)0x79E818 != 0.0f) {
		*(float*)0xC84F8C = *(float*)0x79E818 * *(float*)0x79D6D4;
	} else   {
		*(float*)0xC84F8C = *(float*)0x79D6D4;
	}

	*(DWORD*)0xC84FD0 |= *(DWORD*)0x84CA8C;
	*(DWORD*)0xC84F84  = *(DWORD*)0x79E6A4;
	*(DWORD*)0xC84F88  = *(DWORD*)0x79E6B4;
	*(DWORD*)0x84CA8C  = 0;
	*(float*)0xC84F98  = *(float*)0x79E754 + *(float*)0x79E760;
	*(float*)0xC84F9C  = *(float*)0x79E764 + *(float*)0x79E758;
	*(float*)0xC84FA0  = *(float*)0x79E768 + *(float*)0x79E75C;
	*(float*)0xC5B6EC  = *(float*)0x797618;
	*(float*)0xC5B6F0  = *(float*)0x79761C;
	*(float*)0xC5B6F4  = *(float*)0x797620;
}

void GetSpreadData(weapon_t *weapon, float *spread, float minDamageRange, float *right, float *up) {
	float base, max;

	// TODO: interpolate predictedPlayerState: spreadOverrideState, spreadOverride, viewHeightCurrent
	//

	__asm
	{
		mov ecx, [weapon]
		mov edx, 0x794474
		lea edi, [base]
		lea esi, [max]
		call  [dwCalculateWeaponSpread]
		fld dword ptr ds  :[0x84CAA0]
		fdiv qword ptr ds :[d255]
		fld max
		fsub base
		fmulp st(1), st
		fadd base
		fmul qword ptr ds :[0x70B2D0]
		call  [dwTan]
		fmul dword ptr ds :[minDamageRange]
		fld st
		//fstp  base
		mov eax, up
		push eax
		mov eax, right
		push eax
		mov eax, dword ptr ds :[CL_SERVERTIME]
		push eax
		mov eax, GetRandomFloats
		call eax
		add esp, 12
		mov eax, right
		fld dword ptr ds  :[eax]
		fmul
		fstp dword ptr ds :[eax]
		mov eax, up
		fld dword ptr ds  :[eax]
		fmul
		fstp dword ptr ds :[eax]
	}

	(*spread) = base;
}

void GetWeaponAngles(FVector3 *angles) {
	angles->x = *(float*)0x79E76C;
	angles->y = *(float*)0x79E770;
	angles->z = 0.0f;
}

weapon_t *GetWeaponByIndex(int i) {
	if (i < 0) {
		return NULL;
	}

	return (weapon_t*)*(DWORD*)((i * 4) + 0x736DB8);
}

void AngleVectors(FVector3 *angles, FVector3 *forward, FVector3 *right, FVector3 *up) {
	__asm
	{
		push   [up]
		mov eax, 0x563460
		mov esi, [forward]
		mov edi, [right]
		mov edx, [angles]
		call eax
		add esp, 4
		xor    eax, eax
	}
}


void GetSpreadDelta(FVector3 *delta) {
	weapon_t *weapon;
	float     spread, minDamageRange;
	FVector3  end, weaponAngles, forward, right, up;

	DWORD weaponIndex;

	weaponIndex = ((centity_t*)localEntity)->nextState.weapon;
	/*
	if (cg->predictedPlayerState.weapFlags & 2) {
		weaponIndex = cg->predictedPlayerState.offHandIndex;
	} else   {
		weaponIndex = cg->predictedPlayerState.weapon;
	}
	*/

	weapon = GetWeaponByIndex(weaponIndex);

	__asm
	{
		mov eax, weapon
		cmp dword ptr [eax + 0x130], 3
		jne ld81922
		fld dword ptr [eax + 0x80C]
		jmp store2
ld81922:        fld dword ptr ds :[0x70BD00]
		store2 :         fstp minDamageRange
	}

	float fRight, fUp;

	GetWeaponAngles(&weaponAngles);
	weaponAngles.Normalize360();
	AngleVectors(&weaponAngles, &forward, &right, &up);
	GetSpreadData(weapon, &spread, minDamageRange, &fRight, &fUp);

	__asm
	{
		push eax
		push edi

		lea eax, forward.x
		mov edi, delta

		fld dword ptr [eax]
		fmul minDamageRange
		fstp dword ptr [edi]

		fld dword ptr [eax + 4]
		fmul minDamageRange
		fstp dword ptr [edi + 4]

		fld minDamageRange
		fmul dword ptr [eax + 8]
		fstp dword ptr [edi + 8]

		lea eax, right.x
		fld dword ptr [eax]
		fmul fRight
		fadd dword ptr [edi]
		fstp dword ptr [edi]

		fld dword ptr [eax + 4]
		fmul fRight
		fadd dword ptr [edi + 4]
		fstp dword ptr [edi + 4]

		fld dword ptr [eax + 8]
		fmul fRight
		fadd dword ptr [edi + 8]
		fstp dword ptr [edi + 8]

		lea eax, up.x
		fld dword ptr [eax]
		fmul fUp
		fadd dword ptr [edi]
		fstp dword ptr [edi]

		fld dword ptr [eax + 4]
		fmul fUp
		fadd dword ptr [edi + 4]
		fstp dword ptr [edi + 4]

		fld dword ptr [eax + 8]
		fmul fUp
		fadd dword ptr [edi + 8]
		fstp dword ptr [edi + 8]

		pop edi
		pop eax
	}
}

bool GetSpreadOffset(FVector3 *delta, TVec3 *angles) {
	FVector3  weaponAngles;

	if (1 == noSpread && 1.0f > (*(float*)0x794568)) {
		GetWeaponAngles(&weaponAngles);
		weaponAngles.Normalize360();

		TVec3 tAngles;
		FVec3ToTVec3(&delta->x, &tAngles);
		TVec3Angles(&tAngles.x, &tAngles.y, &tAngles.z);
		TVec3Normalize360(&tAngles);
		FVec3ToTVec3(&weaponAngles[0], angles);
		TVec3Subtract(angles, &tAngles);
		TVec3Normalize180(angles);
		return true;
	}

	return false;
}
