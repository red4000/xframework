#ifndef AIM_H
#define AIM_H

float GetEntityThreat(Entity *ent, PlayerInfo *i);
float GenerateFrac();
bool GetAimbotFrameAngles(DVector3 *res);
float GetFOV(FVector3 *from, FVector3 *to, float fovX = 180.f, float fovY = 180.f);
bool IsInFOV(FVector3 *from, FVector3 *to, float fovX, float fovY);

extern int   saEnabled, saMinDelay, saMaxDelay, saDelay, saMinTime, saMaxTime;
extern float saMinInitialSpeed, saMaxInitialSpeed, saMaxSpeed, saSpeed, saMinAcceleration, saMaxAcceleration, saAcceleration;

#endif
