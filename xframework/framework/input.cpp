#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "_vector.h"
#include <log.h>

_Vector<WNDPROC, 4> listener;
WNDPROC             oldWndProc;
HWND                gameHwnd;
HANDLE              hookInputThread;

void RegisterInputListener(WNDPROC cb) {
	if (NULL == listener.find(cb)) {
		listener.push_back(cb);
	}
}

void UnregisterInputListener(WNDPROC cb) {
	auto it = listener.find(cb);

	if (NULL != it) {
		listener.erase(it);
	}
}

LRESULT CALLBACK newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//DEBUG_LOG("uMsg %x wParam %08x lParam %08x\n", uMsg, wParam, lParam);

	for (auto it = listener.begin(); it != listener.end(); it++) {
		LRESULT result = (*it)(hWnd, uMsg, wParam, lParam);

		if (0 != result) {
			//return 0;
		}
	}

	return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI HookInputThread(char *className) {
	while (1) {
		gameHwnd = FindWindow(className, NULL);
		if (gameHwnd) {
			WNDPROC wndProc = (WNDPROC)GetWindowLong(gameHwnd, GWL_WNDPROC);

			if (wndProc != newWndProc) {
				oldWndProc = wndProc;
				SetWindowLong(gameHwnd, GWL_WNDPROC, (LONG)newWndProc);
			}
		}
		Sleep(500);
	}

	return 0;
}

void HookInput(char *className) {
	hookInputThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HookInputThread, (LPVOID)className, 0, 0);
}

void UnhookInput() {
	TerminateThread(hookInputThread, 0);
	SetWindowLong(gameHwnd, GWL_WNDPROC, (LONG)oldWndProc);
}
