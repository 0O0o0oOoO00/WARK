#pragma once
#include <ntifs.h>

typedef ULONG ENUM_STATUS;
#define ENUM_ERROR 2
#define ENUM_BREAK 1
#define ENUM_CONTINUE 0
#define IS_CONTINUE_ENUM(status) ((ULONG)(status) == ENUM_CONTINUE)

#define NTOSKRNL_NAME L"ntoskrnl.exe"

typedef enum _ALLOC_TYPE {
	OnStack = 0,
	OnHeap
}ALLOC_TYPE, * PALLOC_TYPE;
