#include "framework.h"

bool bDataCompare(BYTE *pData, BYTE *bMask, char *szMask) {
	for (; *szMask; ++szMask, ++pData, ++bMask) {
		if (*szMask == 'x' && *pData != *bMask) {
			return false;
		}
	}
	return (*szMask) == NULL;
}

DWORD FindPattern(DWORD dwAddress, DWORD dwSize, BYTE *pbMask, char *szMask) {
	for (DWORD i = NULL; i < dwSize; i++) {
		if (bDataCompare((BYTE*)(dwAddress + i), pbMask, szMask)) {
			return (DWORD)(dwAddress + i);
		}
	}
	return 0;
}