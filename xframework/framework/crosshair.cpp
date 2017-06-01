#include "framework.h"
#include "vmath.h"
#include "render.h"

int    crosshairEnabled = 0, crosshairType, crosshairSize = 15, crosshairThickness = 1;
_Color crosshairCol = D3DCOLOR_ARGB(255, 200, 200, 200), crosshairCol2 = D3DCOLOR_ARGB(255, 150, 150, 150);

void RenderCrosshair() {
	switch (crosshairType) {
	default:
	case 0:
		render.SetColor(crosshairCol);
		render.Render2DRect(((int)(render.sW * 0.5f)) - crosshairSize, ((int)(render.sH * 0.5f)), crosshairSize * 2, crosshairThickness);
		render.Render2DRect(((int)(render.sW * 0.5f)), ((int)(render.sH * 0.5f)) - crosshairSize, crosshairThickness, crosshairSize * 2);
		break;
	}
}
