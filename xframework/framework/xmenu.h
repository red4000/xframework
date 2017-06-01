#ifndef XMENU_H
#define XMENU_H

#include "_vector.h"
#include "vmath.h"
#include "xgui.h"

enum {
	ME_HEADER,
	ME_CHECKBOX,
	ME_SLIDER,
	ME_KEYINPUT,
	ME_BUTTON
};

enum {
	T_INT,
	T_FLOAT,
	T_DOUBLE
};

class XMenuEntry;

typedef void (*XUserCallback)(XMenuEntry*, void*);

class XMenuEntry {
public:
	int   type, valType, state;
	char *name;
	int   x, y, w, h;
	union {
		int      *iValue;
		float    *fValue;
		double   *dValue;
		FVector3 *fvValue;
		DVector3 *dvValue;
		_Color   *cValue;
	};
	union {
		int       iMin;
		float     fMin;
		double    dMin;
		FVector3 *fvMin;
		DVector3 *dvMin;
	};
	union {
		int       iMax;
		float     fMax;
		double    dMax;
		FVector3 *fvMax;
		DVector3 *dvMax;
	};
	_Vector<XMenuEntry*> entry;
	XMenuEntry          *parent;
	XUserCallback        onClick, onChange;
	void                *userData;
};

class XMenu {
public:
	void Initialize(int x, int y, int w, int h, char *title);
	XMenuEntry *CreateHeader(char *name, XMenuEntry *parent = NULL);
	XMenuEntry *CreateCheckbox(char *name, int *value, XMenuEntry *parent = NULL);
	XMenuEntry *CreateSlider(char *name, int *value, int min, int max, XMenuEntry *parent = NULL);
	XMenuEntry *CreateSlider(char *name, float *value, float min, float max, XMenuEntry *parent = NULL);
	XMenuEntry *CreateSlider(char *name, double *value, double min, double max, XMenuEntry *parent = NULL);
	XMenuEntry *CreateColor(char *name, _Color *value, XMenuEntry *parent = NULL);
	XMenuEntry *CreateVector(char *name, FVector3 *value, FVector3 *min, FVector3 *max, XMenuEntry *parent = NULL);
	XMenuEntry *CreateVector(char *name, DVector3 *value, DVector3 *min, DVector3 *max, XMenuEntry *parent = NULL);
	XMenuEntry *CreateKeyInput(char *name, int *value, XMenuEntry *parent = NULL);
	XMenuEntry *CreateButton(char *name, XUserCallback userOnClick, XMenuEntry *parent = NULL);

	int GetWindowHeight(_Vector<XMenuEntry*> *_entry);
	void RenderWindow(int x, int y, int w, int h, char *title);
	void RenderElement(XMenuEntry *e);
	void RenderMenu(int x, int y, int w, int h, char *title, _Vector<XMenuEntry*> *_entry);
	void Render();

	LRESULT HandleGUIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Window              *window;
	_Vector<XMenuEntry*> entry;
};

#endif

