#ifndef PREDICTION_H
#define PREDICTION_H

#include "tzar.h"

void PredictionBegin();
void PredictionEnd();
void CalculateEntityVelocity(Entity *ent, PlayerInfo *pi, DVector3 *res);
void CalculateEntityTVelocity(Entity *ent, TVec3 *res);

extern float predictionScalar;

#endif
