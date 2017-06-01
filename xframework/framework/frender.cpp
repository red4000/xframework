#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "drender.h"
#include "xgui.h"
#include "game.h"
#include "menu.h"
#include "esp.h"
#include "radar.h"
#include "clock.h"
#include "warning.h"

void InitializeFrameworkResources() {
	InitializeMenu();
	InitializeESP();
	InitializeWarning();
	InitializeRadar();
	InitializeClock();
}

void ReleaseFrameworkResources() {
	DestroyMenu();
	ReleaseESP();
	ReleaseRadar();
	//ReleaseClock();
}

void RenderFrameworkFrame() {
	RenderClock();
}
