#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

BOOL LoadDriver(
	_In_ LPCWSTR pServiceName,
	_In_ LPCWSTR pDriverFullPath
);

BOOL UnloadDriver(
	_In_ LPCWSTR pServiceName
);

#ifdef __cplusplus
}
#endif // __cplusplus