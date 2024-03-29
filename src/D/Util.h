#pragma once
#include <ntifs.h>

#define FORCE_CONVERT(t, p) ((t)((PVOID)(p)))

#define CALC_ADDRESS(base, offset) ((PVOID)((ULONGLONG)(base) + (ULONGLONG)(offset)))
#define CALC_OFFSET(address1, address2) ((ULONG)((ULONGLONG)(address2) - (ULONGLONG)(address1)))

#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

#define ARGS_COUNT(...) (sizeof((void*[]){__VA_ARGS__})/sizeof(void*))

NTSTATUS RtlAppendMultipleUnicodeToString(
	_In_ PUNICODE_STRING pBaseString,
	_In_ ULONG ulCount,
	...
);
#define AppendMultipleUnicodeToString(pBaseString, ...) \
	RtlAppendMultipleUnicodeToString(pBaseString, ARGS_COUNT(__VA_ARGS__), __VA_ARGS__)

NTSTATUS RtlAppendMultipleUnicodeStringToString(
	_In_ PUNICODE_STRING pBaseString,
	_In_ ULONG ulCount,
	...
);
#define AppendMultipleUnicodeStringToString(pBaseString, ...) \
	RtlAppendMultipleUnicodeStringToString(pBaseString, ARGS_COUNT(__VA_ARGS__), __VA_ARGS__)

PVOID MmAllocateZeroedMemory(
	_In_ POOL_TYPE PoolType,
	_In_ SIZE_T NumberOfBytes
);
#define MmAllocateZeroedNonPagedMemory(n) \
	MmAllocateZeroedMemory(NonPagedPool, n)
#define MmAllocateZeroedPagedMemory(n) \
	MmAllocateZeroedMemory(PagedPool, n)

SIZE_T RtlMultipleUnicodeStringLengthBytes(
	_In_ ULONG ulCount,
	...
);
#define MultipleUnicodeStringLengthBytes(...) \
	RtlMultipleUnicodeStringLengthBytes(ARGS_COUNT(__VA_ARGS__), __VA_ARGS__)

NTSTATUS RtlEmptyUnicodeString(
	_Out_ PUNICODE_STRING pString,
	_In_ SIZE_T NumberOfBytes
);

VOID RtlFreeUnicodeStringBuffer(
	_Inout_ PUNICODE_STRING pString
);

PVOID MmAllocateR3Memory(
	_In_ PEPROCESS pEprocess,
	_In_ SIZE_T Size
);

PVOID SendDataToR3(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulLength
);

#define SendStringToR3(pEprocess, pString) \
	SendDataToR3((pEprocess), ((pString)->Buffer), ((pString)->MaximumLength))

NTSTATUS SwitchCr3(
	_In_ ULONGLONG ullNewCr3,
	_Out_opt_ PULONGLONG pOldCr3
);