#ifndef HWBP_H
#define HWBP_H

typedef BOOL (WINAPI * tGetThreadContext)(HANDLE hThread, LPCONTEXT lpContext);

BOOL WINAPI hGetThreadContext(HANDLE hThread, LPCONTEXT lpContext);
LONG WINAPI HandleHWBPException(EXCEPTION_POINTERS *ex);
void AddBreakpoint(LPVOID at);
void EnableBreakpoints();
void DisableBreakpoints();

extern tGetThreadContext oGetThreadContext;

#endif
