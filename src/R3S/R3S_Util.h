#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

PVOID AllocateZeroedMemory(
	_In_ SIZE_T dwSize
);

VOID FreeMemory(
	_In_ PVOID p
);

#ifdef __cplusplus
}
#endif // __cplusplus