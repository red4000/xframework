#ifndef TZAR_H
#define TZAR_H

typedef char TFloat[10];

#pragma pack(push, 1)
class TVec3 {
public:
	TFloat x, y, z;
};
#pragma pack(pop)

extern "C" {
void __stdcall FVec3ToTVec3(float *pfVec3, TVec3 *ptVec3);
void __stdcall DVec3ToTVec3(double *pdVec3, TVec3 *ptVec3);
void __stdcall TVec3ToFVec3(TVec3 *pfVec3, float *ptVec3);
void __stdcall TVec3ToDVec3(TVec3 *pfVec3, double *ptVec3);
void __stdcall TVec3Multiply(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 *= ptVec3
void __stdcall TVec3MultiplyT(TVec3 *pfVec3, TFloat *ptVec); // pfVec3 *= ptVec
void __stdcall TVec3MultiplyTF(TVec3 *pfVec3, float ptVec); // pfVec3 *= fVec
void __stdcall TVec3Subtract(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 -= ptVec3
void __stdcall TVec3Add(TVec3 *pfVec3, TVec3 *ptVec3); // pfVec3 += ptVec3
void __stdcall TVec3Lerp(TVec3 *pfVec3, TVec3 *ptVec3, TFloat *ptA); // *pf = (*pf * (1.0 - *ptA)) + (*pt * *ptA)
void __stdcall TVec3Length(TVec3 *ptVec3, TFloat *poutt);
void __stdcall TVec3Distance(TVec3 *pfVec3, TVec3 *ptVec3, TFloat *poutt);
void __stdcall TVec3Unitize(TVec3 *pVec3);
void __stdcall TVec3Normalize180(TVec3 *pVec3);
void __stdcall TVec3Normalize360(TVec3 *pVec3);
void __stdcall TVec3Angles(TFloat *ptX, TFloat *ptY, TFloat *ptZ);
}

#endif
