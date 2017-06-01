#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <stdio.h>
#pragma hdrstop
#include "log.h"
#include "debug.h"
#include "vftmanager.h"

#define REGISTER_DUMP
#define STACK_DUMP

char          out[512];
HMODULE       hMods[1024];
MODULEENTRY32 meMods[1024];
int           numModules;
HANDLE        hProcess;
DWORD         cbNeeded;

DWORD EnumProcessModules(MODULEENTRY32 *result, size_t maxModules) {
	HANDLE        snap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	
	numModules = 0;
	snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

	if (INVALID_HANDLE_VALUE == snap) {
		return FALSE;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(snap, &me32)) {
		CloseHandle(snap);
		return FALSE;
	}

	size_t i = 0;

	do {
		//if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
			result[i++] = me32;
			if (i >= maxModules) {
				break;
			}
		//}
	} while (Module32Next(snap, &me32));

	numModules = i;
	CloseHandle(snap);
	return i;
}

BOOL UpdateModuleCache() {
	//return(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded));
	return EnumProcessModules(meMods, 1024);
}

void DumpModules() {
	Log("%i modules\n", numModules);

	for (size_t i = 0; i < numModules; i++) {
		Log("module %s base %08X size %08X\n", meMods[i].szModule, meMods[i].modBaseAddr, meMods[i].dwSize);
	}
}

bool CheckValidReadPtr(DWORD Protect) {
	if ((Protect & PAGE_GUARD) == PAGE_GUARD) {
		return false;
	}

	if ((Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ || (Protect & PAGE_READONLY) == PAGE_READONLY || (Protect & PAGE_READWRITE) == PAGE_READWRITE || (Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ) {
		return true;
	}

	return false;
}

BOOL IsAddressReadable(ADDRESS_PROC ea) {
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery((LPVOID)ea,&mbi,64);
  return CheckValidReadPtr(mbi.Protect);
}

BOOL IsPointerToASCII(ADDRESS_PROC ea) {
  /*if(TRUE == IsAddressReadable(ea)) {
	ADDRESS_PROC _ea = *((ADDRESS_PROC*)ea);
	if(TRUE == IsAddressReadable(_ea)) {
	  char *pea = (char *)ea;
#define IS_ASCII(x) (((x) >= 'a' && ((x) <= 'z')) || ((x)>='A' && (x)<='Z') || ((x)>='0' && (x)<='9')))
	  unsigned int numAscii = 0;
	  for(size_t i = 0; i < 4; ++i) {
		if(IS_ASCII(pea[i]))
		  ++numAscii;
	  }
	  if(numAscii >= 3) {
		if(strlen_s(pea,64) < 62) {
		  return TRUE;
		}
	  }
	}
  }*/
   return(FALSE);
}

void GetExtraInfo(char* output, ADDRESS_PROC ea) {
  ADDRESS_PROC *pea = &ea;
  if(IsPointerToASCII((ADDRESS_PROC)pea)) {
	// ea points to an ASCII string.
	sprintf(output, " \"%s\"", (char*)ea);
  } else if(IsPointerToASCII(ea)) {
	// ea points to a pointer of an ASCII string.
	sprintf(output, " PTR to \"%s\"", *((char**)ea));
  } else {
	output[0] = '\0';
  }
}

char *GetAddressStringAsRVA(ADDRESS_PROC ea) {
   unsigned int i;
   char         extraInfo[128];
   
   GetExtraInfo(extraInfo, ea);

   if (ea < 0x00100000) {
	  if (ea == 0) {
		 strcpy_s(out, "0");
		 return(out);
	  }
	  sprintf_s(out, "%08X%s", ea, extraInfo);
	  return(out);
   }

   for (i = 0; i < numModules; i++) {
	   MODULEENTRY32 *mod = &meMods[i];
	  if ((ea >= (ADDRESS_PROC)mod->modBaseAddr) && (ea <= (ADDRESS_PROC)mod->modBaseAddr + (ADDRESS_PROC)mod->modBaseSize)) {
		   sprintf_s(out, "%s.%08X%s", mod->szModule, ea - (ADDRESS_PROC)mod->modBaseAddr, extraInfo);
		   return(out);
	  }
   }

   sprintf_s(out, "%08X%s", ea, extraInfo);
   return(out);
}

char *GetCommonExceptionStr(DWORD ExceptionCode) {
   switch (ExceptionCode) {
   case EXCEPTION_ACCESS_VIOLATION:
	  return("EXCEPTION_ACCESS_VIOLATION");

   case EXCEPTION_DATATYPE_MISALIGNMENT:
	  return("EXCEPTION_DATATYPE_MISALIGNMENT");

   case EXCEPTION_BREAKPOINT:
	  return("EXCEPTION_BREAKPOINT");

   case EXCEPTION_SINGLE_STEP:
	  return("EXCEPTION_SINGLE_STEP");

   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	  return("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");

   case EXCEPTION_FLT_DENORMAL_OPERAND:
	  return("EXCEPTION_FLT_DENORMAL_OPERAND");

   case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	  return("EXCEPTION_FLT_DIVIDE_BY_ZERO");

   case EXCEPTION_FLT_INEXACT_RESULT:
	  return("EXCEPTION_FLT_INEXACT_RESULT");

   case EXCEPTION_FLT_INVALID_OPERATION:
	  return("EXCEPTION_FLT_INVALID_OPERATION");

   case EXCEPTION_FLT_OVERFLOW:
	  return("EXCEPTION_FLT_OVERFLOW");

   case EXCEPTION_FLT_STACK_CHECK:
	  return("EXCEPTION_FLT_STACK_CHECK");

   case EXCEPTION_FLT_UNDERFLOW:
	  return("EXCEPTION_FLT_UNDERFLOW");

   case EXCEPTION_INT_DIVIDE_BY_ZERO:
	  return("EXCEPTION_INT_DIVIDE_BY_ZERO");

   case EXCEPTION_INT_OVERFLOW:
	  return("EXCEPTION_INT_OVERFLOW");

   case EXCEPTION_PRIV_INSTRUCTION:
	  return("EXCEPTION_PRIV_INSTRUCTION");

   case EXCEPTION_IN_PAGE_ERROR:
	  return("EXCEPTION_IN_PAGE_ERROR");

   case EXCEPTION_ILLEGAL_INSTRUCTION:
	  return("EXCEPTION_ILLEGAL_INSTRUCTION");

   case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	  return("EXCEPTION_NONCONTINUABLE_EXCEPTION");

   case EXCEPTION_STACK_OVERFLOW:
	  return("EXCEPTION_STACK_OVERFLOW");

   case EXCEPTION_INVALID_DISPOSITION:
	  return("EXCEPTION_INVALID_DISPOSITION");

   case EXCEPTION_GUARD_PAGE:
	  return("EXCEPTION_GUARD_PAGE");

   case EXCEPTION_INVALID_HANDLE:
	  return("EXCEPTION_INVALID_HANDLE");

   //case EXCEPTION_POSSIBLE_DEADLOCK:
	   //return "EXCEPTION_POSSIBLE_DEADLOCK";
   default:
	   return "EXCEPTION_UNKNOWN";
   }
}

void DumpStack(void *sp) {
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)sp, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	// TODO: make a loop to add more pages as long as they have PAGE_READWRITE
	// TODO: log deepest RA/address in containing module's address space(if there is one)
	if (mbi.Protect == PAGE_READWRITE) {
		ADDRESS_PROC ea2 = (ADDRESS_PROC)mbi.BaseAddress + mbi.RegionSize;
		for (ADDRESS_PROC ea = (ADDRESS_PROC)sp; ea < ea2; ea += 4) {
			Log("%08X: %s\n", ea, GetAddressStringAsRVA(*(ADDRESS_PROC *)ea));
		}
	}
}

LONG WINAPI CrashHandler(EXCEPTION_POINTERS *pExceptionInfo) {
   char *szException = GetCommonExceptionStr(pExceptionInfo->ExceptionRecord->ExceptionCode);

   if (szException) {
	  UpdateModuleCache();
	  if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		 if (pExceptionInfo->ExceptionRecord->NumberParameters != 0) {
			if (pExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 0) {
			   Log("[%s] %08X at %s reading %08X\n", szException, pExceptionInfo->ExceptionRecord->ExceptionCode, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip), pExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
			}
			else if (pExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 1)                    // writing
			{
			   Log("[%s] %08X at %s writing %08X\n", szException, pExceptionInfo->ExceptionRecord->ExceptionCode, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip), pExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
			}
			goto logged;
		 }
	  }
	  Log("[%s] at %s\n", szException, GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip));
logged:
#ifdef REGISTER_DUMP
	  Log("EAX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eax));
	  Log("ECX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ecx));
	  Log("EDX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Edx));
	  Log("EBX %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ebx));
	  Log("ESP %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Esp));
	  Log("EBP %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Ebp));
	  Log("ESI %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Esi));
	  Log("EDI %s\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Edi));
	  Log("EIP %s\n\n", GetAddressStringAsRVA(pExceptionInfo->ContextRecord->Eip));
	  Log("EFL %08X\n\n", pExceptionInfo->ContextRecord->EFlags);
#endif // REGISTER_DUMP

#ifdef STACK_DUMP
	  // traverse stack upwards to end of stack page
	  DumpStack((void*)pExceptionInfo->ContextRecord->Esp);
#endif // STACK_DUMP
	  Sleep(500);           // sleep to make sure the log gets saved.
   }
   return(EXCEPTION_CONTINUE_SEARCH);
}

PVOID debugHandler;

void InitializeDebug() {
	debugHandler = AddVectoredExceptionHandler(1, CrashHandler);
}

void ReleaseDebug() {
	RemoveVectoredExceptionHandler(debugHandler);
	debugHandler = 0;
}

void LogVTable(char *name, void **ptr) {
	void **vmt = (*(void***)ptr);
	size_t numFuncs = CountVFTFunctions(vmt);
	DEBUG_LOG("vtable %s at inst %08x %i funcs rva %s\n", name, ptr, numFuncs, GetAddressStringAsRVA((ADDRESS_PROC)vmt));
	for (size_t i = 0; i < numFuncs; i++) {
		DEBUG_LOG("[%04i] %08X %s\n", i, vmt[i], GetAddressStringAsRVA((ADDRESS_PROC)(vmt)[i]));
	}
	DEBUG_LOG("\n");
}
