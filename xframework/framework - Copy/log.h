#ifndef LOG_H
#define LOG_H

#define ENABLE_LOGGING
#define DEBUG_LOGGING

#ifdef ENABLE_LOGGING
DWORD WINAPI LogThread(void *);
void Log(char *format, ...);
#else
#define Log
#endif

#ifdef DEBUG_LOGGING
#define DEBUG_LOG Log
#else
#define DEBUG_LOG
#endif

void InitializeLog();
void DestroyLog();

#endif
