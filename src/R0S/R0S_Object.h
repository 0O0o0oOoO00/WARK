#pragma once
#include <ntifs.h>
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"

typedef ENUM_STATUS (*ENUM_OBJECT_CALLBACK)(_In_ PNOK_INCOMPLETE_OBJECT_TYPE pObjectType, _In_opt_ PVOID Parameter);

NTSTATUS EnumObjectType(
	_In_ ENUM_OBJECT_CALLBACK Callback,
	_In_ PVOID Parameter
);