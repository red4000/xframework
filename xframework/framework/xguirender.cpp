#include <windows.h>
#include "_vector.h"
#include "drender.h"
#include "xgui.h"
#include "xguirender.h"

GUIRenderer guiRender;

_Color outlineCol    = D3DCOLOR_ARGB(255, 32, 22, 32),
	   backgroundCol = D3DCOLOR_ARGB(255, 238, 238, 238), 
	   foregroundCol = D3DCOLOR_ARGB(255, 100, 100, 100),
	   highlightCol  = D3DCOLOR_ARGB(255, 150, 150, 150),
	   foreTextCol   = D3DCOLOR_ARGB(255, 48, 48, 48),
	   backTextCol   = D3DCOLOR_ARGB(255, 70, 70, 70),
	   ctrlFillCol   = D3DCOLOR_ARGB(255, 255, 255, 255),
	   ctrlLineCol   = D3DCOLOR_ARGB(255, 37, 25, 35),
	   hlctrlLineCol = D3DCOLOR_ARGB(255, 63, 51, 61),
	   shadowCol     = D3DCOLOR_ARGB(50, 20, 20, 20),
	   outlineHlCol  = D3DCOLOR_ARGB(255, 71, 59, 73),
	   outlineHlCol2 = D3DCOLOR_ARGB(255, 107, 92, 113),
	   _outlineCol   = D3DCOLOR_ARGB(255, 20, 20, 20);

GUIRenderer::GUIRenderer() {
}

GUIRenderer::~GUIRenderer() {
}

void GUIRenderer::Initialize() {
	menuFont = render.CreateNewFont("Courier New", 14, FL_BOLD);
	bigMenuFont = render.CreateNewFont("Courier New", 16, FL_BOLD);
}

void GUIRenderer::Destroy() {
}

void GUIRenderer::SetElementSize(int type, Element *e) {
}

void GUIRenderer::RenderCheckBox(int sx, int sy, CheckBox *cb) {
	render.Render2DOutlinedRect(sx + 2, sy + 2, cb->h - 4, cb->h - 4, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	render.RenderText(menuFont, sx + cb->h + 2, sy, cb->text, textColor);
	if (*cb->state) {
		//render.Render2DRect(sx + (int)(((float)cb->h) * 0.25f), sy + (int)(((float)cb->h) * 0.25f), ((int)(((float)cb->h) * 0.5f)), ((int)(((float)cb->h) * 0.5f)), foregroundCol);
		render.Render2DRect(sx + 7, sy + 3, 1, 8, lineColor);
		render.Render2DRect(sx + 3, sy + 7, 8, 1, lineColor);
	}
}

void GUIRenderer::RenderSlider(int sx, int sy, Slider *s) {
	RECT r;
	r.left   = sx + 3;
	r.top    = sy;
	r.bottom = sy + s->h;
	r.right  = sx + s->w;
	render.Render2DOutlinedRect(sx, sy + menuFont->h + 1, s->w, 4, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	int fx;
	switch (s->valueType) {
	case V_FLOAT:
		fx = ((float)s->w - 2) * (((*s->fFrac) - (s->fMin)) / (s->fMax - s->fMin));
		render.Render2DRect(sx + 1, sy + menuFont->h + 2, fx, 2, foregroundCol);

		//render.SetColor(lineColor);
		//render.Render2DRect(sx + fx - 3, sy + menuFont->h +, 6, 20);
		//render.Render2DTriangle(sx + fx - 3, sy + menuFont->h , sx + fx + 3, sy + menuFont->h - 2, sx + fx, sy + menuFont->h - 6);

		render.SetColor(textColor);
		render.RenderTextEx(menuFont, &r, s->text, RT_TOP | RT_LEFT | RT_NOCLIP);
		r.top += menuFont->h - 4;
		r.left = r.right + 3;
		render.RenderTextExf(menuFont, &r, RT_TOP | RT_LEFT | RT_NOCLIP, "%.3f", (double)*s->fFrac);
		break;

	case V_INT:
		render.Render2DRect(sx + 1, sy + menuFont->h + 2, ((float)s->w - 2) * (((*s->iFrac) - (s->iMin)) / (s->iMax - s->iMin)), 2, foregroundCol);
		render.SetColor(textColor);
		render.RenderTextEx(menuFont, &r, s->text, RT_TOP | RT_LEFT | RT_NOCLIP);
		r.top += menuFont->h - 4;
		r.left = r.right + 3;
		render.RenderTextExf(menuFont, &r, RT_TOP | RT_LEFT | RT_NOCLIP, "%i", *s->iFrac);
		break;
	}
}

void GUIRenderer::RenderComboBox(int sx, int sy, ComboBox *cb) {
	RECT r;
	r.left   = sx;
	r.top    = sy + 2;
	r.bottom = 0;
	r.right  = 0;

	if (cb->state == 0) {
		render.Render2DOutlinedRect(sx + cb->xo, sy, cb->w - cb->xo, cb->h, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
		render.RenderText(menuFont, sx + cb->xo + 2, sy + 2, cb->entries[*cb->selected], textColor);
		render.RenderTextEx(menuFont, &r, cb->text, RT_LEFT | RT_TOP | RT_NOCLIP, textColor);
		render.Render2DTriangle(sx + (cb->w - 13), sy + menuFont->h, sx + (cb->w - 7), sy + 4, sx + (cb->w - 2), sy + menuFont->h, foregroundCol);
	} else {
		render.Render2DOutlinedRect(sx + cb->xo, sy, cb->w - cb->xo, 4 + menuFont->h * cb->numEntries, CTRLLINETHICKNESS, ctrlFillCol, hlctrlLineCol);
		render.Render2DTriangle(sx + (cb->w - 13), sy + 4, sx + (cb->w - 7), sy + menuFont->h, sx + (cb->w - 2), sy + 4, foregroundCol);
		//render.Flush(); // fixes bug in testproject but bugs cod4 rendering
		//render.s->End();
		//render.s->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		render.RenderTextEx(menuFont, &r, cb->text, RT_LEFT | RT_TOP | RT_NOCLIP, highlightCol);

		for (size_t i = 0; i < cb->numEntries; i++) {
			if (!CONTAINSMOUSE(sx, sy + 2 + menuFont->h * i, cb->w, menuFont->h)) {
				render.RenderText(menuFont, sx + cb->xo + 2, sy + 2 + menuFont->h * i, cb->entries[i], backTextCol);
			} else {
				render.RenderText(menuFont, sx + cb->xo + 2, sy + 2 + menuFont->h * i, cb->entries[i], highlightCol);
			}
		}
	}
}

void GUIRenderer::RenderInputBox(int sx, int sy, InputBox *ib) {
	RECT r;
	r.left   = sx;
	r.top    = sy + menuFont->h + 2;
	r.bottom = sy + menuFont->h * 2 + 2;
	r.right  = sx + ib->w;
	if (wm.selectedElement == ib) {
		render.Render2DOutlinedRect(sx, sy + menuFont->h + 1, ib->w, menuFont->h + 2, CTRLLINETHICKNESS, ctrlFillCol, hlctrlLineCol);
		DWORD     currentTime = GetTickCount();
		if (0 != ib->cursorTime) {
			if ((currentTime - ib->cursorTime) > CURSORFLASHTIME) {
				ib->cursorTime = currentTime;
				ib->flash     ^= 1;
			}
		} else {
			ib->cursorTime = currentTime;
			ib->flash      = 0;
		}

		if (ib->flash) {
			render.Render2DRect(sx + 1 + ib->right, sy + menuFont->h + 2, CURSORTHICKNESS, menuFont->h, foreTextCol);
		}
			
		render.RenderText(menuFont, sx + 2, sy, ib->text, foreTextCol);
		render.RenderTextEx(menuFont, &r, ib->input, RT_LEFT | RT_TOP, foreTextCol);
	} else {
		render.Render2DOutlinedRect(sx, sy + menuFont->h + 1, ib->w, menuFont->h + 2, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
		render.RenderTextEx(menuFont, &r, ib->input, RT_LEFT | RT_TOP, textColor);
		render.RenderText(menuFont, sx + 2, sy, ib->text, textColor);
	}
}

void GUIRenderer::RenderPreciseSlider(int sx, int sy, PreciseSlider *ps) {
	RECT r;
	r.left   = sx + 2;
	r.top    = sy;
	r.bottom = sy + ps->h;
	r.right  = sx + ps->w;
	render.Render2DOutlinedRect(sx, sy + menuFont->h + 1, ps->w, 6, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	render.Render2DRect(sx + 1, sy + menuFont->h + 2, ((float)ps->w - 2) * ((*ps->frac) - (ps->min) / (ps->max - ps->min)), 4, foregroundCol);
	render.SetColor(textColor);
	render.RenderTextEx(menuFont, &r, ps->text, RT_TOP | RT_LEFT | RT_NOCLIP);
	render.RenderTextExf(menuFont, &r, RT_TOP | RT_RIGHT | RT_NOCLIP, "%.2f", (double)*ps->frac);
}

void GUIRenderer::RenderButton(int sx, int sy, Button *b) {
	RECT r = {sx, sy + 2, sx + b->w, sy + b->h};
	render.Render2DOutlinedRect(sx, sy, b->w, b->h, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	render.RenderTextEx(menuFont, &r, b->text, RT_TOP | RT_CENTER | RT_NOCLIP, textColor);
}

void GUIRenderer::RenderTextBox(int sx, int sy, TextBox *tb) {
	render.Render2DOutlinedRect(sx, sy, tb->w, tb->h, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	RECT r = {sx + 2, sy + 2, sx + tb->w - 4, sy + tb->h - 4};
	render.RenderTextEx(menuFont, &r, tb->text, RT_LEFT | RT_TOP, textColor);
}

void GUIRenderer::RenderListView(int sx, int sy, ListView *lv) {
	render.Render2DOutlinedRect(sx, sy, lv->w, lv->h, CTRLLINETHICKNESS, ctrlFillCol, lineColor);
	render.RenderText(menuFont, sx + 15, sy + 2, lv->text, textColor);
	render.Render2DRect(sx, sy + 2 + menuFont->h, lv->w, CTRLLINETHICKNESS, lineColor);
	render.Render2DTriangle(sx + (lv->w - 15), sy + 5 + menuFont->h, sx + (lv->w - 25), sy + 14 + menuFont->h, sx + (lv->w - 5), sy + 14 + menuFont->h);

	int cy = sy + 2 + menuFont->h + 3;

	for (size_t i = 0; i < lv->items.size(); i++) {
		if ((cy + menuFont->h) > (sy + lv->h)) {
			break;
		}

		if (i != lv->selectedItem) {
			render.RenderText(menuFont, sx + 2, cy, lv->items[i], foreTextCol);
		} else {
			if (!CONTAINSMOUSE(sx + 2, cy, lv->w - 4, menuFont->h + 1)) {
				render.RenderText(menuFont, sx + 2, cy, lv->items[i], backTextCol);
			} else {
				render.RenderText(menuFont, sx + 2, cy, lv->items[i], highlightCol);
			}
		}
		cy += menuFont->h + 1;
	}
}

void GUIRenderer::RenderTreeView(int sx, int sy, TreeView *tv) {
}

void GUIRenderer::RenderTab(int sx, int sy, Tab *t, TabContainer *tc) {
	sy++;
	//render.Render2DRect(sx - 1, sy - 1, t->w + 2, t->h, _outlineCol);
	if (t == tc->selectedTab) {
		//render.Render2DRect(sx, sy, t->w, t->h, foregroundCol);
		//render.Render2DRect(sx, sy, t->w, 1, outlineHlCol2);
		render.RenderText(bigMenuFont, sx + 2, sy, t->text, foreTextCol);
	} else {
		//render.Render2DRect(sx, sy, t->w, t->h, foregroundCol);
		//render.Render2DRect(sx, sy, t->w, 1, outlineHlCol);
		render.RenderText(menuFont, sx + 2, sy, t->text, textColor);
	}
}

void GUIRenderer::RenderTabContainer(int sx, int sy, TabContainer *tc) {
	//render.Render2DRect(sx - 1, sy - 1, tc->w + 2, tc->h + 2, _outlineCol);
	//render.Render2DRect(sx, sy, tc->w, tc->h, foregroundCol);
	//render.Render2DRect(sx, sy, tc->w, 1, outlineHlCol);
	render.Render2DRect(sx, sy, tc->w, 2, foreTextCol);
}

void GUIRenderer::RenderLabel(int sx, int sy, Label *l) {
	render.RenderText(menuFont, sx, sy, l->text, backTextCol);
}

void GUIRenderer::RenderGroupBox(int sx, int sy, GroupBox *gb) {
	render.SetColor(outlineHlCol2);
	int tx = 22 + gb->tw;
	render.Render2DRect(sx + tx, sy + 6, gb->w - tx, 1);
	render.Render2DRect(sx, sy + gb->h, gb->w, 1);
	render.Render2DRect(sx + 1, sy + 6, 19, 1);
	render.Render2DRect(sx, sy + 6, 1, gb->h - 5);
	render.Render2DRect(sx + gb->w, sy + 6, 1, gb->h - 5);
	render.RenderText(menuFont, sx + 22, sy, gb->text, textColor);
}

void GUIRenderer::RenderExpandBox(int sx, int sy, ExpandBox *eb) {
	render.Render2DOutlinedRect(sx, sy, eb->w, eb->h, 1, ctrlFillCol, lineColor);
	if (eb->state == 0) {
		render.Render2DOutlinedRect(sx + 2, sy + 5, 8, 8, 1, ctrlFillCol, lineColor);
		render.Render2DRect(sx + 3, sy + 8, 5, 1, lineColor);
		render.RenderText(menuFont, sx + 14, sy + 1, eb->text, textColor);
	} else {
		render.Render2DOutlinedRect(sx + 2, sy + 5, 8, 8, 1, ctrlFillCol, lineColor);
		render.Render2DRect(sx + 5, sy + 7, 1, 5, lineColor);
		render.Render2DRect(sx + 3, sy + 8, 5, 1, lineColor);
		render.RenderText(menuFont, sx + 14, sy + 1, eb->text, textColor);
	}
}

void GUIRenderer::RenderWindow(Window *w) {
	render.Render2DRect(w->x - OUTLINETHICKNESS, w->y - OUTLINETHICKNESS, w->w + (OUTLINETHICKNESS * 2), w->h + (OUTLINETHICKNESS * 2), outlineCol);
	render.Render2DRect(w->x, w->y, w->w, w->h, backgroundCol);
	if (w->h >= 25) {
		render.Render2DRect(w->x, w->y, w->w, 25, foregroundCol);

		int sx = w->x + (w->w - 25), sy = w->y + 2;
		if (!CONTAINSMOUSE(sx, sy, 20, 5)) {
			render.Render2DRect(sx, sy, 20, 5, foregroundCol);
		} else {
			render.Render2DRect(sx, sy, 20, 5, highlightCol);
		}

		sx = w->x + (w->w - 50);
		if (!CONTAINSMOUSE(sx, sy, 20, 5)) {
			render.Render2DRect(sx, sy, 20, 5, foregroundCol);
		} else {
			render.Render2DRect(sx, sy, 20, 5, highlightCol);
		}
	}
	//render.Render2DRect(w->x, w->y, w->w, 1, outlineHlCol);
	render.RenderText(bigMenuFont, w->x + 10, w->y + 5, w->text, highlightCol);

#ifdef DEV
	if (wm.selectedWindow == w) {
		render.Render2DGradientRect(DIR_UPDOWN, shadowCol, 0, w->x - OUTLINETHICKNESS, w->y + w->h, w->w + (OUTLINETHICKNESS * 2), SHADOWSIZE);
		render.Render2DGradientRect(DIR_LEFTRIGHT, shadowCol, 0, w->x + w->w - OUTLINETHICKNESS, w->y - OUTLINETHICKNESS, SHADOWSIZE, w->h + (OUTLINETHICKNESS * 2));
		render.Render2DGradientRect(DIR_LEFTDOWN, shadowCol, 0, w->x + w->w + OUTLINETHICKNESS, w->y + w->h + OUTLINETHICKNESS, SHADOWSIZE, SHADOWSIZE);
		render.RenderTextf(menuFont, 10, 50, "dx: %i\ndy: %i", mx - wm.selectedWindow->x, my - wm.selectedWindow->y);
		if (wm.selectedElement) {
			render.RenderTextf(menuFont, 10, 80, "x: %i\ny: %i\nw: %i\nh %i", wm.selectedElement->x, wm.selectedElement->y, wm.selectedElement->w, wm.selectedElement->h);
		}
	}
#endif
}

int cdx1 = 6, cdy1 = 8, cdx2 = 13, cdy2 = 10, cdx3, cdy3 = 16;

void GUIRenderer::RenderCursor(int cx, int cy) {
	render.SetColor(foreTextCol);
	render.Render2DTriangle(cx, cy, cx + cdx1, cy + cdy1, cx + cdx2, cy + cdy2);
	render.Render2DTriangle(cx, cy, cx + cdx1, cy + cdy1, cx, cy + cdy3);
	/*render.Render2DTriangle(cx, cy, cx + 6, cy + 8, cx + 16, cy + 10);
	render.Render2DTriangle(cx, cy, cx + 6, cy + 8, cx, cy + 16);*/
	/*render.Render2DLine(cx, cy, cx + 5, cy, 1);
	render.Render2DLine(cx, cy, cx, cy + 5, 1);
	render.Render2DLine(cx, cy, cx + 10, cy + 10, 1);*/
}

int GUIRenderer::GetTitlebarHeight() {
	return 25;
}
