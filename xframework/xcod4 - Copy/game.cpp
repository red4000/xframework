#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "game.h"
#include "_game.h"
#include "esp.h"
#include "aimbot.h"
#include "nospread.h"
#include "aim.h"
#include "prediction.h"
#include "warning.h"

#define ENTITY ((centity_t*)this)
#define CLIENT (&clientinfo[ENTITY->nextState.number])

clientInfo_t *clientinfo  = (clientInfo_t*)0x839270;
cg_t         *cg          = (cg_t*)        0x74E338;
refdef_t     *refdef      = (refdef_t*)    0x797600;
centity_t    *cg_entities = (centity_t*)   0x84F2D8;

size_t pRegisterTag   = 0x518290,
	   pWorldToScreen = 0x42D6D0,
	   pGetPlayerTag  = 0x4024B0,
	   pGetViewOrigin = 0x456580,
	   pTrace         = 0x459EF0;

int (__cdecl *Com_Frame)() = (int (__cdecl *)())0x4FFF30;

FVector3 zero(0, 0, 0), viewOrigin, viewAngles;
short    registeredTag[MAX_TAG];
DWORD    white;
Entity  *localEntity;

PlayerInfo               playerInfo[MAX_PLAYERS];
_Vector<EntityInfo, 128> entityInfo;

char *tagName[] = {
	"j_head",        // 0
	"j_neck",        // 1
	"j_helmet",      // 2
	"tag_eye",       // 3
	"j_spineupper",  // 4
	"j_spinelower",  // 5
	"j_shoulder_le", // 6
	"j_shoulder_ri", // 7
	"j_hip_le",      // 8
	"j_hip_ri",      // 9
	"j_elbow_le",    // 10
	"j_elbow_ri",    // 11
	"j_knee_le",     // 12
	"j_knee_ri",     // 13
	"j_ankle_le",    // 14
	"j_ankle_ri",    // 15
	"j_wrist_le",    // 16
	"j_wrist_ri",    // 17
	"pelvis"         // 18

	//"tag_weapon"     // 19
};

int Entity::IsAlive() {
	return ENTITY->lightingHandle && ENTITY->tree;
}

int Entity::GetTeam() {
	return CLIENT->team;
}

bool Entity::IsHostile() {
	int team = GetTeam();
	if (team == 0) {
		return true;
	}
	return team != localEntity->GetTeam();
}

bool Entity::IsLocalEntity() {
	return GetIndex() == localEntity->GetIndex();
}

int Entity::GetType() {
	return ENTITY->nextState.eType;
}

int Entity::GetIndex() {
	return ENTITY->nextState.number;
}

char *Entity::GetName() {
	return CLIENT->name;
}

char *Entity::GetWeaponName() {
	int weaponNumber;

	if (GetIndex() < MAX_PLAYERS) {
		weaponNumber = *MakePtr(int*, CLIENT, 0x4AC);
	} else {
		weaponNumber = *MakePtr(int*, ENTITY, 0x190);

		if (0 == weaponNumber) {
			weaponNumber = (*MakePtr(int*, ENTITY, 0x154)) % 128;
		}
	}

	if (0 != weaponNumber) {
		weapon_t *weapon = GetWeaponByIndex(weaponNumber);

		if (NULL != weapon) {
			return *MakePtr(char**, (DWORD)0x748658 + (weaponNumber * 0x44), 56);
		}
	}

	return "Unknown";
}

void Entity::GetAngles(FVector3 *res) {
	//(*res) = ENTITY->angles;
	(*res) = ENTITY->nextState.lerp.apos.trBase;
	//(*res) = *MakePtr(FVector3*, CLIENT, 0x3E4);
}

void Entity::GetTagPosition(int id, FVector3 *res) {
	short tag = registeredTag[id];

	__asm {
		mov   ecx, this
		movzx eax, byte ptr [ecx + 4]
		movzx esi, tag
		push  res
		call  pGetPlayerTag
		add   esp, 4
	}
}

void Entity::GetOrigin(FVector3 *res) {
	(*res) = ENTITY->origin;
}

Entity *GetEntityByIndex(int i) {
	return (Entity*)&cg_entities[i];
}

short RegisterTag(char *name) {
	size_t len = strlen(name) + 1;

	__asm {
		push len
		push 1
		push name
		call pRegisterTag
		add  esp, 12
	}
}

bool WorldToScreen(FVector3 *position, int *x, int *y) {
	bool res;
	__asm {
		mov  edx, position
		push y
		push x
		xor  eax, eax
		call pWorldToScreen
		add  esp, 8
		mov res, al
	}
	(*x) = ((int)*(float*)x);
	(*y) = ((int)*(float*)y);
	return res;
}

void GetLocalViewOrigin(FVector3 *result) {
	int ocg_time = *(int*)0x794460;
	*(int*)0x794460 = *(int*)CL_SERVERTIME;
	__asm {
		push result
		mov  eax, 0x794474
		//mov   edx, [eax]
		mov  edx, dword ptr ds:[CL_SERVERTIME]
		call pGetViewOrigin
		add  esp, 4
	}
	*(int*)0x794460 = ocg_time;
}

void Trace(FVector3 *start, trace_t *trace, FVector3 *point, FVector3 *null, int num, int flags) {
	__asm {
		xor  eax, eax
		push eax
		mov  edi, trace
		push eax
		push flags
		mov  eax, null
		push num
		push point
		push null
		push start
		call pTrace
		add  esp, 0x1C
	}
}

bool IsPointVisible(FVector3 *start, FVector3 *end) {
	trace_t trace;

	memset((void*)&trace, 0, sizeof(trace_t));

	Trace(start, &trace, end, &zero, cg->clientNum, 0x080B001);

	if (trace.fraction >= 1.0f) {
		return true;
	}

	return false;
}

bool IsInGame() {
	return NULL != cg->snap && NULL != cg->nextSnap;
}

int GetAmmoInClip() {
	DWORD weapon     = *(DWORD*)((((centity_t*)localEntity)->nextState.weapon * 4) + 0x736DB8);
	DWORD weaponInfo = *(DWORD*)(weapon + 0x330);

	return (*(DWORD*)((DWORD)cg->nextSnap + 0x368 + (weaponInfo * 4)));
}

bool CanFire() {
	/*
	if (cvar_AimKey->iValue != 0) {
		bool pressed = GetAsyncKeyState(cvar_AimKey->iValue) & 1;

		if (!pressed) {
			return false;
		}
	}*/

	if (GetAmmoInClip()) {
		/*
		weapon_t *pwep = GetWeaponByIndex(((centity_t*)localEntity)->nextState.weapon);

		if (pwep) {
			int deltaTime = (*(int*)(pwep + 0x36C)) - ((playerState_s*)0x794474)->weaponTime;

			if (deltaTime < 0) {
				return false;
			}
		}
		*/

		/*
		if ((cvar_NoAimProne->iValue == 1) && (pLocalEnt->State & FLAGS_PRONE)) {
			return false;
		}

		if (cvar_NoAimExplosive->iValue == 1) {
			if (pwep && pwep->displayName) {
				if (bIsExplosive(pwep->displayName)) {
					return false;
				}
			}
		}

		if ((cvar_NoAimClimbing->iValue == 1) && (pLocalEnt->State & FLAGS_CLIMBING)) {
			return false;
		}
		*/

		return AimbotActive();
   }

   return false;
}

void SetViewAngles(FVector3 *newAngles) {
	FVector3 to = *newAngles;

	if (to.x != 0.0f || to.y != 0.0f) {
		FVector3 *cl_viewangles = (FVector3*)0xC84FD8; // TODO: not use cl_viewangles?

		viewAngles.x = cl_viewangles->x = NormalizeAngle180(cl_viewangles->x + to.x);
		viewAngles.y = cl_viewangles->y = NormalizeAngle180(cl_viewangles->y + to.y);
	}
}

void GetRecoil(DVector3 *res) {
	res->x = *(float*)0x79E78C;
	res->y = *(float*)0x79E790;
	res->z = 0.f;
}

DWORD unlockedMouse, mouseUnlockRestore;
int mouseClicked;

void EnableMouseUnlock() {
	mouseUnlockRestore = *(DWORD*)0xCAEE218;
	//*(DWORD*)0xCC147D1 = 0;
	//CAEFAA0 = 0
	//*(DWORD*)0xCAEE218 = 1;
	unlockedMouse      = 1;
}

void DisableMouseUnlock() {
	//*(DWORD*)0xCAEE218 = mouseUnlockRestore;
	unlockedMouse      = 0;
}

void UpdateCursorPosition() {
	int nmx = *(float*)0xCAEE210;
	int nmy = *(float*)0xCAEE214;
	
	if (mx != nmx || my != nmy) {
		HandleGUIInput(gameHwnd, WM_MOUSEMOVE, 0, nmx | (nmy << 16));
	}
	SHORT ks = GetAsyncKeyState(VK_LBUTTON);
	if (mouseClicked == 0) {
		if (ks & 0x8000) {
			//HandleGUIInput(gameHwnd, WM_LBUTTONDOWN, 0, 0);
			mouseClicked = 1;
		}
	} else {
		if (ks == 0) {
			//HandleGUIInput(gameHwnd, WM_LBUTTONUP, 0, 0);
			mouseClicked = 0;
		}
	}
}

void InitializeGame(bool frame) {
	if (!IsInGame()) {
		localEntity = NULL;
		return;
	}

	DWORD nWhite = *(DWORD*)0x84CB30;
	if (white == nWhite) {
		//no update
	} else {
		for (size_t i = TAG_HEAD; i < MAX_TAG; i++) {
			registeredTag[i] = RegisterTag(tagName[i]);
		}
		white = nWhite;
	}

	PredictionBegin();
	GetLocalViewOrigin(&viewOrigin);

	localEntity = (Entity*)0x7973D8;
	viewAngles  = *(FVector3*)(((unsigned int)cg) + 0x4D360);
	//viewAngles.x = *(float*)0xC84FD8;
	//viewAngles.y = *(float*)(0xC84FD8 + 4);
	viewAngles.z = 0;
	AimbotPreFrame();

	if (frame) {
		/*if (menuOpen) {
			if (!unlockedMouse) {
				EnableMouseUnlock();
			}
		} else {
			if (unlockedMouse) {
				DisableMouseUnlock();
			}
		}*/
		EspPreFrame();
		WarningPreFrame();
		/*entityInfo.clear();
		for (size_t i = MAX_PLAYERS; i < MAX_ENTITIES; i++) {
			Entity *ent = GetEntityByIndex(i);
			if (ent->IsAlive()) {
				EntityInfo *ei = entityInfo.push_new();
				ei->type   = ent->GetType();
				ent->GetOrigin(&ei->origin);
				if (WorldToScreen(&ei->origin, &ei->x, &ei->y)) {
				} else {
					ei->x = -1;
				}
			}
		}*/

		if (noRecoil) {
			*(float*)0x79E748 = 0;
			*(float*)0x79E74C = 0;
			*(float*)0x79E750 = 0;
		}

		if (silentAim == 0 && CanFire()) {
			if (targetEnt == NULL) {
				for (size_t i = 0; i < MAX_PLAYERS; i++) {
					Entity *ent = GetEntityByIndex(i);
					if (ent->IsAlive()) {
						PlayerInfo *pi = &playerInfo[i];
						ent->GetOrigin(&pi->origin);
						ent->GetTagPosition(TAG_HEAD, &pi->head);
						pi->visible = IsPointVisible(&viewOrigin, &pi->head);
						pi->head.z += 8.f;
						CalcPlayerRect(ent, pi);
						pi->head.z -= 8.f;
						if (ent->IsHostile()) {
							pi->threat = GetEntityThreat(ent, pi);
							AimAtEntityFrame(ent, pi);
						}
					}
				}
			} else {
				for (size_t i = 0; i < MAX_PLAYERS; i++) {
					Entity *ent = GetEntityByIndex(i);
					if (ent->IsAlive()) {
						PlayerInfo *pi = &playerInfo[i];
						ent->GetOrigin(&pi->origin);
						ent->GetTagPosition(TAG_HEAD, &pi->head);
						pi->visible = IsPointVisible(&viewOrigin, &pi->head);
						pi->head.z += 8.f;
						CalcPlayerRect(ent, pi);
						pi->head.z -= 8.f;
						if (ent->IsHostile()) {
							pi->threat = GetEntityThreat(ent, pi);
						}
					}
				}
			}
		} else {
			for (size_t i = 0; i < MAX_PLAYERS; i++) {
				Entity *ent = GetEntityByIndex(i);
				if (ent->IsAlive()) {
					PlayerInfo *pi = &playerInfo[i];
					ent->GetOrigin(&pi->origin);
					ent->GetTagPosition(TAG_HEAD, &pi->head);
					pi->visible = IsPointVisible(&viewOrigin, &pi->head);
					pi->head.z += 8.f;
					CalcPlayerRect(ent, pi);
					pi->head.z -= 8.f;
					if (ent->IsHostile()) {
						pi->threat = GetEntityThreat(ent, pi);
					}
				}
			}
		}
	} else {
		//if (targetEnt == NULL) {
			for (size_t i = 0; i < MAX_PLAYERS; i++) {
				Entity *ent = GetEntityByIndex(i);
				if (ent->IsAlive()) {
					if (ent->IsHostile()) {
						PlayerInfo *pi = &playerInfo[i];
						ent->GetOrigin(&pi->origin);
						ent->GetTagPosition(TAG_HEAD, &pi->head);
						pi->visible = IsPointVisible(&viewOrigin, &pi->head);
						pi->threat = GetEntityThreat(ent, pi);
						AimAtEntityFrame(ent, pi);
					}
				}
			}
		//}
	}
	PredictionEnd();
}
