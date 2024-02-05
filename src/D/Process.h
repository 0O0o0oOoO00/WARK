#pragma once
#pragma once
#include <ntifs.h>
#include "NtoskrnlStruct.h"
#include "Define.h"
#include "Ioctl.h"

typedef ENUM_STATUS (*ENUM_EPROCESS_CALLBACK)(_In_ PNOK_INCOMPLETE_EPROCESS pEprocess, _In_opt_ PVOID Parameter);

NTSTATUS EnumEprocess(
	_In_ ENUM_EPROCESS_CALLBACK Callback,
	_In_ PVOID Parameter
);

ULONGLONG GetProcessCr3(
	_In_ PEPROCESS pEprocess
);

IOCTL_FUNC(SetTargetProcess);
IOCTL_FUNC(CollectProcessInfo);
IOCTL_FUNC(CollectProcessDetail);

IOCTL_FUNC(TerminateProcess);