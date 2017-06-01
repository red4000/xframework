#ifndef RENDER_H
#define RENDER_H

void HookRenderer();
void UnhookRenderer();

typedef HRESULT (__thiscall *tDrawIndexedPrimitive)(void*, LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

extern void *oDrawIndexedPrimitive;
extern UINT  stride;

#endif
