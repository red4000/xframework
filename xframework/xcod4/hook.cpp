 #define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "hook.h"
#include "game.h"
#include "esp.h"
#include "aimbot.h"
#include "autowall.h"

DWORD ocl_packetdup,
	 *pcl_packetdup    = (DWORD*)0xC5F8D8,
	  ocg_drawSnapshot,
	 *pcg_drawSnapshot = (DWORD*)0x74A888;

void *oCL_CreateNewCommands, *oCL_WritePacket;

PVOID  ehHandle;
HANDLE rehookThread;
int curCmdNum;

void __declspec(naked) hCL_CreateNewCommands() {
	curCmdNum = *(int*)0xCC5FF8;
	__asm {
		call oCL_CreateNewCommands
		pushad
	}
	if (curCmdNum != *(int*)0xCC5FF8) {
		InjectCmd();
	}
	__asm {
		popad
		ret
	}
}

void __declspec(naked) hCL_WritePacket() {
	__asm {
		pushad
	}

	__asm {
		popad
		jmp oCL_WritePacket
	}
}

LONG WINAPI HandleGameException(EXCEPTION_POINTERS *ex) {
	if (EXCEPTION_CONTINUE_EXECUTION == HandleAWException(ex)) {
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	switch (ex->ContextRecord->Eip) {
		case 0x42C04B:
			ex->ContextRecord->Edx = ocg_drawSnapshot;
			InitializeGame(true);
			AimbotPostFrame();
			return EXCEPTION_CONTINUE_EXECUTION;
			break;

		case 0x46408A:
			ex->ContextRecord->Ecx = ocl_packetdup;
			//ex->ContextRecord->Eip = hCL_WritePacket;
			InjectCmd();
			return EXCEPTION_CONTINUE_EXECUTION;
			break;
		/*
		case 0x463E00:
			ex->ContextRecord->Eip = (DWORD)hCL_CreateNewCommands;
			return EXCEPTION_CONTINUE_EXECUTION;
			break;
		*/
	}

	if (EXCEPTION_CONTINUE_EXECUTION == HandlePBException(ex)) {
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return HandleHWBPException(ex);
}

DWORD WINAPI RehookGame(LPVOID) {
	while (1) {
		if ((*pcl_packetdup) != NULL) {
			ocl_packetdup    = *pcl_packetdup;
			(*pcl_packetdup) = NULL;
		}
		
		if ((*pcg_drawSnapshot) != NULL) {
			ocg_drawSnapshot    = *pcg_drawSnapshot;
			(*pcg_drawSnapshot) = NULL;
		}

		Sleep(1000);
	}
}

void HookGame() {
	ehHandle     = AddVectoredExceptionHandler(1, HandleGameException);
	rehookThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RehookGame, 0, 0, 0);

	//GenerateTrampoline((void*)0x463E00, &oCL_CreateNewCommands);
	//AddBreakpoint((LPVOID)0x463E00);

	HookPB();
}

void UnhookGame() {
	TerminateThread(rehookThread, 0);
	UnhookPB();
	(*pcl_packetdup)    = ocl_packetdup;
	(*pcg_drawSnapshot) = ocg_drawSnapshot;
	RemoveVectoredExceptionHandler(ehHandle);
}
