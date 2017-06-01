#include <windows.h>
#include "_vector.h"
#include "antidump.h"

BEGIN_WIPABLE_FUNC(void, PreparePageForWiping(void *ptr));
void PreparePageForWiping(void *ptr) {
	DWORD oldProtect;
	VirtualProtect(ptr, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
}
END_WIPABLE_FUNC(void, PreparePageForWiping(void *ptr));

BEGIN_WIPABLE_FUNC(void, WipeMemory(void *start, void *end));
void WipeMemory(void *start, void *end) {
	for (BYTE *i = (BYTE*)start; i != (BYTE*)end; i++) {
		*i ^= *i;
	}
}
END_WIPABLE_FUNC(void, WipeMemory(void *start, void *end));

BEGIN_WIPABLE_FUNC(void, WipeMemoryEx(void *start, size_t len));
void WipeMemoryEx(void *start, size_t len) {
	for (size_t i = 0; i < len; i++) {
		((BYTE*)start)[i] ^= ((BYTE*)start)[i];
	}
}
END_WIPABLE_FUNC(void, WipeMemoryEx(void *start, size_t len));

void WipeString(char *s) {
	DWORD oldProtect;
	VirtualProtect((LPVOID)s, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	size_t len = strlen(s);
	WipeMemoryEx((void*)s, len);
}
