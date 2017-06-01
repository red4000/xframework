#ifndef ESP_H
#define ESP_H

#include "drender.h"

void InitializeESP();
void ReleaseESP();
void EspPreFrame();
bool CalcPlayerRect(Entity *ent, PlayerInfo *pi);
void RenderPlayerESP(int i, Entity *ent, PlayerInfo *pi);
void RenderExplosiveESP(Entity *ent, EntityInfo *i);
void RenderItemESP(Entity *ent, EntityInfo *i);
_Color GetESPColor(Entity *ent, PlayerInfo *pi);

extern _Color espEnemyVisCol, espEnemyInvisCol, espFriendlyVisCol, espFriendlyInvisCol, explosiveVisCol, explosiveInvisCol, itemVisCol, itemInvisCol, shadowTextCol;
extern int    barrelEsp, barrelLength, boxEsp, boxHairs, boxEdges, pixelEsp, pixelSize, distanceEsp, explosiveEsp, itemEsp, enemiesOnly, nameEsp, weaponEsp, threatEsp, espStyle;
extern float  boxEdgeFrac;
extern Font  *espFont;

#endif
