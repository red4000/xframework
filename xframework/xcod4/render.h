#ifndef RENDER_H
#define RENDER_H

#include "vftmanager.h"

void HookRenderer();
void UnhookRenderer();

extern UINT       stride;
extern VFTManager d3dManager;

#endif
