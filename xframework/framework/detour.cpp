#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "LDE64.h" // TODO: switch to BeaEngine and remove LDE64
#include "detour.h"

#pragma comment(lib, "LDE64")

void *unreadablePage = NULL;

size_t GetInstructionBoundary(void *at, size_t len) {
	size_t i = 0;

	do {
		i += LDE(MakePtr(void*, at, i), 20);

		if (i > len) {
			break;
		}
	} while (i < len);

	return i;
}

DWORD UnprotectMemory(void *address, size_t len, PDWORD oldProtect) {
	DWORD oldProtection;

	if (NULL == oldProtect) {
		oldProtect = &oldProtection;
	}

	VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, oldProtect);

	return oldProtection;
}

void ReprotectMemory(void *address, size_t len, DWORD oldProtect) {
	DWORD oldProtection;

	VirtualProtect(address, len, oldProtect, &oldProtection);
}

void WriteJmp(BYTE *address, void *to) {
	DWORD oldProt = UnprotectMemory(address, 5);

	address[0] = 0xE9;
	DWORD offset = ((DWORD)(to)-(DWORD)(address)) - 5;
	(*(PDWORD)(&address[1])) = offset;

	FlushInstructionCache(GetCurrentProcess(), (LPCVOID)address, 5);

	ReprotectMemory(address, 5, oldProt);
}

void WriteIndirectCall(BYTE *address, void *to) {
	BYTE *buffer = (BYTE*)VirtualAlloc(NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	(*(void**)buffer) = to;
	DWORD oldProt = UnprotectMemory(address, 7);
	(*(BYTE*)(&address[0])) = 0xFF;
	(*(BYTE*)(&address[1])) = 0x25;
	(*(BYTE**)(&address[2])) = (BYTE*)buffer;

	FlushInstructionCache(GetCurrentProcess(), (LPCVOID)address, 7);
	ReprotectMemory(address, 7, oldProt);
}

void *BuildTrampoline(void *address, size_t len) {
	BYTE *buffer = (BYTE*)VirtualAlloc(NULL, len + 5, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	UnprotectMemory(buffer, len + 5);

	if (NULL != buffer) {
		memcpy((void*)buffer, address, len);
		WriteJmp(&buffer[len], (void*)((size_t)(address)+len));
	}

	return (void*)buffer;
}

size_t GetHookLength(int type) {
	switch (type) {
	case HOOK_JMP:
		return 5;

	case HOOK_INDIRECTCALL:
		return 7;
	}

	return 0;
}

void GenerateTrampoline(void *address, void **trampoline) {
	(*trampoline) = BuildTrampoline(address, GetInstructionBoundary(address, 1));
}

size_t DetourFunction(void *address, void *to, void **trampoline, int type) {
	size_t hookLength = 0;

	if (NULL != trampoline) {
		hookLength = GetInstructionBoundary(address, GetHookLength(type));

		if (NULL != trampoline) {
			(*trampoline) = BuildTrampoline(address, hookLength);
		}

		switch (type) {
		case HOOK_JMP:
			WriteJmp((BYTE*)address, to);
			break;

		case HOOK_INDIRECTCALL:
			WriteIndirectCall((BYTE*)address, to);
			break;
		}
	}

	return hookLength;
}

void RetourFunction(void *address, void *trampoline, size_t hookLength) {
	DWORD oldProtect = UnprotectMemory(address, hookLength);
	memcpy(address, trampoline, hookLength);
	ReprotectMemory(address, hookLength, oldProtect);
}