#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "hack.h"
#include "crypt/dauth.h"
#include "cvars.h"
//#include "antidump.h"

//#define LOADER_AUTH

DWORD WINAPI InitializeThread(LPVOID) {
	Sleep(5000);
	InitializeCVars();
	InitializeFramework();
	InitializeHack();
	return 0;
}

DWORD WINAPI InitializeClient(LPVOID) {
	AuthenticateLoader();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitializeThread, 0, 0, 0);
	return 0;
}

BOOL WINAPI DllMain(HMODULE hinstModule, DWORD dwReason, LPVOID lpvReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
#ifdef LOADER_AUTH
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitializeClient, 0, 0, 0);
#else
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitializeThread, 0, 0, 0);
#endif
		break;

	case DLL_PROCESS_DETACH:
		DestroyHack();
		//DestroyFramework();
		break;
	}

	return TRUE;
}
