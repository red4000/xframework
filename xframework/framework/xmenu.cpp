#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "_vector.h"
#include "drender.h"
#include "xgui.h"
#include "xguirender.h"
#include <commctrl.h>
#include "input.h"
#include "menu.h"
#include "xmenu.h"

#define MENUKEY VK_INSERT

void XMenu::Initialize(int x, int y, int w, int h, char *title) {
	window               = wm.CreateNewWindow(title, w, h);
	//window->renderWindow = 0;
	window->enabled      = 0;
	window->x            = x;
	window->y            = y;
}

XMenuEntry *XMenu::CreateHeader(char *name, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->valType = T_INT;
	res->type    = ME_HEADER;
	res->name    = name;
	res->parent  = parent;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

XMenuEntry *XMenu::CreateCheckbox(char *name, int *value, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->valType = T_INT;
	res->type    = ME_CHECKBOX;
	res->name    = name;
	res->iValue  = value;
	res->parent  = parent;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

XMenuEntry *XMenu::CreateSlider(char *name, int *value, int min, int max, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->valType = T_INT;
	res->type    = ME_SLIDER;
	res->iValue  = value;
	res->iMin    = min;
	res->iMax    = max;
	res->name    = name;
	res->parent  = parent;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

XMenuEntry *XMenu::CreateSlider(char *name, float *value, float min, float max, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->valType = T_INT;
	res->type    = ME_SLIDER;
	res->fValue  = value;
	res->fMin    = min;
	res->fMax    = max;
	res->name    = name;
	res->parent  = parent;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

XMenuEntry *XMenu::CreateSlider(char *name, double *value, double min, double max, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->valType = T_DOUBLE;
	res->type    = ME_SLIDER;
	res->dValue  = value;
	res->dMin    = min;
	res->dMax    = max;
	res->name    = name;
	res->parent  = parent;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

void UpdateColor(XMenuEntry *e, int *col) {
	switch (e->name[0]) {
	case 'A':
		break;
		
	case 'R':
		break;
		
	case 'G':
		break;
		
	case 'B':
		break;
	}
}

XMenuEntry *XMenu::CreateColor(char *name, _Color *value, XMenuEntry *parent) {
	XMenuEntry *res = CreateHeader(name, parent);
	float *resValues = (float*)malloc(sizeof(float) * 4);
	XMenuEntry *a = CreateSlider("A", &resValues[0], 0, 255.f, res);
	a->userData = (void*)value;
	a->onChange = (XUserCallback)UpdateColor;
	XMenuEntry *r = CreateSlider("R", &resValues[1], 0, 255.f, res);
	r->userData = (void*)value;
	r->onChange = (XUserCallback)UpdateColor;
	XMenuEntry *g = CreateSlider("G", &resValues[2], 0, 255.f, res);
	g->userData = (void*)value;
	g->onChange = (XUserCallback)UpdateColor;
	XMenuEntry *b = CreateSlider("B", &resValues[3], 0, 255.f, res);
	b->userData = (void*)value;
	b->onChange = (XUserCallback)UpdateColor;
	return res;
}

XMenuEntry *XMenu::CreateVector(char *name, FVector3 *value, FVector3 *min, FVector3 *max, XMenuEntry *parent) {
	XMenuEntry *res = CreateHeader(name, parent);
	CreateSlider("X", &value->x, min->x, max->x, res);
	CreateSlider("Z", &value->y, min->y, max->y, res);
	CreateSlider("Z", &value->z, min->z, max->z, res);
	return res;
}

XMenuEntry *XMenu::CreateVector(char *name, DVector3 *value, DVector3 *min, DVector3 *max, XMenuEntry *parent) {
	XMenuEntry *res = CreateHeader(name, parent);
	CreateSlider("X", &value->x, min->x, max->x, res);
	CreateSlider("Y", &value->y, min->y, max->y, res);
	CreateSlider("Z", &value->z, min->z, max->z, res);
	return res;
}

XMenuEntry *XMenu::CreateKeyInput(char *name, int *value, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->type    = ME_KEYINPUT;
	res->name    = name;
	res->parent  = parent;
	res->iValue  = value;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

XMenuEntry *XMenu::CreateButton(char *name, XUserCallback userOnClick, XMenuEntry *parent) {
	XMenuEntry *res = (XMenuEntry*)calloc(1, sizeof(XMenuEntry));
	res->type    = ME_BUTTON;
	res->name    = name;
	res->parent  = parent;
	res->onClick = userOnClick;
	if (parent == NULL) {
		entry.push_back(res);
	} else {
		parent->entry.push_back(res);
	}
	return res;
}

int XMenu::GetWindowHeight(_Vector<XMenuEntry*> *_entry) {
	return 30 + _entry->size() * 25;
}

void XMenu::RenderWindow(int x, int y, int w, int h, char *title) {
	render.Render2DOutlinedRect(x, y, w, h, 1, backgroundCol, outlineCol);
	render.RenderText(guiRender.menuFont, x + 5, y + 10, title, highlightCol);
	render.Render2DRect(x + 1, y + 15 + guiRender.menuFont->h, w - 2, h - (15 + 2 + guiRender.menuFont->h), foregroundCol);
}

void XMenu::RenderElement(XMenuEntry *e) {
	switch (e->type) {
	case ME_HEADER:
		render.RenderText(guiRender.menuFont, e->x + 5, e->y, e->name);
		// TODO: render > at the right edge
		break;

	case ME_CHECKBOX:
		//render box
		render.Render2DRectEdges(e->x, e->y, 20, 20, 1);
		if (*e->iValue) {
			render.Render2DRect(e->x + 5, e->y + 5, 10, 10);
		}
		render.RenderText(guiRender.menuFont, e->x + 25, e->y, e->name);
		break;

	case ME_SLIDER:
		break;

	case ME_KEYINPUT:
		break;

	case ME_BUTTON:
		break;
	}
}

void XMenu::RenderMenu(int x, int y, int w, int h, char *title, _Vector<XMenuEntry*> *entry) {
	int cy = y + 30, hy = 0;
	RenderWindow(x, y, w, h, title);

	for (auto it = entry->begin(); it != entry->end(); it++) {
		XMenuEntry *e = *it;

		e->x = x + 5;
		e->y = cy;
		e->w = w - 10;
		e->h = 25;

		RenderElement(e);

		if (e->type == ME_HEADER) {
			if (e->state) {
				if (hy == 0 || cy > hy) {
					hy = cy;
				}
				int hh = GetWindowHeight(&e->entry);
				RenderMenu(x + w + 5, hy, w, hh, e->name, &e->entry);
				hy += hh + 5;
			}
		}

		cy += e->h;
	}
}

void XMenu::Render() {
	if (window->enabled && window->state) {
		window->h = GetWindowHeight(&entry);
		RenderMenu(window->x, window->y, window->w, window->h , window->text, &entry);
	}
}

LRESULT XMenu::HandleGUIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return 0;
}
