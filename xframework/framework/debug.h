#ifndef DEBUG_H
#define DEBUG_H

typedef DWORD ADDRESS_PROC;
BOOL UpdateModuleCache();
void DumpModules();
void DumpStack(void *sp);
char *GetAddressStringAsRVA(ADDRESS_PROC ea);
LONG WINAPI CrashHandler(EXCEPTION_POINTERS *pExceptionInfo);
void LogVTable(char *name, void **ptr);

#ifdef DEBUG_LOGGING
#define LOGVTABLE(className) LogVTable(#className, (void**)className)
#else
#define LOGVTABLE(className)
#endif

void InitializeDebug();
void ReleaseDebug();

#endif
