#ifndef AIM_H
#define AIM_H

float GenerateFrac();
float GetEntityThreat(Entity *ent, PlayerInfo *i);
bool GetAimbotFrameAngles(DVector3 *res);
float GetFOV(FVector3 *from, FVector3 *to);
bool IsInFOV(FVector3 *from, FVector3 *to, float fov);

extern int   saEnabled, saMinDelay, saMaxDelay, saDelay, saMinTime, saMaxTime;
extern float saMinInitialSpeed, saMaxInitialSpeed, saMaxSpeed, saSpeed, saMinAcceleration, saMaxAcceleration, saAcceleration;

#endif
