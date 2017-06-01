#ifndef WARNING_H
#define WARNING_H

void InitializeWarning();
void WarningPreFrame();
void RenderPlayerWarning(Entity *ent, PlayerInfo *i);
void RenderEntityWarning(Entity *ent);

extern int   playerWarning, entityWarning;
extern float warningThreshold;

#endif
