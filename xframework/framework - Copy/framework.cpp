#include "framework.h"
#include "config.h"
#include "drender.h"
#include "input.h"
#include "log.h"
#include "input.h"
#include "xgui.h"
#include "debug.h"
#include "mt19937.h"

void InitializeFramework() {
	SYSTEMTIME st;
	GetSystemTime(&st);
	mainPrng.init_by_array((unsigned long*)&st, sizeof(st) / 4);
	InitializeLog();
	InitializeConfig();
	//HookInput();
	InitializeDebug();
	Sleep(50);
}

void DestroyFramework() {
	ReleaseDebug();
	//UnhookInput();
	DestroyConfig();
	DestroyLog();
}
