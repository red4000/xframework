#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "chams.h"
#include "render.h"
#include "antipb.h"
#include "game.h"

#define AMBIENT
//#define FASTAMBIENT
#define NOFOG
#define MATERIAL
//#define SILHOUETTE

extern IDirect3DDevice9 *gameDevice;

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
HRESULT GeneratePixelShader(IDirect3DDevice9 *pD3Ddev, IDirect3DPixelShader9 **pShader, DWORD col);
bool fast_strstr(char *input, char *subStr, size_t slStr2);

int playerChams, onlyEnemyChams, visChamStyle, invisChamStyle, wallHack, weaponChams, visWeaponChamStyle, invisWeaponChamStyle, wallChams;

_Color enemyVisCol      = D3DCOLOR_ARGB(255, 255, 20, 20),
	   enemyInvisCol    = D3DCOLOR_ARGB(255, 20, 255, 20),
	   friendlyVisCol   = D3DCOLOR_ARGB(255, 15, 150, 15),
	   friendlyInvisCol = D3DCOLOR_ARGB(255, 20, 255, 20),
	   weaponVisCol     = D3DCOLOR_ARGB(255, 255, 10, 255),
	   weaponInvisCol   = D3DCOLOR_ARGB(255, 20, 255, 255);

enum DrawnType {
	DRAWN_NOTHING,
	DRAWN_TEAM1,
	DRAWN_TEAM2,
	DRAWN_WEAPON,
	DRAWN_LAST_TEAM,
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
				  *friendlyInvisTex,
				  *weaponVisTex,
				  *weaponInvisTex;

IDirect3DPixelShader9 *enemyVisPs,
					  *enemyInvisPs,
					  *friendlyVisPs,
					  *friendlyInvisPs,
					  *weaponVisPs,
					  *weaponInvisPs,
					  *oPix;

typedef HRESULT (WINAPI * tDrawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount);
typedef HRESULT (WINAPI * tSetStreamSource)(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride);
typedef HRESULT (__thiscall * tEndScene)(void*, LPDIRECT3DDEVICE9);

tSetStreamSource      oSetStreamSource;
tDrawIndexedPrimitive oDrawIndexedPrimitive;
//tEndScene             oEndScene;
UINT                  stride;

IDirect3DBaseTexture9 *oTex0,
					  *oTex1;

bool      bTex0, bTex1, bPix;
D3DCAPS9  devCaps;
DWORD     oBLENDENABLE = 0, oSRCCOLOR, oDESTCOLOR, oZENABLE;
int       lastModelType = DRAWN_NOTHING;

#ifdef AMBIENT
BOOL      *lightEnabled;
D3DLIGHT9 *lights;
#endif
#ifdef FASTAMBIENT
DWORD oAmbient;
#endif
#ifdef MATERIAL
D3DMATERIAL9 oMat;
#endif

void PreCham(IDirect3DDevice9 *pDevice, LPDIRECT3DTEXTURE9 tex, LPDIRECT3DPIXELSHADER9 pix, bool alpha = false) {
	if (NULL != tex) {
		pDevice->GetTexture(1, &oTex1);
		pDevice->GetTexture(0, &oTex0);
		pDevice->SetTexture(1, tex);
		pDevice->SetTexture(0, tex);
		bTex0 = bTex1 = true;
	}
	else if (NULL != pix) {
		pDevice->GetPixelShader(&oPix);
		pDevice->SetPixelShader(pix);
		if (alpha) {
			pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oBLENDENABLE);
			pDevice->GetRenderState(D3DRS_SRCBLEND, &oSRCCOLOR);
			pDevice->GetRenderState(D3DRS_DESTBLEND, &oDESTCOLOR);

			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
		}
		bPix = true;
	}

#ifdef NOFOG
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
#endif
#ifdef AMBIENT
	for (size_t i = 0; i < devCaps.MaxActiveLights; i++) {
		lightEnabled[i] = FALSE;
		pDevice->GetLightEnable(i, &lightEnabled[i]);

		if (TRUE == lightEnabled[i]) {
			pDevice->GetLight(i, &lights[i]);
			D3DLIGHT9 newLight = lights[i];
			newLight.Ambient.a = newLight.Ambient.r = newLight.Ambient.g = newLight.Ambient.b = 1.0f;
			pDevice->SetLight(i, &newLight);
		}
	}
#endif
#ifdef FASTAMBIENT
	pDevice->GetRenderState(D3DRS_AMBIENT, &oAmbient);
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
#endif
#ifdef MATERIAL
	pDevice->GetMaterial(&oMat);
	D3DMATERIAL9 newMat;
	newMat.Ambient.r  = newMat.Ambient.g = newMat.Ambient.b = newMat.Ambient.a = 1.0f;
	newMat.Diffuse.r  = newMat.Diffuse.g = newMat.Diffuse.b = newMat.Diffuse.a = 1.0f;
	newMat.Emissive.r = newMat.Emissive.g = newMat.Emissive.b = newMat.Emissive.a = 1.0f;
	newMat.Specular.r = newMat.Specular.g = newMat.Specular.b = newMat.Specular.a = 1.0f;
	newMat.Power      = oMat.Power * 2;
	pDevice->SetMaterial(&newMat);
#endif
}

void GenericPostCham(IDirect3DDevice9 *pDevice, bool alpha = false) {
	if (bTex0) {
		pDevice->SetTexture(0, oTex0);
	}
	if (bTex1) {
		pDevice->SetTexture(1, oTex1);
	}
	else if (bPix) {
		pDevice->SetPixelShader(oPix);
	}

	if (!alpha) {
	} else {
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oBLENDENABLE);
		pDevice->SetRenderState(D3DRS_SRCBLEND, oSRCCOLOR);
		pDevice->SetRenderState(D3DRS_DESTBLEND, oDESTCOLOR);
	}

#ifdef AMBIENT
	for (size_t i = 0; i < devCaps.MaxActiveLights; i++) {
		if (TRUE == lightEnabled[i]) {
			pDevice->SetLight(i, &lights[i]);
		}
	}
#endif
#ifdef FASTAMBIENT
	pDevice->SetRenderState(D3DRS_AMBIENT, oAmbient);
#endif
#ifdef NOFOG
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
#endif
#ifdef MATERIAL
	pDevice->SetMaterial(&oMat);
#endif
}

void InitializeChams() {
	GenerateTexture(render.device, &enemyVisTex, enemyVisCol);
	GenerateTexture(render.device, &enemyInvisTex, enemyInvisCol);
	GenerateTexture(render.device, &friendlyVisTex, friendlyVisCol);
	GenerateTexture(render.device, &friendlyInvisTex, friendlyInvisCol);
	// TODO: weapon resources...

	GeneratePixelShader(render.device, &enemyVisPs, enemyVisCol);
	GeneratePixelShader(render.device, &enemyInvisPs, enemyInvisCol);
	GeneratePixelShader(render.device, &friendlyVisPs, friendlyVisCol);
	GeneratePixelShader(render.device, &friendlyInvisPs, friendlyInvisCol);
}

HRESULT WINAPI hDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT primCount) {
	bool restoreState;

	if (NULL == friendlyInvisPs || GetTickCount() < safetyTime) {
		goto wrong;
	}

	int modelType;
	bool alpha;

	if ((((void*)0x64695C == _ReturnAddress()) || ((void*)0x646A53 == _ReturnAddress()) || ((void*)0x6569BD == _ReturnAddress()))) {
		char *model;

		__asm {
			mov eax, [ebp + 0x48]
			test eax, eax
			je wrong
			mov eax, [eax + 0xB8]
			test eax, eax
			je wrong
			mov eax, [eax]
			test eax, eax
			je wrong
			mov model, eax
		}

		restoreState = false;
		modelType    = DRAWN_NOTHING;

		if (('m' == model[0]) && ('c' == model[1]) && ('m' == model[3]) && ('e' != model[7])) {
			for (int i = 0; NULL != models[i].model; ++i) {
				if (fast_strstr(model, models[i].model, models[i].len)) {
					modelType = models[i].type;
					goto check;
				}
			}

			goto wrong;
		} else {
			goto wrong;
		}

 check:;
		int oModelType = modelType, team;
		switch (modelType) {
		case DRAWN_NOTHING:
			goto wrong;
			break;

		case DRAWN_TEAM1:
			team = localEntity->GetTeam();
			if (0 == team || (modelType != team)) {
				lastModelType = DRAWN_TEAM1;
			} else {
				lastModelType = DRAWN_TEAM2;
			}
			break;

		case DRAWN_TEAM2:
			team = localEntity->GetTeam();
			if (0 == team || (modelType != team)) {
				modelType     = DRAWN_TEAM1;
				lastModelType = DRAWN_TEAM1;
			} else {
				lastModelType = DRAWN_TEAM2;
			}
			break;

		case DRAWN_LAST_TEAM:
			modelType = lastModelType;
			break;

		case DRAWN_WEAPON:
			break;
		}

		if (1 == wallHack) {
			bTex0        = bTex1 = bPix = false;
			restoreState = false;
			alpha        = false;

			switch (modelType) {
			case DRAWN_TEAM1:
				if (playerChams) {
					switch (invisChamStyle) {
					case 0:
						break;
						
					case 1:
						PreCham(pDevice, enemyInvisTex, NULL);
						restoreState = true;
						break;
						
					case 2:
						PreCham(pDevice, NULL, enemyInvisPs);
						restoreState = true;
						break;
						
					case 3:
						PreCham(pDevice, enemyInvisTex, NULL, true);
						alpha        = true;
						restoreState = true;
						break;
						
					case 4:
						PreCham(pDevice, NULL, enemyInvisPs, true);
						alpha        = true;
						restoreState = true;
						break;
					}
						
					case 5:
						goto afterInvisDraw;
						break;
				}
				break;

			case DRAWN_TEAM2:
				if (playerChams) {
					if (!onlyEnemyChams) {
						switch (invisChamStyle) {
						case 0:
							break;
							
						case 1:
							PreCham(pDevice, friendlyInvisTex, NULL);
							restoreState = true;
							break;
							
						case 2:
							PreCham(pDevice, NULL, friendlyInvisPs);
							restoreState = true;
							break;
							
						case 3:
							PreCham(pDevice, friendlyInvisTex, NULL, true);
							restoreState = true;
							alpha        = true;
							break;
							
						case 4:
							PreCham(pDevice, NULL, friendlyInvisPs, true);
							restoreState = true;
							alpha        = true;
							break;
							
						case 5:
							goto afterInvisDraw;
							break;
						}
					}
				}
				break;

			case DRAWN_WEAPON:
				if (weaponChams) {
					switch (invisWeaponChamStyle) {
					case 0:
						break;
						
					case 1:
						PreCham(pDevice, weaponInvisTex, NULL);
						restoreState = true;
						break;
						
					case 2:
						PreCham(pDevice, NULL, weaponInvisPs);
						restoreState = true;
						break;
						
					case 3:
						PreCham(pDevice, weaponInvisTex, NULL, true);
						alpha        = true;
						restoreState = true;
						break;
						
					case 4:
						PreCham(pDevice, NULL, weaponInvisPs, true);
						alpha        = true;
						restoreState = true;
						break;
						
					case 5:
						goto afterInvisDraw;
						break;
					}
				}
				break;
			}

			pDevice->SetRenderState(D3DRS_ZENABLE, false);
			oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);
			pDevice->SetRenderState(D3DRS_ZENABLE, true);
			if (true == restoreState) {
				GenericPostCham(pDevice, alpha);
				restoreState = false;
				alpha        = false;
			}
afterInvisDraw:;
		}

		bTex0 = bTex1 = bPix = false;
#ifdef SILHOUETTE
		pDevice->GetPixelShader(&oPix);
		//pDevice->GetTexture(0, &oTex0);
		//pDevice->SetTexture(0, NULL);
		LPDIRECT3DSURFACE9 oldRenderSurface;
		gameDevice->GetRenderTarget(0, &oldRenderSurface);
		pDevice->SetRenderTarget(0, silSurface);
		pDevice->SetPixelShader(pixelShader);
		float constData[4], oldData[4];
		pDevice->GetPixelShaderConstantF(0, oldData, 1);

		DWORD oZenable;
		pDevice->GetRenderState(D3DRS_ZENABLE, &oZenable);
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		switch (oModelType) {
		case DRAWN_TEAM1:
			constData[0] = yellow.value[0]; constData[1] = yellow.value[1]; constData[2] = yellow.value[2]; constData[3] = yellow.value[3];
			break;

		case DRAWN_TEAM2:
			constData[0] = blue.value[0]; constData[1] = blue.value[1]; constData[2] = blue.value[2]; constData[3] = blue.value[3];
			break;

		case DRAWN_WEAPON:
			constData[0] = black.value[0]; constData[1] = black.value[1]; constData[2] = black.value[2]; constData[3] = black.value[3];
			break;
		}

		pDevice->SetPixelShaderConstantF(0, constData, 1);
		oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);
		pDevice->SetRenderState(D3DRS_ZENABLE, oZenable);

		switch (oModelType) {
		case DRAWN_TEAM1:
			constData[0] = red.value[0]; constData[1] = red.value[1]; constData[2] = red.value[2]; constData[3] = red.value[3];
			break;

		case DRAWN_TEAM2:
			constData[0] = green.value[0]; constData[1] = green.value[1]; constData[2] = green.value[2]; constData[3] = green.value[3];
			break;

		case DRAWN_WEAPON:
			constData[0] = black.value[0]; constData[1] = black.value[1]; constData[2] = black.value[2]; constData[3] = black.value[3];
			break;
		}

		pDevice->SetPixelShaderConstantF(0, constData, 1);
		oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);
		//pDevice->SetTexture(0, oTex0);
		pDevice->SetPixelShader(oPix);
		pDevice->SetRenderTarget(0, oldRenderSurface);
		pDevice->SetPixelShaderConstantF(0, oldData, 1);
#endif
		
		restoreState = false;
		switch (modelType) {
		case DRAWN_TEAM1:
			if (playerChams) {
				switch (visChamStyle) {
				case 0:
					break;
					
				case 1:
					PreCham(pDevice, enemyVisTex, NULL);
					restoreState = true;
					break;
					
				case 2:
					PreCham(pDevice, NULL, enemyVisPs);
					restoreState = true;
					break;
					
				case 3:
					PreCham(pDevice, enemyVisTex, NULL, true);
					alpha        = true;
					restoreState = true;
					break;
					
				case 4:
					PreCham(pDevice, NULL, enemyVisPs, true);
					alpha        = true;
					restoreState = true;
					break;
					
				case 5:
					goto afterVisDraw;
					break;
				}
			}
			break;

		case DRAWN_TEAM2:
			if (playerChams) {
				if (!onlyEnemyChams) {
					switch (visChamStyle) {
					case 0:
						break;
						
					case 1:
						PreCham(pDevice, friendlyVisTex, NULL);
						restoreState = true;
						break;
						
					case 2:
						PreCham(pDevice, NULL, friendlyVisPs);
						restoreState = true;
						break;
						
					case 3:
						PreCham(pDevice, friendlyVisTex, NULL, true);
						restoreState = true;
						alpha        = true;
						break;
						
					case 4:
						PreCham(pDevice, NULL, friendlyVisPs, true);
						restoreState = true;
						alpha        = true;
						break;
					
					case 5:
						goto afterVisDraw;
						break;
					}
				}
			}
			break;

		case DRAWN_WEAPON:
			if (weaponChams) {
				switch (visWeaponChamStyle) {
				case 0:
					break;
					
				case 1:
					PreCham(pDevice, weaponVisTex, NULL);
					restoreState = true;
					break;
					
				case 2:
					PreCham(pDevice, NULL, weaponVisPs);
					restoreState = true;
					break;
					
				case 3:
					PreCham(pDevice, weaponVisTex, NULL, true);
					alpha        = true;
					restoreState = true;
					break;
					
				case 4:
					PreCham(pDevice, NULL, weaponVisPs, true);
					alpha        = true;
					restoreState = true;
					break;
					
				case 5:
					goto afterVisDraw;
					break;
				}
			}
			break;
		}
	}
//wrong:
	HRESULT result = oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);

	if (true == restoreState) {
		GenericPostCham(pDevice, alpha);
	}
	afterVisDraw:;

	return result;
	
 wrong:
	return oDrawIndexedPrimitive(pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, primCount);
}

/*
HRESULT __fastcall hEndScene(void *thisptr, int edx, IDirect3DDevice9 *pDevice) {
#ifdef SILHOUETTE
	gameDevice->GetPixelShader(&oPix);
	gameDevice->SetPixelShader(silPS);
	gameDevice->GetTexture(0, &oTex0);
	gameDevice->SetTexture(0, silTexture);
	LPDIRECT3DVERTEXBUFFER9 oldStreamSource;
	UINT                    oldOffset, oldStride;
	gameDevice->GetStreamSource(0, &oldStreamSource, &oldOffset, &oldStride);
	gameDevice->SetStreamSource(0, silBuffer, 0, sizeof(sVertex));
	DWORD oldFVF;
	gameDevice->GetFVF(&oldFVF);
	gameDevice->SetFVF(SILFVF);
	LPDIRECT3DSURFACE9 oldRenderSurface;
	gameDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
#endif

	HRESULT res = oEndScene(thisptr, pDevice);
#ifdef SILHOUETTE
	gameDevice->GetRenderTarget(0, &oldRenderSurface);
	gameDevice->SetRenderTarget(0, silSurface);
	gameDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0);

	gameDevice->SetRenderTarget(0, oldRenderSurface);
	gameDevice->SetPixelShader(oPix);
	gameDevice->SetTexture(0, oTex0);
	gameDevice->SetStreamSource(0, oldStreamSource, oldOffset, oldStride);
	gameDevice->SetFVF(oldFVF);
#endif

	return res;
}
*/

HRESULT APIENTRY hSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9 *pStreamData, UINT OffsetInBytes, UINT Stride) {
	stride = Stride;
	return(oSetStreamSource(pDevice, StreamNumber, pStreamData, OffsetInBytes, Stride));
}

void HookChams() {
	gameDevice = *(IDirect3DDevice9**)0xCC9A408;
	//oEndScene = (tEndScene)d3dManager.oldVFT[42];
	//d3dManager.HookFunction(42, (void*)hEndScene);
	oDrawIndexedPrimitive = (tDrawIndexedPrimitive)d3dManager.oldVFT[82];
	d3dManager.HookFunction(82, (void*)hDrawIndexedPrimitive);
	oSetStreamSource = (tSetStreamSource)d3dManager.oldVFT[100];
	d3dManager.HookFunction(100, (void*)hSetStreamSource);
}

void UnhookChams() {
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

size_t fast_strlen(char *input) {
	size_t i = 0;

	while (1) {
		if (input[i++] == 0) {
			return --i;
		}
	}
}

bool fast_strstr(char *input, char *subStr, size_t slStr2) {
	size_t slStr        = fast_strlen(input);
	size_t correctChars = 0;

	for (size_t i = 0; i < (slStr - slStr2); ++i) {
		for (size_t h = 0; h < slStr2 + 1; ++h) {
			if (input[i + h] == subStr[h]) {
				correctChars++;

				if (correctChars >= slStr2) {
					return true;
				}
			} else {
				correctChars = 0;
				break;
			}
		}
	}

	return false;
}
