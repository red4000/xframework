#include "framework.h"
#include "drender.h"
#include "xgui.h"
#include "xguirender.h"

int clockEnabled, clockType, clockSize = 60;
Window *clockWindow;

void InitializeClock() {
	if (clockWindow != NULL) {
		goto setClockPos;
	}
	clockWindow = wm.CreateNewWindow("", clockSize, guiRender.menuFont->h);
setClockPos:
	clockWindow->x       = render.sW - (clockSize + 10);
	clockWindow->y       = 10;
	clockWindow->enabled = 0;
}

void RenderClock() {
	clockWindow->Render();
	SYSTEMTIME st;
	GetLocalTime(&st);
	char timeStr[64];
	sprintf_s(timeStr, sizeof(timeStr), "%02i:%02i:%02i", st.wHour, st.wMinute, st.wSecond);
	switch (clockType)
	{
	case 0:
	default:
		render.RenderText(guiRender.menuFont, clockWindow->x + 1, clockWindow->y + 1, timeStr, foreTextCol);
		break;
	}
}
