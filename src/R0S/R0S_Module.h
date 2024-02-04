#pragma once
#include <ntifs.h>
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"

typedef ENUM_STATUS (*ENUM_MODULE)(_In_ PKLDR_DATA_TABLE_ENTRY pKldrDataTableEntry, _In_opt_ PVOID Parameter);

NTSTATUS EnumModule(
	_In_ ENUM_MODULE fnCallback,
	_In_opt_ PVOID Parameter
);

PVOID GetNtoskrnlBase();