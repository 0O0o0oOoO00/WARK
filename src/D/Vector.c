#include "Vector.h"
#include "Util.h"

BOOLEAN NextVectorEntry(
	_In_ PVECTOR pVector,
	_Inout_ PVECTOR_ENTRY* ppEntry
) {
	if (!pVector || !ppEntry) {
		return FALSE;
	}

	PVECTOR_ENTRY pFirstEntry = pVector->pVectorEntry;
	PVECTOR_ENTRY pNextEntry = CONTAINING_RECORD((*ppEntry)->EntryLink.Blink, VECTOR_ENTRY, EntryLink);

	if (pNextEntry != pFirstEntry) {
		(*ppEntry) = pNextEntry;
		return TRUE;
	}
	return FALSE;
}

NTSTATUS VectorAddEntry(
	_In_ PVECTOR pVector
) {
	if (!pVector) {
		return STATUS_UNSUCCESSFUL;
	}

	ULONG ulEntrySize = sizeof(VECTOR_ENTRY) + MAXIMUM_ITEMS_PRE_VECTOR_ENTRY * pVector->ulTypeSize;
	PVECTOR_ENTRY pEntry = ExAllocatePool(ulEntrySize, NonPagedPool);
	if (!pEntry) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory(pEntry, ulEntrySize);
	InsertTailList(&(pVector->pVectorEntry->EntryLink), &(pEntry->EntryLink));

	return STATUS_SUCCESS;
}

PVECTOR NewVector(
	_In_ ULONG ulTypeSize
) {
	if (!ulTypeSize) {
		return NULL;
	}

	PVECTOR pVector = MmAllocateZeroedNonPagedMemory(sizeof(VECTOR));
	if (!pVector) {
		return NULL;
	}

	pVector->ulTypeSize = ulTypeSize;

	ULONG ulEntrySize = sizeof(VECTOR_ENTRY) + MAXIMUM_ITEMS_PRE_VECTOR_ENTRY * ulTypeSize;
	PVECTOR_ENTRY pFirstEntry = MmAllocateZeroedNonPagedMemory(ulEntrySize);
	if (!pFirstEntry) {
		ExFreePool(pVector);
		return NULL;
	}

	InitializeListHead(&(pFirstEntry->EntryLink));
	pVector->pVectorEntry = pFirstEntry;

	return pVector;
}

PVOID VectorIndexOf(
	_In_ PVECTOR pVector,
	_In_ ULONG ulIndex
) {
	// TODO: vector index check
	if (!pVector) {
		return NULL;
	}

	// TODO: switch entry page
	PVECTOR_ENTRY pEntry = CONTAINING_RECORD(pVector->pVectorEntry->EntryLink.Flink, VECTOR_ENTRY, EntryLink);
	return (PVOID)(pEntry->Data + (ulIndex % MAXIMUM_ITEMS_PRE_VECTOR_ENTRY) * pVector->ulTypeSize);
}

NTSTATUS VectorPush(
	_In_ PVECTOR pVector,
	_In_ PVOID pItem
) {
	if (!pVector || !pItem) {
		return STATUS_UNSUCCESSFUL;
	}

	if (pVector->ulCount != 0 && pVector->ulCount % MAXIMUM_ITEMS_PRE_VECTOR_ENTRY == 0) {
		if (!NT_SUCCESS(VectorAddEntry(pVector))) {
			return STATUS_UNSUCCESSFUL;
		}
	}
	RtlCopyMemory(VectorIndexOf(pVector, pVector->ulNextIndex), pItem, pVector->ulTypeSize);
	pVector->ulCount++;

	return STATUS_SUCCESS;
}

VOID VectorPop(
	_In_ PVECTOR pVector
) {
	if (!pVector) {
		return STATUS_UNSUCCESSFUL;
	}
	PVOID pItem = VectorIndexOf(pVector, pVector->ulNextIndex - 1);
	if (!pItem) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlZeroMemory(pItem, pVector->ulTypeSize);
	pVector->ulCount--;
	if (pVector->ulCount % MAXIMUM_ITEMS_PRE_VECTOR_ENTRY == 0) {
		RemoveTailList(pVector->pVectorEntry->EntryLink.Flink);
	}
}

ULONG VectorItemsCount(
	_In_ PVECTOR pVector
) {
	if (!pVector) {
		return 0;
	}

	return pVector->ulCount;
}

ULONG VectorItemsBytesLength(
	_In_ PVECTOR pVector
) {
	if (!pVector) {
		return 0;
	}

	return pVector->ulCount * pVector->ulTypeSize;
}

VOID FreeVector(
	_In_ PVECTOR pVector
) {
	if (!pVector) {
		return;
	}

	PLIST_ENTRY pFirstEntry = &(pVector->pVectorEntry->EntryLink);
	PLIST_ENTRY pNextEntry = pFirstEntry;
	PLIST_ENTRY pPreviousEntry = pFirstEntry;

	while (TRUE) {
		pNextEntry = pNextEntry->Blink;
		if (pNextEntry == pFirstEntry) {
			break;
		}
		ExFreePool(CONTAINING_RECORD(pPreviousEntry, VECTOR_ENTRY, EntryLink));
		pPreviousEntry = pNextEntry;
	}

	ExFreePool(pVector);
}

PVOID SendVectorContentToR3(
	_In_ PEPROCESS pEprocess,
	_In_ PVECTOR pVector
) {
	if (!pEprocess || !pVector) {
		return NULL;
	}

	ULONG ulVectorBytesLength = VectorItemsBytesLength(pVector);
	if (!ulVectorBytesLength) {
		return NULL;
	}

	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(ulVectorBytesLength);
	if (!pBuffer) {
		return NULL;
	}

	for (ULONG i = 0; i < VectorItemsCount(pVector); i++) {
		PVOID pItem = VectorIndexOf(pVector, i);
		if (!pItem) {
			ExFreePool(pBuffer);
			return NULL;
		}
		RtlCopyMemory(CALC_ADDRESS(pBuffer, pVector->ulTypeSize * i), pItem, pVector->ulTypeSize);
	}
	PVOID pR3 = SendDataToR3(pEprocess, pBuffer, ulVectorBytesLength);
	if (!pR3) {
		ExFreePool(pBuffer);
		return NULL;
	}
	ExFreePool(pBuffer);
	return pR3;
}