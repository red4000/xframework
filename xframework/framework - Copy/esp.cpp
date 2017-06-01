#include "framework.h"
#include "drender.h"
#include "game.h"
#include <radar.h>

_Color enemyVisCol       = D3DCOLOR_ARGB(255, 255, 25, 25),
	   enemyInvisCol     = D3DCOLOR_ARGB(255, 25, 255, 25),
	   friendlyVisCol    = D3DCOLOR_ARGB(255, 20, 200, 20),
	   friendlyInvisCol  = D3DCOLOR_ARGB(255, 20, 20, 255),
	   explosiveVisCol   = D3DCOLOR_ARGB(255, 255, 20, 255),
	   explosiveInvisCol = D3DCOLOR_ARGB(255, 220, 60, 220),
	   itemVisCol        = D3DCOLOR_ARGB(255, 220, 220, 220),
	   itemInvisCol      = D3DCOLOR_ARGB(255, 150, 150, 150),
	   shadowTextCol     = D3DCOLOR_ARGB(255, 30, 30, 30);

int      barrelEsp, barrelLength = 250, boxEsp, distanceEsp, explosiveEsp, itemEsp, enemiesOnly, nameEsp, weaponEsp, threatEsp, espStyle;
Font    *espFont;
FVector3 leftVector, bottomVec;

void InitializeESP() {
	espFont = render.CreateNewFont("Courier New", 13, FL_BOLD);
}

void ReleaseESP() {
}

void EspPreFrame() {
	bottomVec = viewAngles;
	bottomVec.AngleVectors(NULL, &leftVector, NULL);
	bottomVec = leftVector;
#define BOXRADIUS 13.0f
	bottomVec.Scale(BOXRADIUS);
	leftVector.Scale(-1.0f * BOXRADIUS);
}

void RenderBarrel(FVector3 *start, FVector3 *angles, float len) {
	FVector3 end;
	angles->AngleVectors(&end, NULL, NULL);
	end.Scale(len);
	end.Add(start);
	//render.SetColor(c);
	//render.Render3DLine(&start, &end);
}

bool CalcPlayerRect(Entity *ent, PlayerInfo *i) {
	FVector3 top, bottom;

	top = i->head;
	top.Add(&bottomVec);
	bottom = i->origin;
	bottom.Add(&leftVector);
	i->rect.left = 0;
	
	int bottomX, bottomY, topX, topY;
	if (WorldToScreen(&bottom, &bottomX, &bottomY)) {
		if (WorldToScreen(&top, &topX, &topY)) {
			if (topX < bottomX) {
				i->rect.left  = topX;
				i->rect.right = bottomX;
			} else {
				i->rect.left  = bottomX;
				i->rect.right = topX;
			}
			if (topY < bottomY) {
				i->rect.top    = topY;
				i->rect.bottom = bottomY;
			} else {
				i->rect.top    = bottomY;
				i->rect.bottom = topY;
			}
			//i->rect.right = i->rect.left + (0.5f * (i->rect.bottom - i->rect.top));
			return true;
		}
	}
	return false;
}

void RenderPlayerESP(int i, Entity *ent, PlayerInfo *pi) {
	_Color entCol;
		
	if (ent->IsHostile()) {
		if (pi->visible) {
			entCol = enemyVisCol;
		} else {
			entCol = enemyInvisCol;
		}
	} else {
		if (enemiesOnly) {
			return;
		}
		if (pi->visible) {
			entCol = friendlyVisCol;
		} else {
			entCol = friendlyInvisCol;
		}
	}

	if (pi->rect.left) {
		char espText[128];
		espText[0] = 0;
		if (nameEsp) {
			strcat_s(espText, sizeof(espText), ent->GetName());
			strcat_s(espText, sizeof(espText), "\n");
		}
		if (distanceEsp) {
			char distanceString[32];
			sprintf_s(distanceString, sizeof(distanceString), "%.1f\n", viewOrigin.Distance(&pi->head) * (1.0 / 47.0));
			strcat_s(espText, sizeof(espText), distanceString);
		}
		if (weaponEsp) {
			strcat_s(espText, sizeof(espText), ent->GetWeaponName());
			strcat_s(espText, sizeof(espText), "\n");
		}
		if (threatEsp) {
			char threatString[32];
			sprintf_s(threatString, sizeof(threatString), "%.1f\n", pi->threat);
			strcat_s(espText, sizeof(espText), threatString);
		}

		if (boxEsp) {
			int bw = pi->rect.right - pi->rect.left,
				bh = pi->rect.bottom - pi->rect.top;
			render.Render2DRectEdges(pi->rect.left + 1, pi->rect.top + 1, bw, bh, 1, shadowTextCol);
			render.Render2DRectEdges(pi->rect.left, pi->rect.top, bw, bh, 1, entCol);
			if (espText[0]) {
				render.RenderText(espFont, pi->rect.right + 1, pi->rect.top + (0.5f * (bh)) + 1, espText, shadowTextCol);
				render.RenderText(espFont, pi->rect.right, pi->rect.top + (0.5f * (bh)), espText, entCol);
			}
		} else {
			if (espText[0]) {
				int bw = pi->rect.right - pi->rect.left;
				render.RenderCenteredText(espFont, pi->rect.left + (0.5f * (bw)) + 1, pi->rect.top + 1, espText, shadowTextCol);
				render.RenderCenteredText(espFont, pi->rect.left + (0.5f * (bw)), pi->rect.top, espText, entCol);
			}
		}
	}
	if (radarEnabled && radarPlayers) {
		RenderPlayerOnRadar(ent, pi, entCol);
	}
}

void RenderExplosiveESP(Entity *ent, EntityInfo *i) {
	if (explosiveEsp) {
		if (i->x != -1) {
			_Color col;

			if (IsPointVisible(&viewOrigin, &i->origin)) {
				col = explosiveVisCol;
			} else {
				col = explosiveInvisCol;
			}

			render.RenderCenteredText(espFont, i->x + 1, i->y + 1, ent->GetWeaponName(), shadowTextCol);
			render.RenderCenteredText(espFont, i->x, i->y, ent->GetWeaponName(), col);
		}
	}
	if (radarEnabled && radarExplosives) {
		RenderExplosiveOnRadar(ent);
	}
}

void RenderItemESP(Entity *ent, EntityInfo *i) {
	if (itemEsp) {
		if (i->x != -1) {
			_Color col;

			if (IsPointVisible(&viewOrigin, &i->origin)) {
				col = itemVisCol;
			} else {
				col = itemInvisCol;
			}

			render.RenderCenteredText(espFont, i->x + 1, i->y + 1, ent->GetWeaponName(), shadowTextCol);
			render.RenderCenteredText(espFont, i->x, i->y, ent->GetWeaponName(), col);
		}
	}
	if (radarEnabled && radarItems) {
		RenderItemOnRadar(ent);
	}
}

_Color GetESPColor(Entity *ent, PlayerInfo *pi) {
	return 0;
}
