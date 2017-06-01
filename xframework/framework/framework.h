#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "_vector.h"
#include "vmath.h"
#include "config.h"
#include "log.h"
#include "debug.h"
#include "detour.h"
#include "antipb.h"
#include "hwbp.h"
#include "input.h"
#include "mt19937.h"
#include "drender.h"
#include "input.h"
#include "vftmanager.h"
#include "xgui.h"
#include "xguirender.h"
#include "frender.h"
#include "controlfp.h"
#include "tzar.h"
#include "fcvars.h"
#include "antidump.h"
#include "pattern.h"
#include "memory.h"

void InitializeFramework();
void DestroyFramework();

#endif
