#ifndef ENGINE__GAME_H
#define ENGINE__GAME_H

class clientInfo_t;
class centity_t;
class LerpEntityState;
class entityState_t;
class trajectory_t;
class refdef_t;
class cg_t;
class snapshot_t;

#include "vmath.h"

#pragma pack(push, 2)
class trace_t
{
public:
	float            fraction;
	FVector3         normal;
	int              surfaceFlags;
	int              contents;
	const char      *material;
	int              hitType;
	unsigned __int16 hitId;
	unsigned __int16 modelIndex;
	unsigned __int16 partName;
	unsigned __int16 partGroup;
	unsigned int     allsolid;
	unsigned int     startsolid;
	unsigned int     walkable;
	BYTE             unknown2[2];
	FVector3         end;
	int              hitSelf;
	int              materialType;
	BYTE             padding[20];
};
#pragma pack(pop)

class clientInfo_t
{
public:
	__int32 infoValid; //0x0000 
	__int32 nextValid; //0x0004 
	__int32 clientNum; //0x0008 
	char    name[16]; //0x000C 
	__int32 team; //0x001C 
	__int32 oldteam; //0x0020 
	char    _0x0024[1192];
};//Size=0x048C

class trajectory_t
{
public:
	__int32  trType; //0x0000
	__int32  trTime; //0x0004
	__int32  trDuration; //0x0008
	FVector3 trBase; //0x000C
	FVector3 trDelta; //0x0018
}; //Size=0x0024

class LerpEntityState
{
public:
	__int32      eFlags; //0x0000 
	trajectory_t pos; //0x0004 
	trajectory_t apos; //0x0028 
	char         _0x004C[28];

};//Size=0x0068

class entityState_t
{
public:
	__int32         number; //0x0000 
	__int32         eType; //0x0004 
	LerpEntityState lerp; //0x0008 
	__int32         time2; //0x0070 
	__int32         otherEntityNum; //0x0074 
	__int32         attackerEntityNum; //0x0078 
	__int32         groundEntityNum; //0x007C 
	__int32         loopSound; //0x0080 
	__int32         surfType; //0x0084 
	__int32         brushModel; //0x0088 
	__int32         item; //0x008C 
	__int32         xmodel; //0x0090 
	__int32         primaryLight; //0x0094 
	char            _0x0098[4];
	__int32         iHeadIcon; //0x009C 
	__int32         iHeadIconTeam; //0x00A0 
	__int32         solid; //0x00A4 
	DWORD           eventParm; //0x00A8 
	__int32         eventSequence; //0x00AC 
	char            _0x00B0[20];
	__int32         weapon; //0x00C4 
	__int32         weaponModel; //0x00C8 
	__int32         legsAnim; //0x00CC 
	__int32         torsoAnim; //0x00D0 
	__int32         scale; //0x00D4 
	__int32         eventParm2; //0x00D8 
	__int32         helicopterStage; //0x00DC 
	__int32         hintString; //0x00E0 
	__int32         grenadeInPickupRange; //0x00E4 
	__int32         vehicleXModel; //0x00E8 
	float           fTorsoPitch; //0x00EC 
	float           fWaistPitch; //0x00F0 
};//Size=0x00F4

class centity_t
{
public:
	unsigned long   lightingHandle; //0x0000
	char            _0x0004[24];
	FVector3        origin; //0x001C 
	FVector3        angles; //0x0028 
	char            _0x0034[48];
	LerpEntityState currentState; //0x0064 
	entityState_t   nextState; //0x00CC 
	DWORD           tree; //0x01C0 
	char            _0x01C4[24];
};//Size=0x01DC

class refdef_t
{
public:
	__int32  x; //0x0000 
	__int32  y; //0x0004 
	__int32  width; //0x0008 
	__int32  height; //0x000C 
	float    fovX; //0x0010 
	float    fovY; //0x0014 
	FVector3 vieworg; //0x0018 
	FVector3 viewaxis_0; //0x0024 
	FVector3 viewaxis_1; //0x0030 
	FVector3 viewaxis_2; //0x003C 
	FVector3 viewOffset; //0x0048 
	__int32  time; //0x0054 
};//Size=0x0058

class cg_t
{
public:
	__int32     clientNum; //0x0000 
	__int32     localClientNum; //0x0004 
	__int32     demoType; //0x0008 
	char        _0x000C[20];
	snapshot_t *snap; //0x0020 
	snapshot_t *nextSnap; //0x0024 
};//Size=0x0028

class snapshot_t
{
public:
	__int32 snapFlags; //0x0000 
	__int32 ping; //0x0004 
	__int32 serverTime; //0x0008 
};//Size=0x000C

class entityState_s
{
public:
	__int32         number; //0x0000
	__int32         eType; //0x0004
	LerpEntityState lerp; //0x0008  lerp
	__int32         time2; //0x0070
	__int32         otherEntityNum; //0x0074
	__int32         attackerEntityNum; //0x0078
	__int32         groundEntityNum; //0x007C
	__int32         loopSound; //0x0080
	__int32         surfType; //0x0084
	__int32         brushmodel; //0x0088
	__int32         item; //0x008C
	__int32         xmodel; //0x0090
	__int32         primaryLight; //0x0094
	char            _0x0098[4]; //__int32 clientNum; //0x0098
	__int32         iHeadIcon; //0x009C
	__int32         iHeadIconTeam; //0x00A0
	__int32         solid; //0x00A4
	unsigned long   eventParm; //0x00A8
	__int32         eventSequence; //0x00AC
	char            _0x00B0[20];
	__int32         weapon; //0x00C4
	__int32         weaponModel; //0x00C8
	__int32         legsAnim; //0x00CC
	__int32         torsoAnim; //0x00D0
	__int32         scale; //0x00D4
	__int32         eventParm2; //0x00D8
	__int32         helicopterStage; //0x00DC
	__int32         hintString; //0x00E0
	__int32         grenadeInPickupRange; //0x00E4
	__int32         vehicleXModel; //0x00E8
	float           fTorsoPitch; //0x00EC
	float           fWaistPitch; //0x00F0
}; //Size=0x00F4

class SprintState_t
{
public:
	__int32 sprintButtonUpRequired; //0x0000
	__int32 sprintDelay; //0x0004
	__int32 lastSprintStart; //0x0008
	__int32 lastSprintEnd; //0x000C
	__int32 sprintStartMaxLength; //0x0010
}; //Size=0x0014

class MantleState
{
public:
	float   yaw; //0x0000
	__int32 timer; //0x0004
	__int32 transIndex; //0x0008
	__int32 flags; //0x000C
}; //Size=0x0010

class objective_t
{
public:
	__int32  state; //0x0000
	FVector3 origin; //0x0004
	__int32  icon; //0x0010
	__int32  teamNum; //0x0014
	__int32  N0051B8E5; //0x0018
}; //Size=0x001C

class hudelem_t
{
public:
	char          _0x0000[12];
	float         N0051B8E6; //0x000C
	float         N0051B8E7; //0x0010
	float         N0051B8E8; //0x0014
	float         N0051B8E9; //0x0018
	__int32       N0051B8EA; //0x001C
	__int32       N0051B8EB; //0x0020
	__int32       N0051B8EC; //0x0024
	unsigned long color; //0x0028
	char          _0x002C[148];
	float         N0084EC80; //0x00C0
	unsigned long glowColor; //0x00C4
	char          _0x00C8[120];
}; //Size=0x0140

class playerState_s
{
public:
	__int32       commandTime; //0x0000
	__int32       pm_type; //0x0004
	__int32       bobCycle; //0x0008
	__int32       pm_flags; //0x000C
	__int32       weapFlags; //0x0010
	__int32       otherFlags; //0x0014
	__int32       pm_time; //0x0018
	FVector3      origin; //0x001C
	FVector3      velocity; //0x0028
	float         oldVelocity[2]; //0x0034
	__int32       weaponTime; //0x003C
	__int32       weaponDelay; //0x0040
	__int32       grenadeTimeLeft; //0x0044
	__int32       throwBackGrenadeOwner; //0x0048
	__int32       throwBackGrenadeTimeLeft; //0x004C
	__int32       weaponRestrictKickTime; //0x0050
	__int32       foliageSoundTime; //0x0054
	__int32       gravity; //0x0058
	float         leanf; //0x005C
	__int32       speed; //0x0060
	FVector3      delta_angles; //0x0064
	__int32       groundEntityNum; //0x0070
	FVector3      vLadderVec; //0x0074
	__int32       jumpTime; //0x0080
	float         jumpOriginZ; //0x0084
	__int32       legsTimer; //0x0088
	__int32       legsAnim; //0x008C
	__int32       torsoTimer; //0x0090
	__int32       torsoAnim; //0x0094
	__int32       legsAnimDuration; //0x0098
	__int32       torsoAnimDuration; //0x009C
	__int32       damageTimer; //0x00A0
	__int32       damageDuration; //0x00A4
	__int32       flinchYawAnim; //0x00A8
	__int32       movementDir; //0x00AC  -180..180 angle
	__int32       eFlags; //0x00B0
	__int32       eventSequence; //0x00B4
	int           events[4]; //0x00B8
	unsigned int  eventParms[4]; //0x00C8
	__int32       oldEventSequence; //0x00D8
	__int32       clientNum; //0x00DC
	__int32       offHandIndex; //0x00E0
	__int32       offhandSecondary; //0x00E4
	unsigned long weapon; //0x00E8
	__int32       weaponstate; //0x00EC
	unsigned long weaponShotCount; //0x00F0
	float         fWeaponPosFrac; //0x00F4  aka zoom fraction
	__int32       adsDelayTime; //0x00F8
	__int32       spreadOverride; //0x00FC
	__int32       spreadOverrideState; //0x0100
	__int32       viewmodelIndex; //0x0104
	FVector3      viewangles; //0x0108
	__int32       viewHeightTarget; //0x0114
	float         viewHeightCurrent; //0x0118
	__int32       viewHeightLerpTime; //0x011C
	__int32       viewHeightLerpTarget; //0x0120
	__int32       viewHeightLerpDown; //0x0124
	float         viewAngleClampBase[2]; //0x0128
	float         viewAngleClampRange[2]; //0x0130
	__int32       damageEvent; //0x0138
	__int32       damageYaw; //0x013C
	__int32       damagePitch; //0x0140
	__int32       damageCount; //0x0144
	int           stats[5]; //0x0148
	int           ammo[128]; //0x015C
	int           ammoclip[128]; //0x035C
	unsigned int  weapons[4]; //0x055C
	unsigned int  weaponold[4]; //0x056C
	unsigned int  weaponrechamber[4]; //0x057C
	float         proneDirection; //0x058C
	float         proneDirectionPitch; //0x0590
	float         proneTorsoPitch; //0x0594
	__int32       viewlocked; //0x0598
	__int32       viewlocked_entNum; //0x059C
	__int32       cursorHint; //0x05A0
	__int32       cursorHintString; //0x05A4
	__int32       cursorHintEntIndex; //0x05A8
	__int32       iCompassPlayerInfo; //0x05AC
	__int32       radarEnabled; //0x05B0
	__int32       locationSelectionInfo; //0x05B4
	SprintState_t sprintState; //0x05B8  sprintState
	float         fTorsoPitch; //0x05CC
	float         fWaistPitch; //0x05D0
	float         holdBreathScale; //0x05D4
	__int32       holdBreathTimer; //0x05D8
	float         moveSpeedScaleMultiplier; //0x05DC
	MantleState   mantleState; //0x05E0  mantleState
	float         meleeChargeYaw; //0x05F0
	__int32       meleeChargeDist; //0x05F4
	__int32       meleeChargeTime; //0x05F8
	__int32       perks; //0x05FC
	int           actionSlotType[4]; //0x0600
	int           actionSlotParm[4]; //0x0610
	__int32       entityEventSequence; //0x0620
	__int32       weapAnim; //0x0624
	float         aimSpreadScale; //0x0628
	__int32       shellshockIndex; //0x062C
	__int32       shellshockTime; //0x0630
	__int32       shellshockDuration; //0x0634
	float         dofNearStart; //0x0638
	float         dofNearEnd; //0x063C
	float         dofFarStart; //0x0640
	float         dofFarEnd; //0x0644
	float         dofNearBlur; //0x0648
	float         dofFarBlur; //0x064C  shit goes haywire somewhere between here and the end of the
	__int32       hudElemLastAssignedSoundID; //0x0650  struct, but this shit isn't important anyways
	char          _0x0654[8];
	objective_t   objective[16]; //0x065C
	char          weaponmodels[128]; //0x081C
	char          _0x089C[4];
	__int32       deltaTime; //0x08A0
	hudelem_t     current[31]; //0x08A4
}; //Size=0x2F64

class clientState_s
{
public:
	__int32 clientIndex; //0x0000
	__int32 team; //0x0004
	__int32 modelindex; //0x0008
	int     attachModelIndex[6]; //0x000C
	int     attachTagIndex[6]; //0x0024
	char    name[32]; //0x003C
	float   maxSprintTimeMultiplier; //0x005C
	__int32 rank; //0x0060
}; //Size=0x0064

class snapshot_s
{
public:
	__int32       snapFlags; //0x0000
	__int32       ping; //0x0004
	__int32       serverTime; //0x0008
	playerState_s ps; //0x000C  ps
	__int32       numEntities; //0x2F70
	__int32       numClients; //0x2F74
	entityState_s entities[512]; //0x2F78
	clientState_s clients[64]; //0x21778
	__int32       serverCommandSequence; //0x23078
}; //Size=0x2307C

class usercmd_s
{
public:
	__int32 serverTime; //0x0000
	__int32 buttons; //0x0004
	int     angles[3]; //0x0008
	__int8  weapon; //0x0014
	__int8  offHandIndex; //0x0015
	__int8  forwardmove; //0x0016
	__int8  rightmove; //0x0017
	float   meleeChargeYaw; //0x0018
	__int8  meleeChargeDist; //0x001C
	char    _0x001D[3];
}; //Size=0x0020

class BulletFireParams
{
public:
	int      weaponEntIndex; // 0
	int      ignoreEntIndex; // 4
	float    damageMultiplier; // 8
	int      methodOfDeath; // 12
	FVector3 origStart; // 16
	FVector3 start; // 28
	FVector3 end; // 44
	FVector3 dir; // 56
};

extern clientInfo_t *clientinfo;
extern cg_t         *cg;
extern refdef_t     *refdef;
extern centity_t    *cg_entities;

#endif
