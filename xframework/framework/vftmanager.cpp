#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "vftmanager.h"

bool CheckValidReadPtr(LPVOID ptr) {
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T res = VirtualQuery(ptr, &mbi, sizeof(mbi));
	if (mbi.Protect & PAGE_GUARD) {
		return false;
	}

	if ((mbi.Protect & PAGE_EXECUTE_READ) || (mbi.Protect & PAGE_READONLY) || (mbi.Protect & PAGE_READWRITE) || (mbi.Protect & PAGE_EXECUTE_READ)) {
		return true;
	}

	return false;
}

size_t CountVFTFunctions(void **vft) {
	size_t                   res = 0;

	do {
		if (CheckValidReadPtr((LPVOID)vft[res])) {
			res++;
		} else {
			break;
		}
	} while (vft[res] != NULL);

	return res;
}

void VFTManager::Initialize(void ***inst) {
	instance = inst;
	oldVFT   = *instance;
	numFuncs = CountVFTFunctions(oldVFT);
	newVFT   = (void**)malloc((numFuncs + 1) * sizeof(void*));
	memcpy(newVFT, oldVFT, numFuncs * sizeof(void*));
}

void VFTManager::Destroy() {
	if (instance) {
		if (IsValid()) {
			if ((*instance) == newVFT) {
				Unhook();
			}
		}
	}

	if (newVFT) {
		memset(newVFT, 0, numFuncs * sizeof(void*));
		free(newVFT);
		Unhook();
	}
	oldVFT   = NULL;
	instance = NULL;
	numFuncs = 0;
}

void VFTManager::Hook() {
	*instance = newVFT;
}

void VFTManager::Unhook() {
	if (instance) {
		if (IsValid()) {
			*instance = oldVFT;
		}
	}
}

void VFTManager::HookFunction(int i, void *to) {
	newVFT[i] = to;
}

void VFTManager::UnhookFunction(int i) {
	newVFT[i] = oldVFT[i];
}

bool VFTManager::IsHooked() {
	return *instance == newVFT;
}

bool VFTManager::IsValid() {
	if (CheckValidReadPtr((LPVOID)instance)) {
		if (CheckValidReadPtr((LPVOID)*instance)) {
			return true;
		}
	}
	
	return false;
}

