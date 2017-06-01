#include "framework.h"
#include "game.h"
#include "_game.h"
#include "prediction.h"
#include "nospread.h"

entityState_s *GetEntityStateByIndex(snapshot_s *snap, int clientNumber);
float predictionScalar = 0.4f, zScalar = 0.4f;

int      oldtime, ocg_frameTime;
FVector3 oldViewOffset;
float    ocg_frameInterpolation;

void PredictionBegin() {
	oldViewOffset.Initialize((FVector3*)0x797648);

	ocg_frameTime = *(int*)0x79445C;
	int newFrameTime;

	newFrameTime = (*(int*)CL_SERVERTIME) - (*(int*)0x794460);

	(*(int*)0x79445C) = newFrameTime;

	double newFrameInterpolation;

	newFrameInterpolation = ((double)((*(int*)CL_SERVERTIME) - cg->snap->serverTime)) / (double)(cg->nextSnap->serverTime - cg->snap->serverTime);

	if (cg->nextSnap->serverTime == cg->snap->serverTime || newFrameInterpolation < 0.0) {
		newFrameInterpolation = 0.0;
	}

	ocg_frameInterpolation = *(float*)0x794458;
	(*(float*)0x794458)    = newFrameInterpolation;

	(*(float*)0x797648) = ((((snapshot_s*)cg->nextSnap)->ps.origin[0] - ((snapshot_s*)cg->snap)->ps.origin[0]) * newFrameInterpolation) + ((snapshot_s*)cg->snap)->ps.origin[0];
	(*(float*)0x79764C) = ((((snapshot_s*)cg->nextSnap)->ps.origin[1] - ((snapshot_s*)cg->snap)->ps.origin[1]) * newFrameInterpolation) + ((snapshot_s*)cg->snap)->ps.origin[1];
	(*(float*)0x797650) = ((((snapshot_s*)cg->nextSnap)->ps.origin[2] - ((snapshot_s*)cg->snap)->ps.origin[2]) * newFrameInterpolation) + ((snapshot_s*)cg->snap)->ps.origin[2] + ((snapshot_s*)cg->nextSnap)->ps.viewHeightCurrent;
}

void PredictionEnd() {
	(*(FVector3*)0x797648) = oldViewOffset;
	(*(float*)0x794458)    = ocg_frameInterpolation;
	(*(int*)0x79445C)      = ocg_frameTime;
}

void CalculateEntityVelocity(Entity *ent, PlayerInfo *pi, DVector3 *res) {
	res->ZeroInitialize();
	
	entityState_s *newEntity = GetEntityStateByIndex((snapshot_s*)cg->nextSnap, ((centity_t*)ent)->nextState.number);
	entityState_s *curEntity = GetEntityStateByIndex((snapshot_s*)cg->snap, ((centity_t*)ent)->nextState.number);

	if (newEntity && curEntity) {
		res->x = predictionScalar * (newEntity->lerp.pos.trBase[0] - curEntity->lerp.pos.trBase[0]);
		res->y = predictionScalar * (newEntity->lerp.pos.trBase[1] - curEntity->lerp.pos.trBase[1]);
		res->z = predictionScalar * zScalar * (newEntity->lerp.pos.trBase[2] - curEntity->lerp.pos.trBase[2]);
	} else {
		res->x = predictionScalar * (((centity_t*)ent)->nextState.lerp.pos.trBase[0] - ((centity_t*)ent)->currentState.pos.trBase[0]);
		res->y = predictionScalar * (((centity_t*)ent)->nextState.lerp.pos.trBase[1] - ((centity_t*)ent)->currentState.pos.trBase[1]);
		res->z = predictionScalar * zScalar * (((centity_t*)ent)->nextState.lerp.pos.trBase[2] - ((centity_t*)ent)->currentState.pos.trBase[2]);
	}

	return;
}

void CalculateEntityTVelocity(Entity *ent, TVec3 *res) {
	entityState_s *newEntity = GetEntityStateByIndex((snapshot_s*)cg->nextSnap, ((centity_t*)ent)->nextState.number);
	entityState_s *curEntity = GetEntityStateByIndex((snapshot_s*)cg->snap, ((centity_t*)ent)->nextState.number);

	if (newEntity && curEntity) {
		TVec3 base;
		FVec3ToTVec3(&newEntity->lerp.pos.trBase[0], res);
		FVec3ToTVec3(&curEntity->lerp.pos.trBase[0], &base);
		TVec3Subtract(res, &base);
		FVector3 scalar(predictionScalar, predictionScalar, predictionScalar * zScalar);
		FVec3ToTVec3(&scalar[0], &base);
		TVec3Multiply(res, &base);
		//TVec3MultiplyTF(res, predictionScalar);
	} else {
		TVec3 base;
		FVec3ToTVec3(&((centity_t*)ent)->nextState.lerp.pos.trBase[0], res);
		FVec3ToTVec3(&((centity_t*)ent)->currentState.pos.trBase[0], &base);
		TVec3Subtract(res, &base);
		FVector3 scalar(predictionScalar, predictionScalar, predictionScalar * zScalar);
		FVec3ToTVec3(&scalar[0], &base);
		TVec3Multiply(res, &base);
		//TVec3MultiplyTF(res, predictionScalar);
	}
}

entityState_s *GetEntityStateByIndex(snapshot_s *snap, int clientNumber) {
	if (NULL == snap) {
		return NULL;
	}

	for (int i = 0; i < snap->numEntities; i++) {
		if (snap->entities[i].number == clientNumber /* && snap->entities[i].eType == TYPE_HUMAN && GetClientStateByIndex(snap, clientNumber)*/) {
			return &snap->entities[i];
		}
	}

	return NULL;
}
