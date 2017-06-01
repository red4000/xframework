#ifndef CHAMS_H
#define CHAMS_H

#include "drender.h"

void InitializeChams();
HRESULT WINAPI hDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount);

void HookChams();
void UnhookChams();

extern int playerChams, onlyEnemyChams, visChamStyle, invisChamStyle, wallHack, weaponChams, visWeaponChamStyle, invisWeaponChamStyle, wallChams;
extern _Color enemyVisCol, enemyInvisCol, friendlyVisCol, friendlyInvisCol, weaponVisCol, weaponInvisCol;

#endif
