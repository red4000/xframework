#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include "hwbp.h"
#include <detour.h>

tGetThreadContext oGetThreadContext;
LPVOID hwbpLocation[4], handler[4];
size_t numBps = 0;

BOOL WINAPI hGetThreadContext(HANDLE hThread, LPCONTEXT lpContext) {
	BOOL result = oGetThreadContext(hThread, lpContext);

	if (0 != result) {
		lpContext->Dr0 = lpContext->Dr1 = lpContext->Dr2 = lpContext->Dr3 = lpContext->Dr6 = lpContext->Dr7 = 0;
	}

	return result;
}

LONG WINAPI HandleHWBPException(EXCEPTION_POINTERS *ex) {
	if (EXCEPTION_SINGLE_STEP != ex->ExceptionRecord->ExceptionCode) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	if (((DWORD)GetThreadContext) == ex->ContextRecord->Eip) {
		ex->ContextRecord->Eip = (DWORD)hGetThreadContext;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	/*
	for (size_t i = 0; i < numBps; i++) {
		if (((DWORD)hwbpLocation[i]) == ex->ContextRecord->Eip) {
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	*/

	return EXCEPTION_CONTINUE_SEARCH;
}

void AddBreakpoint(LPVOID at) {
	if (4 > numBps) {
		hwbpLocation[numBps++] = at;
	}
}

DWORD EnumProcessThreads(THREADENTRY32 *result, size_t maxThreads) { // FIXME: thread enumeration is x32 and not x64 compatible
	HANDLE        threadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	threadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());

	if (INVALID_HANDLE_VALUE == threadSnap) {
		return FALSE;
	}

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(threadSnap, &te32)) {
		CloseHandle(threadSnap);
		return FALSE;
	}

	size_t i = 0;

	do {
		if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
			result[i++] = te32;
			if (i >= maxThreads) {
				break;
			}
		}
	} while (Thread32Next(threadSnap, &te32));

	CloseHandle(threadSnap);
	return i;
}

void EnableBreakpoints() {
	THREADENTRY32 threads[256];

	memset(threads, 0, sizeof(threads));
	size_t numThreads = EnumProcessThreads(threads, 256);
	GenerateTrampoline((void*)GetThreadContext, (void**)&oGetThreadContext);

	for (size_t i = 0; i < numThreads; i++) {
		HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT, false, threads[i].th32ThreadID);

		if (INVALID_HANDLE_VALUE != hThread) {
			CONTEXT ctx;

			memset((void*)&ctx, 0, sizeof(ctx));
			SuspendThread(hThread);
			GetThreadContext(hThread, &ctx);

			ctx.ContextFlags |= CONTEXT_DEBUG_REGISTERS;
			ctx.Dr0           = (DWORD)hwbpLocation[0];
			ctx.Dr1           = (DWORD)hwbpLocation[1];
			ctx.Dr2           = (DWORD)hwbpLocation[2];
			ctx.Dr3           = (DWORD)hwbpLocation[3];
			ctx.Dr6           = 0;
			ctx.Dr7           = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);

			SetThreadContext(hThread, &ctx);
			ResumeThread(hThread);
		}
	}
}

void DisableBreakpoints() {
	THREADENTRY32 threads[256];

	memset(threads, 0, sizeof(threads));
	size_t numThreads = EnumProcessThreads(threads, sizeof(threads) / sizeof(THREADENTRY32));

	for (size_t i = 0; i < numThreads; i++) {
		HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT, false, threads[i].th32ThreadID);

		if (INVALID_HANDLE_VALUE != hThread) {
			CONTEXT ctx;

			memset((void*)&ctx, 0, sizeof(ctx));
			SuspendThread(hThread);
			GetThreadContext(hThread, &ctx);

			ctx.ContextFlags |= CONTEXT_DEBUG_REGISTERS;
			ctx.Dr0           = 0;
			ctx.Dr1           = 0;
			ctx.Dr2           = 0;
			ctx.Dr3           = 0;
			ctx.Dr6           = 0;
			ctx.Dr7           = 0;

			SetThreadContext(hThread, &ctx);
			ResumeThread(hThread);
		}
	}
}
