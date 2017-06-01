#ifndef INPUT_H
#define INPUT_H

void RegisterInputListener(WNDPROC cb);
void UnregisterInputListener(WNDPROC cb);
void HookInput(char *className);
void UnhookInput();

extern HWND gameHwnd;

#endif
