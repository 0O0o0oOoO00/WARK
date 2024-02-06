#pragma once
#include <ntifs.h>
#include "NtoskrnlStruct.h"
#include "Define.h"

typedef ENUM_STATUS (*ENUM_MODULE)(_In_ PKLDR_DATA_TABLE_ENTRY pKldrDataTableEntry, _In_opt_ PVOID Parameter);

NTSTATUS EnumModule(
	_In_ ENUM_MODULE fnCallback,
	_In_opt_ PVOID Parameter
);

PVOID GetNtoskrnlBase();

IOCTL_FUNC(CollectModuleInfo);
