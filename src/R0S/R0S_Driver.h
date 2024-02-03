#pragma once
#include <ntifs.h>
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"

typedef ENUM_STATUS (*ENUM_DRIVER_LDR_CALLBACK)(_In_ PNOK_INCOMPLETE_LDR_DATA_TABLE_ENTRY pLdr, _In_opt_ PVOID Parameter);

NTSTATUS EnumDriverLdr(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ ENUM_DRIVER_LDR_CALLBACK Callback,
	_In_ PVOID Parameter
);

PVOID GetNtoskrnlBase(
	_In_ PDRIVER_OBJECT pDriverObject
);