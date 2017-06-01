#ifndef RADAR_H
#define RADAR_H

#include "xgui.h"

void InitializeRadar();
void ReleaseRadar();
void RenderRadar();
void RenderPlayerOnRadar(Entity *ent, PlayerInfo *pi, _Color col);
void RenderExplosiveOnRadar(Entity *ent);
void RenderItemOnRadar(Entity *ent);

extern Window *radarWindow;
extern int     radarEnabled, radarBarrels, radarBarrelsLength, radarPlayers, radarExplosives, radarItems, radarX, radarY, radarW, radarH;
extern float   radarScalar;

#endif
