#ifndef ANTIDUMP_H
#define ANTIDUMP_H

#define DECLARE_WIPABLE_FUNC(r, f) void Begin_ ## f; r f; void End_ ## f
#define BEGIN_WIPABLE_FUNC(r, f)  __declspec(naked) void Begin_ ## f {}
#define END_WIPABLE_FUNC(r, f)    __declspec(naked) void End_ ## f {}
#define WIPE_FUNC(f)              WipeMemory(Begin_ ## f, End_ ## f)
#define WIPE_STRING(s)            WipeMemory((void*)s, MAKEPTR(void*, s, strlen(s)))

DECLARE_WIPABLE_FUNC(void, PreparePageForWiping(void *ptr));
DECLARE_WIPABLE_FUNC(void, WipeMemory(void *start, void *end));
DECLARE_WIPABLE_FUNC(void, WipeMemoryEx(void *start, size_t len));

void WipeString(char *s);

#endif
