#pragma once
#include <ntifs.h>

#define MAXIMUM_ITEMS_PRE_VECTOR_ENTRY 512

typedef struct _VECTOR_ENTRY {
	LIST_ENTRY EntryLink;
	CHAR Data[];
}VECTOR_ENTRY, * PVECTOR_ENTRY;

typedef struct _KVECTOR {
	ULONG ulTypeSize;
	union {
		ULONG ulCount;
		ULONG ulNextIndex;
	};
	PVECTOR_ENTRY pVectorEntry;
}KVECTOR, * PKVECTOR;

PKVECTOR NewVector(
	_In_ ULONG ulTypeSize
);
PVOID VectorIndexOf(
	_In_ PKVECTOR pVector,
	_In_ ULONG ulIndex
);
NTSTATUS VectorPush(
	_In_ PKVECTOR pVector,
	_In_ PVOID pItem
);
VOID VectorPop(
	_In_ PKVECTOR pVector
);
ULONG VectorItemsCount(
	_In_ PKVECTOR pVector
);
ULONG VectorItemsBytesLength(
	_In_ PKVECTOR pVector
);
VOID FreeVector(
	_In_ PKVECTOR pVector
);
PVOID SendVectorContentToR3(
	_In_ PEPROCESS pEprocess,
	_In_ PKVECTOR pVector
);