#ifndef HOOK_H
#define HOOK_H

#include <d3d9.h>

void HookGame();
void UnhookGame();
void HookDevice(IDirect3DDevice9 *device);

#endif
