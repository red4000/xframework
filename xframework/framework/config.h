#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include "_vector.h"
#include "vmath.h"

enum {
	FLOAT_CVAR,
	INT_CVAR,
	HEX_CVAR,
	VECTOR3_CVAR,
	VECTOR4_CVAR,
	STRING_CVAR,
	HEADER_CVAR
};

#define FLAG_HEADER_OPEN  0x1000
#define FLAG_NODISPLAY    0x2000
#define DEFAULT_PRECISION 2
#define MAX_CONFIG_SIZE   20000

class CVar {
public:
	CVar(char *name, int flag, float min, float max, float *value, CVar *_parent = NULL);
	CVar(char *name, int flag, float min, float max, float *value, int _precision, CVar *_parent = NULL);
	CVar(char *name, int flag, int min, int max, int *value, CVar *_parent = NULL);
	CVar(char *name, int flag, unsigned int *value, CVar *_parent = NULL);
	CVar(char *name, int flag, FVector3 *min, FVector3 *max, FVector3 *value, CVar *_parent = NULL);
	CVar(char *name, int flag, FVector4 *min, FVector4 *max, FVector4 *value, CVar *_parent = NULL);
	CVar(char *name, int flag, char *value, CVar *_parent = NULL);
	CVar(char *header, CVar *_parent = NULL);

	union {
		float    *fValue;
		int      *iValue;
		DWORD    *hValue;
		FVector3 *vValue;
		FVector4 *v4Value;
		char     *szValue;
	};

	void (*ReadCVar)(CVar *var, char *text);
	void (*OutputCVar)(CVar *var, char *text, size_t maxLen);
	void (*Increment)(CVar *var);
	void (*Decrement)(CVar *var);

	CVar *parent;
	char *name;
	int   flag;
	int   type;
	int   precision;

	union {
		float     fMin;
		int       iMin;
		FVector3 *vMin;
		FVector4 *v4Min;
	};

	union {
		float     fMax;
		int       iMax;
		FVector3 *vMax;
		FVector4 *v4Max;
	};
};

class CVarManager {
public:
	CVarManager();
	~CVarManager();

	void RegisterCVar(CVar *var);
	void UnregisterCVar(CVar *var);
	bool IsVarVisible(CVar *var);
	void GetHeaders(_Vector<CVar*> *result);
	void GetChildren(CVar *header, _Vector<CVar*> *result);

	void LoadConfig(char *configFile);
	void SaveConfig(char *configFile);

	void DumpCVars(char *output, size_t maxLen);

	CVar *GetCVarByName(char *name);
	CVar *GetCVarByName(char *name, CVar *parent);

	_Vector<CVar*> var;
	HANDLE         autoSaveThreadHandle;
};

CVar *GetBaseParent(CVar *var);
void GetIndentForCVar(CVar *var, char *res, size_t len);
void ReadFloatCVar(CVar *var, char *text);
void ReadIntCVar(CVar *var, char *text);
void ReadHexCVar(CVar *var, char *text);
void ReadVectorCVar(CVar *var, char *text);
void ReadFVector4CVar(CVar *var, char *text);
void ReadTextCVar(CVar *var, char *text);

void OutputFloatCVar(CVar *var, char *text, size_t maxLen);
void OutputIntCVar(CVar *var, char *text, size_t maxLen);
void OutputHexCVar(CVar *var, char *text, size_t maxLen);
void OutputVectorCVar(CVar *var, char *text, size_t maxLen);
void OutputFVector4CVar(CVar *var, char *text, size_t maxLen);
void OutputTextCVar(CVar *var, char *text, size_t maxLen);
void OutputHeaderCVar(CVar *var, char *text, size_t maxLen);

void IncrementFloatCVar(CVar *var);
void IncrementIntCVar(CVar *var);
void IncrementOtherCVar(CVar *var);
void IncrementHeaderCVar(CVar *var);

void DecrementFloatCVar(CVar *var);
void DecrementIntCVar(CVar *var);
void DecrementOtherCVar(CVar *var);
void DecrementHeaderCVar(CVar *var);

DWORD WINAPI AutoSave(LPVOID);
void InitializeConfig();
void DestroyConfig();

extern CVarManager manager;

#endif
