#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "framework.h"
#include "hook.h"
#include "menu.h"
#include "render.h"
#include "game.h"
#include "esp.h"
#include "visual.h"
#include "warning.h"
#include "sesp.h"
#include "dodge.h"
#include "radar.h"

void InitializeHack() {
	HookGame();
	HookRenderer();
	HookInput("CoD4");
	DEBUG_LOG("Framework initialized\n"); 
}

void DestroyHack() {
	UnhookInput();
	UnhookGame();
	UnhookRenderer();
	DestroyMenu();
}

extern int testInt;

void RenderHackFrame() {
	if (radarEnabled) {
		RenderRadar();
	}

	/*
	for (auto it = entityInfo.begin(); it != entityInfo.end(); it++) {
		// crashes somewhere
		switch (it->type)
		{
		default:
			break;

		case ET_WEAPON:
			RenderItemESP(it->ent, it);
			break;

		case ET_EXPLOSIVE:
			if (entityWarning) {
				RenderEntityWarning(it->ent);
			}
			RenderExplosiveESP(it->ent, it);
			break;
		}
	}
	*/

	for (size_t i = 0; i < MAX_PLAYERS; i++) {
		Entity     *ent = GetEntityByIndex(i);
		PlayerInfo *pi  = &playerInfo[i];
		if (ent->IsAlive() && !ent->IsLocalEntity()) {
			if (playerWarning) {
				if (ent->IsHostile()) {
					RenderPlayerWarning(ent, pi);
				}
			}
			RenderPlayerESP(i, ent, pi);
			if (soundEsp) {
				PlaySoundESP(ent, pi);
			}
			if (dodgeEnabled) {
				DodgeEntity(ent, pi);
			}
		}
	}

	RenderVisuals();
}
