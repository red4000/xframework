#include "framework.h"
#include "game.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

int soundEsp, soundEspDelay;
float soundEspFOV = 5.f;

DWORD soundTime;

void PlaySoundESP(Entity *ent, PlayerInfo *i) {
	if (i->fov <= soundEspFOV) {
		PlaySoundA("tick.wav", NULL, SND_ASYNC);
		//PlaySoundA(NULL, NULL, 0); // stop sound
	}
}
