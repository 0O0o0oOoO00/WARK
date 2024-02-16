#pragma once
#include <ntifs.h>
#include "Define.h"

#define DEFAULT_BUFFER_PAGE_SIZE 1024

typedef struct _BUFFER_PAGE {
	LIST_ENTRY PageLink;
	ULONG ulPageSize;
	ULONG ulUsed;
	CHAR Date[];
}BUFFER_PAGE, * PBUFFER_PAGE;

typedef struct _KBUFFER {
	ULONG ulBufferSize;
	ULONG ulUsedSize;
	ULONG ulPageCount;
	ALLOC_TYPE AllocType;
	PBUFFER_PAGE pBufferPage;
}KBUFFER, * PKBUFFER;

PKBUFFER NewBuffer();
NTSTATUS InitBuffer(
	_Out_ PKBUFFER pKBuffer
);

NTSTATUS WriteToBufferWithOffset(
	_In_ PKBUFFER pKBuffer,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteLength
);
NTSTATUS WriteToBufferContinuously(
	_In_ PKBUFFER pKBuffer,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteLength
);

NTSTATUS ReadFromBufferWithOffset(
	_In_ PKBUFFER pKBuffer,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadLength
);
NTSTATUS ReadFromBufferContinuously(
	_In_ PKBUFFER pKBuffer,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadLength
);