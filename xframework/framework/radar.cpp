#include "framework.h"
#include "game.h"
#include "render.h"
#include "radar.h"
#include "esp.h"
#include "xguirender.h"

Window *radarWindow;
int     radarEnabled, radarBarrels, radarBarrelLength = 10, radarPlayers = 1, radarExplosives, radarItems, radarX, radarY, radarW = 200, radarH = 200;
float   radarScalar = 5.f;

#define CROSSSIZE 15

void CalculateRadarPoint(FVector3 *point, int *outX, int *outY, int x, int y, int w, int h);

void InitializeRadar() {
	if (radarWindow != NULL) {
		return;
	}
	radarWindow = wm.CreateNewWindow("Radar", radarW, radarH);
	radarWindow->x       = radarX;
	radarWindow->y       = radarY;
	radarWindow->enabled = 0;
}

void ReleaseRadar() {
}

void RenderRadar() {
	radarWindow->Render();
	radarX = radarWindow->x;
	radarY = radarWindow->y + guiRender.GetTitlebarHeight();
	radarH = radarWindow->h - guiRender.GetTitlebarHeight();
	//RenderPlayerOnRadar(localEntity, &playerInfo[localEntity->GetIndex()], friendlyVisCol);
	int hw = radarW / 2, hh = radarH / 2;
	render.SetColor(foregroundCol);
	render.Render2DRect(radarX + hw, radarY + (hh - CROSSSIZE), 1, CROSSSIZE * 2);
	render.Render2DRect(radarX + (hw - CROSSSIZE), radarY + hh, CROSSSIZE * 2, 1);
}

void RenderPlayerOnRadar(Entity *ent, PlayerInfo *pi, _Color col) {
	int x, y;
	CalculateRadarPoint(&pi->origin, &x, &y, radarX, radarY, radarW, radarH);
	render.Render2DOutlinedRect(x - 1, y - 1, 3, 3, 1, col, outlineCol);
	if (radarBarrels) {
		/*
		FVector3 angles;
		ent->GetAngles(&angles);
		angles.Normalize360();
		angles.AngleVectors(&angles, NULL, NULL);
		angles.Scale(radarBarrelLength);
		angles.Add(&pi->head);
		int x2, y2;
		CalculateRadarPoint(&angles, &x2, &y2, radarX, radarY, radarW, radarH);
		render.SetColor(col);
		render.Render2DLine(x, y, x2, y2, 1);
		*/
	}
	if (CONTAINSMOUSE(x, y, 4, 4)) {
		// render extra info
	}
}

void RenderExplosiveOnRadar(Entity *ent) {
}

void RenderItemOnRadar(Entity *ent) {
}

void CalculateRadarPoint(FVector3 *point, int *outX, int *outY, int x, int y, int w, int h) {
	FVector3 delta, fwd, rgt;

	delta = *point;
	delta.Subtract(&viewOrigin);
	delta.z = 0.f;
	delta.Scale(radarScalar);

	fwd.x = 0;
	fwd.y = viewAngles.y - 90.f;
	fwd.z = 0;
	fwd.Normalize180();
	fwd.AngleVectors(&fwd, &rgt, 0);

	int pX = (int)((x + w * 0.5f) + (delta.x * fwd.x + delta.y * fwd.y + delta.z * fwd.z) / (float)w);
	int pY = (int)((y + h * 0.5f) + (delta.x * rgt.x + delta.y * rgt.y + delta.z * rgt.z) / (float)h);

	if (pY < y) {
		pY = y;
	}

	if (pY > y + h) {
		pY = y + h;
	}

	if (pX < x) {
		pX = x;
	}

	if (pX > x + w) {
		pX = x + w;
	}

	(*outX) = pX;
	(*outY) = pY;
}
