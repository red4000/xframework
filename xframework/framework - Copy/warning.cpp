#include "framework.h"
#include "game.h"
#include "drender.h"
#include "esp.h"

int   playerWarning, entityWarning;
float warningThreshold = 10.f;
int   wy;
Font *warningFont;

void InitializeWarning() {
	warningFont = render.CreateNewFont("Courier New", 19, FL_BOLD);
}

void WarningPreFrame() {
	wy = render.sH * 0.15f;
}

void RenderPlayerWarning(Entity *ent, PlayerInfo *i) {
	if (i->threat < warningThreshold) {
		char warningStr[128];
		sprintf_s(warningStr, sizeof(warningStr), "%3.1f %s %s", (double)i->threat, ent->GetName(), ent->GetWeaponName());
		render.RenderCenteredText(warningFont, (render.sW / 2) + 1, wy + 1, warningStr, D3DCOLOR_ARGB(255, 20, 20, 20));
		render.RenderCenteredText(warningFont, render.sW / 2, wy, warningStr, D3DCOLOR_ARGB(255, 250, 100, 100));
		wy += warningFont->h;
   }
}

void RenderEntityWarning(Entity *ent) {
}
