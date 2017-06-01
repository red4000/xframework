#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include "framework.h"
#include "game.h"
#include "detour.h"
#include "hwbp.h"

#define SS_SAFETY_TIME 3000

DWORD pGdiEntry13,
	  oGdiEntry13,
	  pbBase,
	  pbSize,
	  safetyTime;

void __declspec(naked) hGdiEntry13() {
	__asm {
		pushad
	}

	if (((DWORD)_ReturnAddress()) > pbBase && ((DWORD)_ReturnAddress()) < (pbBase + pbSize)) {
		safetyTime = GetTickCount() + SS_SAFETY_TIME;
		Com_Frame();
		safetyTime = GetTickCount() + SS_SAFETY_TIME;
		Com_Frame();
		safetyTime = GetTickCount() + SS_SAFETY_TIME;
		Com_Frame();
		safetyTime = GetTickCount() + SS_SAFETY_TIME;
	}

	__asm {
		popad
		jmp [oGdiEntry13]
	}
}

DWORD WINAPI AntiPBSSThread(LPVOID) {
	DWORD module;

	while (NULL == (module = (DWORD)GetModuleHandle("pbcl.dll"))) {
		Sleep(100);
	}

	IMAGE_DOS_HEADER *mz = (IMAGE_DOS_HEADER*)module;
	IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS*)(((LONG)module) + mz->e_lfanew);

	char              pbclPath[MAX_PATH];

	GetModuleFileNameA((HMODULE)module, pbclPath, MAX_PATH);
	HANDLE handle = CreateFileA(pbclPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD  size;

	if (INVALID_HANDLE_VALUE != handle) {
		size = GetFileSize(handle, NULL);
		CloseHandle(handle);
	} else {
		size = 388708;
	}

	pbBase = module;
	pbSize = size;
	return 0;
}

LONG WINAPI HandlePBException(EXCEPTION_POINTERS *ex) {
	if (pGdiEntry13 == ex->ContextRecord->Eip) {
		ex->ContextRecord->Eip = (DWORD)hGdiEntry13;
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void HookPB() {
	DWORD gdi32;

	while (NULL == (gdi32 = (DWORD)LoadLibraryA("gdi32.dll"))) {
		Sleep(5);
	}

	pGdiEntry13 = (DWORD)GetProcAddress((HMODULE)gdi32, "GdiEntry13");
	GenerateTrampoline((void*)pGdiEntry13, (void**)&oGdiEntry13);

	AddBreakpoint((LPVOID*)pGdiEntry13);
	AddBreakpoint((LPVOID)GetThreadContext);
	EnableBreakpoints();

	CreateThread(0, 0, AntiPBSSThread, 0, 0, 0);
}

void UnhookPB() {
	DisableBreakpoints();
}
