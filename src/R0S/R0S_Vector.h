#pragma once
#include <ntifs.h>

#define MAXIMUM_ITEMS_PRE_VECTOR_ENTRY 512

typedef struct _VECTOR_ENTRY {
	LIST_ENTRY EntryLink;
	CHAR Data[];
}VECTOR_ENTRY, * PVECTOR_ENTRY;

typedef struct _VECTOR {
	ULONG ulTypeSize;
	union {
		ULONG ulCount;
		ULONG ulNextIndex;
	};
	PVECTOR_ENTRY pVectorEntry;
}VECTOR, * PVECTOR;

PVECTOR NewVector(
	_In_ ULONG ulTypeSize
);
PVOID VectorIndexOf(
	_In_ PVECTOR pVector,
	_In_ ULONG ulIndex
);
NTSTATUS VectorPush(
	_In_ PVECTOR pVector,
	_In_ PVOID pItem
);
VOID VectorPop(
	_In_ PVECTOR pVector
);
ULONG VectorItemsCount(
	_In_ PVECTOR pVector
);
ULONG VectorItemsBytesLength(
	_In_ PVECTOR pVector
);
VOID FreeVector(
	_In_ PVECTOR pVector
);
PVOID SendVectorContentToR3(
	_In_ PEPROCESS pEprocess,
	_In_ PVECTOR pVector
);