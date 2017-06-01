#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include "dauth.h"
#include "crypt\dhash.h"

unsigned char ejectStub[] = {0xB8, 0, 0, 0, 0, 0x68, 0, 0, 0, 0, 0x31, 0x04, 0x24, 0x68, 0, 0, 0, 0, 0x31, 0x04, 0x24, 0x68, 0, 0, 0, 0, 0x31, 0x04, 0x24, 0x05, 0, 0, 0, 0, 0xFF, 0xD0, 0x68, 0, 0, 0, 0, 0x58, 0x6A, 0x00, 0x05, 0, 0, 0, 0, 0xFF, 0xD0};

DWORD ejectKey, moduleSize, moduleBase, offset1, offest2;

//BEGIN_WIPABLE_FUNC(void, EjectCheat());
void SetupEjectionState() {
	ejectKey = rand() ^ (rand() << 16);
	moduleBase = (DWORD)GetModuleHandleA(NULL);
	MODULEINFO mi;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBase, &mi, sizeof(MODULEINFO));
	moduleSize = mi.SizeOfImage ^ ejectKey;
	VirtualProtect((LPVOID)moduleBase, mi.SizeOfImage, PAGE_EXECUTE_READWRITE, &mi.SizeOfImage);
	moduleBase ^= ejectKey;
	offset1 = ((DWORD)memset) - ejectKey;
	//offest2 = ((DWORD)ExitThread) - (DWORD)memset;
	offest2 = ((DWORD)ExitProcess) - (DWORD)memset;
}

void EjectCheat() {
	//WIPE_FUNC(AuthenticateLoader);
	SetupEjectionState();
	HANDLE process = GetCurrentProcess();
	DWORD oldProtect;
	void *stub = VirtualAllocEx(process, 0, sizeof(ejectStub), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	VirtualProtectEx(process, stub, sizeof(ejectStub), PAGE_EXECUTE_READWRITE, &oldProtect);

	//DWORD ejectKey = mainPrng.genrand_int32();
	*(DWORD*)(&ejectStub[1]) = ejectKey;
	*(DWORD*)(&ejectStub[6]) = moduleSize;
	*(DWORD*)(&ejectStub[0xE]) = ejectKey;
	*(DWORD*)(&ejectStub[0x16]) = moduleBase;
	*(DWORD*)(&ejectStub[0x1E]) = offset1;
	*(DWORD*)(&ejectStub[0x25]) = (DWORD)memset;
	*(DWORD*)(&ejectStub[0x2D]) = offest2;
	//*(DWORD*)(&ejectStub[0x25]) = (DWORD)ExitThread;
	//*(DWORD*)(&ejectStub[0x2D]) = 0;

	WriteProcessMemory(process, stub, ejectStub, sizeof(ejectStub), &oldProtect);
	_asm jmp stub
}
//END_WIPABLE_FUNC(void, EjectCheat())

DWORD WINAPI EntryThread(LPVOID);
#include "log.h"

//BEGIN_WIPABLE_FUNC(void, AuthenticateLoader())
void AuthenticateLoader() {
	char pipeName[256];
	BYTE nameHash[128];
	char szProcessName[128];
	memset(nameHash, 0, sizeof(nameHash));

	//PreparePageForWiping((void*)DHash);
	GetModuleFileNameA(NULL, szProcessName, sizeof(szProcessName));
	char *pName = strrchr(szProcessName, '\\') + 1;
	memmove(szProcessName, pName, strlen(pName) + 1);
	DHash((BYTE*)&szProcessName, strlen(szProcessName), (BYTE*)&nameHash, sizeof(nameHash));
	size_t hi          = 0;
	char   randAscii[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#%^&*()[]}{.,=-";
	for (size_t i = 0; i < (sizeof(pipeName) - 2); i += 1) {
		pipeName[i] = randAscii[nameHash[(hi++) % sizeof(nameHash)] % sizeof(randAscii)];
	}
	pipeName[sizeof(pipeName) - 1] = 0;
	char finalPipeName[512];
	sprintf_s(finalPipeName, sizeof(finalPipeName), "\\\\.\\pipe\\%s", pipeName);
	DPermutate((BYTE*)nameHash, sizeof(nameHash), 16);

	for (size_t i = 0; i < (sizeof(pipeName) - 2); i += 1) {
		pipeName[i] = randAscii[nameHash[(hi++) % sizeof(nameHash)] % sizeof(randAscii)];
	}
	pipeName[sizeof(pipeName) - 1] = 0;

	HANDLE pipe = CreateFileA(finalPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == pipe) {
		goto dontreturn;
	}

	char  data[sizeof(pipeName)];
	DWORD numRead;
	
	//for (size_t i = 0; i < 10; i++) {
		ReadFile(pipe, data, sizeof(data), &numRead, NULL);
		Log("process %s\npipename %s\nexpected data %s\ndata %s\n", szProcessName, finalPipeName, pipeName, data);

		if (0 < numRead) {
			if (0 == strcmp(data, pipeName)) {
				//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)EntryThread, NULL, NULL, NULL);
				CloseHandle(pipe);
				return;
			}
		}
		Sleep(2000);
		//Sleep(10);
	//}
 dontreturn:
	EjectCheat();
}
//END_WIPABLE_FUNC(void, AuthenticateLoader())
