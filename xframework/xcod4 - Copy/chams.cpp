#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "chams.h"
#include "render.h"

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
HRESULT GeneratePixelShader(IDirect3DDevice9 *pD3Ddev, IDirect3DPixelShader9 **pShader, DWORD col);

int playerChams, wallHack;

_Color enemyVisCol, enemyInvisCol, friendlyVisCol, friendlyInvisCol;

enum DrawnType {
	DRAWN_NOTHING,
	DRAWN_TEAM1,
	DRAWN_TEAM2,
	DRAWN_LAST_TEAM,
	DRAWN_WEAPON,
};

typedef struct {
	DrawnType type;
	char     *model;
	size_t    len;
} DrawnModel;

DrawnModel models[] = {
	DRAWN_TEAM1, "arab", 4,
	DRAWN_TEAM2, "usmc", 4,
	DRAWN_TEAM2, "marine", 6,
	DRAWN_TEAM1, "op_force", 8,
	DRAWN_TEAM1, "opforce", 7,
	DRAWN_TEAM2, "sas", 3,
	DRAWN_LAST_TEAM, "ghillie", 7,
	DRAWN_NOTHING, "reflex", 6,
	DRAWN_NOTHING, "acog", 4,
	DRAWN_WEAPON, "weapon", 6,
	DRAWN_NOTHING, NULL, 0
};

IDirect3DTexture9 *enemyVisTex,
				  *enemyInvisTex,
				  *friendlyVisTex,
				  *friendlyInvisTex;

IDirect3DPixelShader9 *enemyVisPs,
					  *enemyInvisPs,
					  *friendlyVisPs,
					  *friendlyInvisPs;

void InitializeChams() {
	/*
	GenerateTexture(render.device, &enemyVisTex, hostilePlayerChams.visibleColor);
	GenerateTexture(render.device, &enemyInvisTex, hostilePlayerChams.invisibleColor);
	GenerateTexture(render.device, &friendlyVisTex, friendlyPlayerChams.visibleColor);
	GenerateTexture(render.device, &friendlyInvisTex, friendlyPlayerChams.invisibleColor);

	GeneratePixelShader(render.device, &enemyVisPs, hostilePlayerChams.visibleColor);
	GeneratePixelShader(render.device, &enemyInvisPs, hostilePlayerChams.invisibleColor);
	GeneratePixelShader(render.device, &friendlyVisPs, friendlyPlayerChams.visibleColor);
	GeneratePixelShader(render.device, &friendlyInvisPs, friendlyPlayerChams.invisibleColor);
	*/
}

HRESULT __fastcall hDrawIndexedPrimitive(void *thisptr, int edx, LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount) {
	return ((tDrawIndexedPrimitive)oDrawIndexedPrimitive)(thisptr, pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);
}

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32) {
	(*ppD3Dtex) = NULL;

	for (int i = 0; i < 5; ++i) {
		pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL);

		if (*ppD3Dtex) {
			break;
		}
	}

	if (NULL == *ppD3Dtex) {
		return E_FAIL;
	}

	WORD           colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12) | (WORD)(((colour32 >> 20) & 0xF) << 8) | (WORD)(((colour32 >> 12) & 0xF) << 4) | (WORD)(((colour32 >> 4) & 0xF) << 0);
	D3DLOCKED_RECT d3dlr;

	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; ++xy) {
		*pDst16++ = colour16;
	}

	(*ppD3Dtex)->UnlockRect(0);
	return S_OK;
}

HRESULT GeneratePixelShader(IDirect3DDevice9 *pD3Ddev, IDirect3DPixelShader9 **pShader, DWORD col) {
	ID3DXBuffer *pShaderBuf = NULL;
	char         szShader[256];

	float r = ((col >> 16) & 0xFF), g = ((col >> 8) & 0xFF), b = (col & 0xFF), a = ((col >> 24) & 0xFF);

	(*pShader) = NULL;

	for (int i = 0; i < 150; ++i) {
		sprintf_s(szShader, "ps.3.0\ndef c0,%f,%f,%f,%f\nmov oC0,c0", r, g, b, a);
		D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);

		if (!pShaderBuf) {
			sprintf_s(szShader, "ps.1.1\ndef c0,%f,%f,%f,%f\nmov r0,c0", r, g, b, a);
			D3DXAssembleShader(szShader, sizeof(szShader), NULL, NULL, 0, &pShaderBuf, NULL);
		}

		if (pShaderBuf) {
			break;
		}

		Sleep(1);
	}

	if (!pShaderBuf) {
		return E_FAIL;
	}

	if (FAILED(pD3Ddev->CreatePixelShader((const DWORD*)pShaderBuf->GetBufferPointer(), pShader))) {
		pShaderBuf->Release();
		return E_FAIL;
	}

	pShaderBuf->Release();

	return S_OK;
}
