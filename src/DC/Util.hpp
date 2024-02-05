#pragma once
#include <Windows.h>

BOOL LoadDriver(
	_In_ LPCWSTR pServiceName,
	_In_ LPCWSTR pDriverFullPath
);

BOOL UnloadDriver(
	_In_ LPCWSTR pServiceName
);

PVOID AllocateZeroedMemory(
	_In_ SIZE_T dwSize
);

VOID FreeMemory(
	_In_ PVOID p
);