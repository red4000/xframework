#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "log.h"

CRITICAL_SECTION log_cs;
char             logbuf[4096];
HANDLE           logThread = INVALID_HANDLE_VALUE;

#ifdef ENABLE_LOGGING

bool _Log(char *szLog, const char *data, size_t len) {
	FILE *f = NULL;

	fopen_s(&f, szLog, "a");

	if (!f) {
		fopen_s(&f, szLog, "a");
	}

	if (f) {
		fwrite(data, len, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

void FlushLog() {
	//char fpath[MAX_PATH];
	//strcpy_s(fpath, sizeof(fpath), path);
	//strcat_s(fpath, sizeof(fpath), "sLog.txt");
	//strcpy_s(fpath, "xlog.txt");
	char *fpath = "xlog.txt";

	size_t len = strlen(logbuf);

	if (len > 0) {
		if (_Log(fpath, logbuf, len)) {
			memset(logbuf, 0, sizeof(logbuf));
		}
	}
}

DWORD WINAPI LogThread(void *) {
	InitializeCriticalSection(&log_cs);

	while (1) {
		Sleep(100);
		EnterCriticalSection(&log_cs);
		FlushLog();
		LeaveCriticalSection(&log_cs);
	}

	return 0;
}

void __Log(char *data) {
	strcat_s(logbuf, sizeof(logbuf), data);
}
 
void Log(char *pText, ...) {
	va_list valist;
	char    logBuf[2048];

	va_start(valist, pText);
	int len = vsprintf_s(logBuf, pText, valist);
	va_end(valist);

	EnterCriticalSection(&log_cs);
	if ((len + strlen(logbuf)) >= sizeof(logbuf)) {
		FlushLog();
	}
	__Log(logBuf);
	LeaveCriticalSection(&log_cs);
}

void InitializeLog() {
	logThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LogThread, 0, 0, 0);
}

void DestroyLog() {
	TerminateThread(logThread, 0);
	DeleteCriticalSection(&log_cs);
}

#else

void InitializeLog() {
}

void DestroyLog() {
}

#endif
