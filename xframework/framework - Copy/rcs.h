#ifndef RCS_H
#define RCS_H

bool GetRCSAngle(DVector3 *res);

extern int   rcsEnabled, rcsDelay;
extern float rcsMinLength, rcsStartSpeed, rcsAccel, rcsMaxSpeed, rcsRandom;

#endif
