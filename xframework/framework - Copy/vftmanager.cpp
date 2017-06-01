#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "vftmanager.h"

size_t CountVFTFunctions(void **vft) {
	MEMORY_BASIC_INFORMATION mbi;
	size_t                   res = 0;

	do {
		if (!IsBadCodePtr((FARPROC)vft[res])) {
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
	if (instance && (*instance) == newVFT) {
		Unhook();
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
		*instance = oldVFT; // TODO: add memory module, w/ ptr checking, and check ptr validity(as game may have freed it)
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
