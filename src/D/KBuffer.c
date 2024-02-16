#include "KBuffer.h"
#include "Util.h"

PKBUFFER NewBuffer() {
	PKBUFFER pKBuffer = MmAllocateZeroedNonPagedMemory(sizeof(KBUFFER));
	pKBuffer->AllocType = OnHeap;
	InitBuffer(pKBuffer);
	return pKBuffer;
}

NTSTATUS AddBufferPage(
	_Out_ PKBUFFER pKBuffer
) {
	if (!pKBuffer) {
		return STATUS_UNSUCCESSFUL;
	}

	PBUFFER_PAGE pPage = MmAllocateZeroedNonPagedMemory(sizeof(BUFFER_PAGE) + DEFAULT_BUFFER_PAGE_SIZE);
	pPage->ulPageSize = DEFAULT_BUFFER_PAGE_SIZE;

	if (pKBuffer->pBufferPage) {
		InsertTailList(&(pKBuffer->pBufferPage->PageLink), &(pPage->PageLink));
	} else {
		InitializeListHead(&(pPage->PageLink));
		pKBuffer->pBufferPage = pPage;
	}

	pKBuffer->ulBufferSize += DEFAULT_BUFFER_PAGE_SIZE;
	pKBuffer->ulPageCount++;

	return STATUS_SUCCESS;
}

NTSTATUS InitBuffer(
	_Out_ PKBUFFER pKBuffer
) {
	if (!pKBuffer) {
		return STATUS_UNSUCCESSFUL;
	}
	return AddBufferPage(pKBuffer);
}

VOID FreeBuffer(
	_In_ PKBUFFER pKBuffer
) {
	if (!pKBuffer) {
		return;
	}
	if (pKBuffer->ulPageCount) {
		ULONG ulCount = pKBuffer->ulPageCount;
		PLIST_ENTRY pFirst = &(pKBuffer->pBufferPage->PageLink);
		PLIST_ENTRY pList = pFirst;
		PBUFFER_PAGE* pBufferPageList = MmAllocateZeroedNonPagedMemory(sizeof(PBUFFER_PAGE) * ulCount);
		ULONG ulIndex = 0;

		do {
			pBufferPageList[ulIndex++] = CONTAINING_RECORD(pList, BUFFER_PAGE, PageLink);
			pList = pList->Flink;
		} while (pList != pFirst);

		for (ULONG i = 0; i < ulCount; i++) {
			ExFreePool(pBufferPageList[i]);
		}
		ExFreePool(pBufferPageList);
	}
	pKBuffer->ulBufferSize = 0;
	pKBuffer->ulUsedSize = 0;
	pKBuffer->ulPageCount = 0;

	if (pKBuffer->AllocType == OnHeap) {
		ExFreePool(pKBuffer);
	}
}

NTSTATUS WriteToBufferWithOffset(
	_In_ PKBUFFER pKBuffer,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteLength
) {
	// TODO: to be finished	
}

NTSTATUS WriteToBufferContinuously(
	_In_ PKBUFFER pKBuffer,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteLength
) {
	// TODO: to be finished	
}

NTSTATUS ReadFromBufferWithOffset(
	_In_ PKBUFFER pKBuffer,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadLength
) {
	// TODO: to be finished	
}


NTSTATUS ReadFromBufferContinuously(
	_In_ PKBUFFER pKBuffer,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadLength
) {
	// TODO: to be finished	
}
