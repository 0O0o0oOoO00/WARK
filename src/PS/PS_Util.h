#pragma once

#define FORCE_CONVERT(t, p) ((t)((PVOID)(p)))

#define CALC_ADDRESS(base, offset) ((PVOID)((ULONGLONG)(base) + (ULONGLONG)(offset)))
#define CALC_OFFSET(address1, address2) ((ULONG)((ULONGLONG)(address2) - (ULONGLONG)(address1)))

#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)