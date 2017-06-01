#ifndef CHAMS_H
#define CHAMS_H

#include "drender.h"

void InitializeChams();
HRESULT __fastcall hDrawIndexedPrimitive(void *thisptr, int edx, LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount);

extern _Color enemyVisCol, enemyInvisCol, friendlyVisCol, friendlyInvisCol;

#endif
