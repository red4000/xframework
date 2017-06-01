#ifndef GAME_H
#define GAME_H

#include "vmath.h"
#include "_vector.h"

#define MAX_PLAYERS  64
#define MAX_ENTITIES (1 << 10)

enum {
	TAG_HEAD,
	TAG_NECK,
	TAG_HELMET,
	TAG_EYE,
	TAG_UPPERSPINE,
	TAG_LOWERSPINE,
	TAG_LSHOULDER,
	TAG_RSHOULDER,
	TAG_LHIP,
	TAG_RHIP,
	TAG_LELBOW,
	TAG_RELBOW,
	TAG_LKNEE,
	TAG_RKNEE,
	TAG_LANKLE,
	TAG_RANKLE,
	TAG_LWRIST,
	TAG_RWRIST,
	TAG_PELVIS,
	MAX_TAG
};

enum {
	ET_HUMAN     = 1,
	ET_WEAPON    = 3,
	ET_EXPLOSIVE = 4
};

#define T_VISIBLE 1

struct CachedTag {
	int      flags;
	FVector3 pos;
};

struct PlayerInfo {
	FVector3 origin, head;
	RECT     rect;
	bool     visible;
	float    threat, fov;
};

class Entity {
public:
	int IsAlive();
	int GetTeam();
	bool IsHostile();
	bool IsLocalEntity();
	int GetType();
	int GetIndex();
	char *GetName();
	char *GetWeaponName();
	void GetAngles(FVector3 *res);
	void GetTagPosition(int id, FVector3 *res);
	void GetOrigin(FVector3 *res);
};

struct EntityInfo {
	Entity  *ent;
	int      type;
	FVector3 origin;
	int      x, y;
};

extern PlayerInfo               playerInfo[MAX_PLAYERS];
extern _Vector<EntityInfo, 128> entityInfo;

Entity *GetEntityByIndex(int i);
void GetLocalViewOrigin(FVector3 *result);
bool WorldToScreen(FVector3 *position, int *x, int *y);
bool IsPointVisible(FVector3 *start, FVector3 *end);
bool IsInGame();
bool CanFire();
void SetViewAngles(FVector3 *newAngles);
void GetRecoil(DVector3 *res);
void EnableMouseUnlock();
void DisableMouseUnlock();
void UpdateCursorPosition();
void InitializeGame(bool frame = false);

extern int (__cdecl *Com_Frame)();
extern Entity  *localEntity;
extern FVector3 viewOrigin, viewAngles;

#endif
