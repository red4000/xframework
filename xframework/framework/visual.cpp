#include "framework.h"
#include "crosshair.h"
#include "clock.h"

extern int c1,c2,c3,c4,c5;

void RenderVisuals() {
	/*
	if (c4) {
		render.Render2DRect(5, 5, 40, 40, D3DCOLOR_ARGB(255,0,0,0));
	}
	if (c3) {
		render.Render2DRect(10, 10, 20, 20, D3DCOLOR_ARGB(255,255,0,0));
	}
	if (c2) {
		render.Render2DRect(15, 15, 10, 10, D3DCOLOR_ARGB(255,0,0,255));
	}
	if (c5) {
		render.Render2DRect(15, 25, 15, 15, D3DCOLOR_ARGB(0,0,255,255));
	}
	render.SetColor(D3DCOLOR_ARGB(255,255,255,255));
	render.RenderTextf(guiRender.menuFont, 10, 20, "c1 %i\nc2 %i\nc3 %i\nc4 %i\nc5 %i", c1, c2, c3, c4, c5);
	*/

	if (crosshairEnabled) {
		RenderCrosshair();
	}
}
