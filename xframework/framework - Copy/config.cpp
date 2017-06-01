#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <algorithm>
#include <cstdio>
#include "config.h"
//#include "main.h"
#include <math.h>

#define INITIALIZE_CVAR(cMin, cMax, cValue, ReadFunction, OutputFunction, IncrementFunction, DecrementFunction, cType) this->name = name; this->flag = flag; this->cMin = min; this->cMax = max; this->cValue = value; this->type = cType; \
	this->ReadCVar                                                                                                            = ReadFunction; this->OutputCVar = OutputFunction; this->Increment = IncrementFunction; this->Decrement = DecrementFunction; manager.RegisterCVar(this)

CVarManager manager;
HANDLE      autoSaveThreadHandle;
/*
CVar::CVar(char *name, int flag, float min, float max, float *value) {
	INITIALIZE_CVAR(fMin, fMax, fValue, ReadFloatCVar, OutputFloatCVar, IncrementFloatCVar, DecrementFloatCVar, FLOAT_CVAR);
}

CVar::CVar(char *name, int flag, int min, int max, int *value) {
	INITIALIZE_CVAR(iMin, iMax, iValue, ReadIntCVar, OutputIntCVar, IncrementIntCVar, DecrementIntCVar, INT_CVAR);
}

CVar::CVar(char *name, int flag, FVector3 *min, FVector3 *max, FVector3 *value) {
	INITIALIZE_CVAR(vMin, vMax, vValue, ReadVectorCVar, OutputVectorCVar, IncrementOtherCVar, DecrementOtherCVar, VECTOR3_CVAR);
}

CVar::CVar(char *name, int flag, FVector4 *min, FVector4 *max, FVector4 *value) {
	INITIALIZE_CVAR(v4Min, v4Max, v4Value, ReadFVector4CVar, OutputFVector4CVar, IncrementOtherCVar, DecrementOtherCVar, VECTOR4_CVAR);
}
 */

CVar::CVar(char *name, int flag, float min, float max, float *value, CVar *_parent) {
	INITIALIZE_CVAR(fMin, fMax, fValue, ReadFloatCVar, OutputFloatCVar, IncrementFloatCVar, DecrementFloatCVar, FLOAT_CVAR);
	parent    = _parent;
	precision = DEFAULT_PRECISION;
}

CVar::CVar(char *name, int flag, float min, float max, float *value, int _precision, CVar *_parent) {
	INITIALIZE_CVAR(fMin, fMax, fValue, ReadFloatCVar, OutputFloatCVar, IncrementFloatCVar, DecrementFloatCVar, FLOAT_CVAR);
	parent    = _parent;
	precision = _precision;
}

CVar::CVar(char *name, int flag, int min, int max, int *value, CVar *_parent) {
	INITIALIZE_CVAR(iMin, iMax, iValue, ReadIntCVar, OutputIntCVar, IncrementIntCVar, DecrementIntCVar, INT_CVAR);
	parent    = _parent;
	precision = DEFAULT_PRECISION;
}

CVar::CVar(char *name, int flag, unsigned int *value, CVar *_parent) {
	this->parent     = _parent;
	this->name       = name;
	this->type       = HEX_CVAR;
	this->flag       = flag;
	this->ReadCVar   = ReadHexCVar;
	this->OutputCVar = OutputHexCVar;
	this->Increment  = IncrementOtherCVar;
	this->Decrement  = DecrementOtherCVar;
	this->hValue     = (DWORD*)value;

	manager.RegisterCVar(this);
}

CVar::CVar(char *name, int flag, FVector3 *min, FVector3 *max, FVector3 *value, CVar *_parent) {
	INITIALIZE_CVAR(vMin, vMax, vValue, ReadVectorCVar, OutputVectorCVar, IncrementOtherCVar, DecrementOtherCVar, VECTOR3_CVAR);
	parent    = _parent;
	precision = DEFAULT_PRECISION;
}

CVar::CVar(char *name, int flag, FVector4 *min, FVector4 *max, FVector4 *value, CVar *_parent) {
	INITIALIZE_CVAR(v4Min, v4Max, v4Value, ReadFVector4CVar, OutputFVector4CVar, IncrementOtherCVar, DecrementOtherCVar, VECTOR4_CVAR);
	parent    = _parent;
	precision = DEFAULT_PRECISION;
}

CVar::CVar(char *name, int flag, char *value, CVar *_parent) {
	this->parent     = _parent;
	this->name       = name;
	this->type       = STRING_CVAR;
	this->flag       = flag;
	this->szValue    = value;
	this->ReadCVar   = ReadTextCVar;
	this->OutputCVar = OutputTextCVar;
	this->Increment  = IncrementOtherCVar;
	this->Decrement  = DecrementOtherCVar;

	manager.RegisterCVar(this);
}

CVar::CVar(char *header, CVar *_parent) {
	this->name       = header;
	this->parent     = _parent;
	this->type       = HEADER_CVAR;
	this->flag       = 0;
	this->precision  = DEFAULT_PRECISION;
	this->ReadCVar   = NULL;
	this->OutputCVar = OutputHeaderCVar;
	this->Increment  = IncrementHeaderCVar;
	this->Decrement  = DecrementHeaderCVar;

	manager.RegisterCVar(this);
}

CVar *GetBaseParent(CVar *var) {
	CVar *res = var;
	while (res->parent != NULL) {
		res = res->parent;
	}
	return res;
}

void GetIndentForCVar(CVar *var, char *res, size_t len) {
	*res = 0;
	while (var->parent) {
		strcat_s(res, len, " ");
		var = var->parent;
	}
}

void ReadFloatCVar(CVar *var, char *text) {
	float newValue;

	if (1 == sscanf_s(text, "%f", &newValue)) {
		if (newValue < var->fMin) {
			newValue = var->fMin;
		} else if (newValue > var->fMax) {
			newValue = var->fMax;
		}

		(*var->fValue) = newValue;
	}
}

void ReadIntCVar(CVar *var, char *text) {
	int newValue;

	if (1 == sscanf_s(text, "%i", &newValue)) {
		if (newValue < var->iMin) {
			newValue = var->iMin;
		} else if (newValue > var->iMax) {
			newValue = var->iMax;
		}

		(*var->iValue) = newValue;
	}
}

void ReadHexCVar(CVar *var, char *text) {
	unsigned int newValue;

	if (1 == sscanf_s(text, "%x", &newValue)) {
		(*var->hValue) = newValue;
	}
}

void ReadVectorCVar(CVar *var, char *text) {
	FVector3 newValue;

	if (3 == sscanf_s(text, "%f %f %f", &newValue[0], &newValue[1], &newValue[2])) {
		if (newValue[0] < var->vMin->x) {
			newValue[0] = var->vMin->x;
		} else if (newValue[0] > var->vMax->x) {
			newValue[0] = var->vMax->x;
		}
		
		if (newValue[1] < var->vMin->y) {
			newValue[1] = var->vMin->y;
		} else if (newValue[1] > var->vMax->y) {
			newValue[1] = var->vMax->y;
		}
		
		if (newValue[2] < var->vMin->z) {
			newValue[2] = var->vMin->z;
		} else if (newValue[2] > var->vMax->z) {
			newValue[2] = var->vMax->z;
		}

		(*var->vValue) = newValue;
	}
}

void ReadFVector4CVar(CVar *var, char *text) {
	FVector4 newValue;

	if (4 == sscanf_s(text, "%f %f %f %f", &newValue[0], &newValue[1], &newValue[2], &newValue[3])) {
		for (size_t i = 0; i < 4; i++) {
			if (newValue[i] < var->v4Min->v[i]) {
				newValue[i] = var->v4Min->v[i];
			} else if (newValue[i] > var->v4Max->v[i]) {
				newValue[i] = var->v4Max->v[i];
			}
		}

		(*var->v4Value) = newValue;
	}
}

void ReadTextCVar(CVar *var, char *text) {
	if (NULL == var->szValue) {
		var->szValue = (char*)malloc(strlen(text) + 1);
		strcpy(var->szValue, text);
	} else {
		if (0 != strcmp(var->szValue, text)) {
			var->szValue = (char*)malloc(strlen(text) + 1);
			strcpy(var->szValue, text);
		}
	}
}

void OutputFloatCVar(CVar *var, char *text, size_t maxLen) {
	switch (var->precision) {
	case 2:
	default:
		sprintf_s(text, maxLen, "%.2f", *var->fValue);
		break;

	case 3:
		sprintf_s(text, maxLen, "%.3f", *var->fValue);
		break;

	case 4:
		sprintf_s(text, maxLen, "%.4f", *var->fValue);
		break;
	}
}

void OutputIntCVar(CVar *var, char *text, size_t maxLen) {
	sprintf_s(text, maxLen, "%i", *var->iValue);
}

void OutputHexCVar(CVar *var, char *text, size_t maxLen) {
	sprintf_s(text, maxLen, "%X", *var->hValue);
}

void OutputVectorCVar(CVar *var, char *text, size_t maxLen) {
	switch (var->precision) {
	case 2:
	default:
		sprintf_s(text, maxLen, "%.2f %.2f %.2f", var->vValue->x, var->vValue->y, var->vValue->z);
		break;

	case 3:
		sprintf_s(text, maxLen, "%.3f %.3f %.3f", var->vValue->x, var->vValue->y, var->vValue->z);
		break;

	case 4:
		sprintf_s(text, maxLen, "%.4f %.4f %.4f", var->vValue->x, var->vValue->y, var->vValue->z);
		break;
	}
}

void OutputFVector4CVar(CVar *var, char *text, size_t maxLen) {
	switch (var->precision) {
	case 2:
	default:
		sprintf_s(text, maxLen, "%.2f %.2f %.2f %.2f", var->v4Value->v[0], var->v4Value->v[1], var->v4Value->v[2], var->v4Value->v[3]);
		break;

	case 3:
		sprintf_s(text, maxLen, "%.3f %.3f %.3f", var->v4Value->v[0], var->v4Value->v[1], var->v4Value->v[2], var->v4Value->v[3]);
		break;

	case 4:
		sprintf_s(text, maxLen, "%.4f %.4f %.4f", var->v4Value->v[0], var->v4Value->v[1], var->v4Value->v[2], var->v4Value->v[3]);
		break;
	}
}

void OutputTextCVar(CVar *var, char *text, size_t maxLen) {
	strcpy_s(text, maxLen, var->szValue);
}

void OutputHeaderCVar(CVar *var, char *text, size_t maxLen) {
	strcpy_s(text, maxLen, var->name);
}

void IncrementFloatCVar(CVar *var) {
	(*var->fValue) += 1.0 / (pow(10.0, (double)var->precision));

	if ((*var->fValue) > var->fMax) {
		(*var->fValue) = var->fMin;
	}
}

void IncrementIntCVar(CVar *var) {
	(*var->iValue)++;

	if ((*var->iValue) > var->iMax) {
		(*var->iValue) = var->iMin;
	}
}

void IncrementOtherCVar(CVar *var) {
	return;
}

void IncrementHeaderCVar(CVar *var) {
	var->flag ^= FLAG_HEADER_OPEN;
}

void DecrementFloatCVar(CVar *var) {
	if ((*var->fValue) <= var->fMin) {
		(*var->fValue) = var->fMax;
	}

	(*var->fValue) -= 1.0 / (pow(10.0, (double)var->precision));
}

void DecrementIntCVar(CVar *var) {
	if ((*var->iValue) <= var->iMin) {
		(*var->iValue) = var->iMax;
	}

	(*var->iValue)--;
}

void DecrementOtherCVar(CVar *var) {
	return;
}

void DecrementHeaderCVar(CVar *var) {
	var->flag ^= FLAG_HEADER_OPEN;
}

DWORD WINAPI AutoSave(LPVOID) {
	manager.LoadConfig("sCoD4.cfg");

	while (1) {
		Sleep(5000);
		manager.SaveConfig("sCoD4.cfg");
	}

	return TRUE;
}

void InitializeConfig() {
	autoSaveThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoSave, 0, 0, 0);
}

void DestroyConfig() {
	TerminateThread(autoSaveThreadHandle, 0);
}

CVarManager::CVarManager() {
}

CVarManager::~CVarManager() {
	//manager.SaveConfig("sCoD4.cfg");
}

void CVarManager::RegisterCVar(CVar *var) {
	if (this->var.end() != std::find(this->var.begin(), this->var.end(), var)) {
		return;
	}

	this->var.push_back(var);
}

void CVarManager::UnregisterCVar(CVar *var) {
	auto it = this->var.find(var);

	if (NULL != it) {
		this->var.erase(it);
	}
}

bool CVarManager::IsVarVisible(CVar *var) {
	CVar *parent;

	parent = var->parent;

	while (1) {
		if (NULL == parent) {
			return true;
		}

		if (parent->flag & FLAG_HEADER_OPEN) {
			goto next;
		} else {
			return false;
		}
 next:;
		if (NULL != parent->parent) {
			parent = parent->parent;
		} else {
			return true;
		}
	}

	return false;
}

void CVarManager::GetHeaders(_Vector<CVar*> *result) {
	for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
		if (HEADER_CVAR == (*it)->type && IsVarVisible(*it) && NULL == (*it)->parent) {
			result->push_back(*it);
		}
	}
}

bool CompareStringAlpha(char *s1, char *s2) { // returns true if s1 goes before s2, otherwise s2 before s1
	while (NULL != (*s1) && NULL != (*s2)) {
		char c1[2], c2[2];

		c1[0] = *s1;
		c1[1] = 0;
		c2[0] = *s2;
		c2[1] = 0;

		_strupr_s(c1, sizeof(c1));
		_strupr_s(c2, sizeof(c2));

		s1++;
		s2++;
	}
}

void CVarManager::GetChildren(CVar *header, _Vector<CVar*> *result) {
	for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
		if (((*it)->parent) == header && header->flag & FLAG_HEADER_OPEN) {
			if (IsVarVisible(*it)) {
				/*
				   if ((*it)->type != HEADER_CVAR) {
						result->push_back(*it);
				   } else {
						if ((*it)->flag & FLAG_HEADER_OPEN) {
								GetChildren(*it, result);
						}

						result->push_back(*it);
				   }
				 */
				result->push_back(*it);
			}
		}
	}
}

void CVarManager::LoadConfig(char *configFile) {
	FILE  *file = NULL;
	size_t fileSize;
	char   cfgPath[MAX_PATH];

	//strcpy_s(cfgPath, sizeof(cfgPath), path);
	//strcat_s(cfgPath, sizeof(cfgPath), configFile);
	strcpy_s(cfgPath, sizeof(cfgPath), configFile);

	fopen_s(&file, cfgPath, "rb");

	if (NULL == file) {
		return;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	char *_input = (char*)malloc(fileSize + 2), name[256], value[256];

	memset(_input, 0, fileSize + 1);

	if (NULL == _input) {
		fclose(file);
		return;
	}

	fread(_input, 1, fileSize, file);

	_input[fileSize] = '\0';

	CVar *currentHeader = NULL;
	int minIndent = 0;
	_Vector<CVar*> headers;

	for (char *i = _input; 1; i) {
		if ('\0' == i[0]) {
			break;
		}

		while (('\r' == i[0]) || ('\n' == i[0])) {
			i++;
		}

		int indent = 0;

		while (' ' == *i) {
			i++;
			indent++;
		}

		char *start = i;

		while (('=' != i[0]) && (']' != i[0])) {
			i++;
		}

		char restore = i[0];
		bool isHeader;
		
		if (i[0] == ']') {
			start    = &start[1];
			isHeader = true;
		} else {
			isHeader = false;
		}
	
		i[0] = 0;
		strcpy_s(name, sizeof(name), start);
		i[0] = restore;

		i++;

		if (isHeader) {
			currentHeader = GetCVarByName(name);
		}

		if (indent > minIndent) {
			minIndent = indent;
			if (currentHeader != NULL && ((!headers.size()) || headers[headers.size() - 1] != currentHeader)) {
				headers.push_back(currentHeader);
			}
		} else {
			if (indent < minIndent) {
				headers.pop_back();
				if (indent == 0) {
					headers.clear();
				}
				if (headers.size()) {
					currentHeader = headers[headers.size() - 1];
				} else {
					currentHeader = NULL;
				}
			}
		}

		if (!isHeader) {
			start = i;

			for (size_t j = 0; j < 255; j++) {
				if ((i[0] == '\r' && i[1] == '\n') || i[0] == 0) {
					break;
				}

				i++;
			}

			char restore = i[0];
			i[0] = 0;
			strcpy_s(value, sizeof(value), start);
			i[0] = restore;

			CVar *var;
			if (currentHeader) {
				var = GetCVarByName(name, currentHeader);
			} else {
				var = GetCVarByName(name);
			}

			if (NULL != var && NULL != var->ReadCVar) {
				var->ReadCVar(var, value);
			}
		}

		if ('\r' == i[0]) {
			i += 2;
		} else if ('\n' == i[0]) {
			i++;
		} else if (0 == i[0]) {
			break;
		}
	}

	fclose(file);
}

void CVarManager::SaveConfig(char *configFile) {
	FILE *file;
	char  cfgPath[MAX_PATH];

	//strcpy_s(cfgPath, sizeof(cfgPath), path);
	//strcat_s(cfgPath, sizeof(cfgPath), configFile);
	strcpy_s(cfgPath, sizeof(cfgPath), configFile);

	fopen_s(&file, cfgPath, "wb");

	if (NULL == file) {
		return;
	}

	char *output = (char*)malloc(MAX_CONFIG_SIZE);

	if (NULL == output) {
		fclose(file);
		return;
	}

	DumpCVars(output, MAX_CONFIG_SIZE);
	fwrite(output, strlen(output), 1, file);
	fclose(file);
	free(output);
}

void CVarManager::DumpCVars(char *output, size_t maxLen) {
	output[0] = 0;
	/*
	   _Vector<CVar*> sortedVars, unsortedVars(this->var);

	   while (sortedVars.size() < this->var.size()) {
			CVar *first = NULL;
			for (auto it = (unsortedVars.begin()++); it != unsortedVars.end(); it++) {
			}
	   }*/

	_Vector<CVar*> baseHeaders;

	for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
		if ((*it)->type == HEADER_CVAR && (*it)->parent == NULL) {
			baseHeaders.push_back(*it);
		}
	}

	for (auto bh = baseHeaders.begin(); bh != baseHeaders.end(); bh++) {
		CVar *currentHeader = NULL, *lastHeader = NULL;
		char indentStr[32];
		indentStr[0] = 0;
	
		for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
			char line[256];
			if (*it != *bh && (*it)->parent != *bh) {
				continue;
			}
		
			if (HEADER_CVAR != (*it)->type) {
				if (currentHeader != (*it)->parent) {
					indentStr[strlen(indentStr) - 1] = 0;
				}
				sprintf_s(line, sizeof(line), "%s %s", indentStr, (*it)->name);
			} else {
				currentHeader = *it;
				if ((*it)->parent != NULL) {
					strcat_s(indentStr, sizeof(indentStr), " ");
				} else {
					indentStr[0] = 0;
				}
				sprintf_s(line, sizeof(line), "%s[%s]", indentStr, (*it)->name);
			}
		}
	
		indentStr[0]  = 0;
		currentHeader = lastHeader = NULL;

		for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
			char line[256];
			CVar *bp = GetBaseParent(*it);
			if ((*it)->parent == NULL) {
				if (*it != *bh) {
					continue;
				}
			} else {
				if (bp != *bh) {
					continue;
				}
			}

			GetIndentForCVar(*it, indentStr, sizeof(indentStr));
			if (HEADER_CVAR != (*it)->type) {
				sprintf_s(line, sizeof(line), "%s %s=", indentStr, (*it)->name);
				strcat_s(output, maxLen, line);

				if (NULL != (*it)->OutputCVar) {
					(*it)->OutputCVar(*it, line, sizeof(line));
				}

				strcat_s(output, maxLen, line);
				strcat_s(output, maxLen, "\r\n");
			} else {
				sprintf_s(line, sizeof(line), "%s[%s]", indentStr, (*it)->name);
				strcat_s(output, maxLen, line);
				strcat_s(output, maxLen, "\r\n");
			}
		}
	}
}

CVar *CVarManager::GetCVarByName(char *name) {
	for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
		if (0 == strcmp(name, (*it)->name)) {
			return *it;
		}
	}

	return 0;
}

CVar *CVarManager::GetCVarByName(char *name, CVar *parent) {
	for (_Vector<CVar*>::iterator it = this->var.begin(); it != this->var.end(); it++) {
		if ((*it)->parent == parent) {
			if (0 == strcmp(name, (*it)->name)) {
				return *it;
			}
		}
	}

	return 0;
}
