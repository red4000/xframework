#ifndef DETOUR_H
#define DETOUR_H

enum {
	HOOK_JMP,
	HOOK_INDIRECTCALL,
	HOOK_CALL // TODO: implement new hook type, where there are 3x arguments to a trampoline call.
	// i.e.:
	/*
	   xor eax, eax
	   xor eax, 0x59AF25A2
	   push eax
	   xor eax, 0xA01BC97E
	   push eax
	   xor eax, 0x78ABDC21
	   push eax
	   call loc_xxx

	   loc_xxx:
	   pop eax
	   add esp, 8
	   jmp eax
	 */
};

#define MakePtr(cast, ptr, addValue) (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
#define MakeDelta(cast, ptr, subValue) (cast)((DWORD_PTR)(ptr) - (DWORD_PTR)(subValue))

DWORD UnprotectMemory(void *address, size_t len, PDWORD oldProtect = NULL);
void ReprotectMemory(void *address, size_t len, DWORD oldProtect);
void GenerateTrampoline(void *address, void **trampoline);
size_t DetourFunction(void *address, void *to, void **trampoline, int type = HOOK_INDIRECTCALL);
void RetourFunction(void *address, void *trampoline, size_t hookLength);

#endif
