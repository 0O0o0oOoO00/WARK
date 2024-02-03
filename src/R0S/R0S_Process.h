#pragma once
#include <ntifs.h>
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"

typedef ENUM_STATUS (*ENUM_EPROCESS_CALLBACK)(_In_ PNOK_INCOMPLETE_EPROCESS pEprocess, _In_opt_ PVOID Parameter);

NTSTATUS EnumEprocess(
	_In_ ENUM_EPROCESS_CALLBACK Callback,
	_In_ PVOID Parameter
);