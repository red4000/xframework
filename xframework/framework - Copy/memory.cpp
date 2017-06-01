#include "framework.h"
#include <psapi.h>

DWORD GetModuleSize(HMODULE mod) {
	MODULEINFO mi;
	GetModuleInformation(GetCurrentProcess(), mod, &mi, sizeof(mi));
	return mi.SizeOfImage;
}
