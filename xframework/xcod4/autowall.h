#ifndef AUTOWALL_H
#define AUTOWALL_H

LONG WINAPI HandleAWException(EXCEPTION_POINTERS *ex);
float CalculatePower(Entity *ent, FVector3 *start, FVector3 *end);

extern int   autoWall;
extern float autoWallThreshold;

#endif
