#pragma once
#include <ntifs.h>
#include "Define.h"

#define OBJECT_DIRECTORY_INFORMATION_BUFFER_LENGTH 256

typedef struct _OBJECT_DIRECTORY_INFORMATION {
	UNICODE_STRING Name;
	UNICODE_STRING TypeName;
}OBJECT_DIRECTORY_INFORMATION, * POBJECT_DIRECTORY_INFORMATION;

typedef ENUM_STATUS (*ENUM_DIRECTORY_OBJECT_CALLBACK)(_In_ POBJECT_DIRECTORY_INFORMATION pDirectoryObjectInformation, _In_ PVOID Parameter);

NTSTATUS EnumDirectoryObject(
	_In_ PUNICODE_STRING pDirectory,
	_In_ ENUM_DIRECTORY_OBJECT_CALLBACK Callback,
	_In_ PVOID Parameter
);