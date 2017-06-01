#ifndef ANTIPB_H
#define ANIIPB_H

LONG WINAPI HandlePBException(EXCEPTION_POINTERS *ex);
void HookPB();
void UnhookPB();

extern DWORD safetyTime;

#endif
