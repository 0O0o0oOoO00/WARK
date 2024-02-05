#pragma once
#include <ntifs.h>
#include "Ioctl.h"
#include "NtoskrnlStruct.h"
#include "Define.h"

typedef ENUM_STATUS (*ENUM_OBJECT_CALLBACK)(_In_ PNOK_INCOMPLETE_OBJECT_TYPE pObjectType, _In_opt_ PVOID Parameter);

NTSTATUS EnumObjectType(
	_In_ ENUM_OBJECT_CALLBACK Callback,
	_In_ PVOID Parameter
);

IOCTL_FUNC(CollectObjectTypeInfo);
IOCTL_FUNC(CollectObjectTypeDetail);