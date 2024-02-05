#include "Memory.h"
#include "InputStruct.h"
#include "Process.h"
#include "Util.h"

PEPROCESS g_pTargetProcess;

NTSTATUS MmMdlReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulReadSize) {
		return STATUS_UNSUCCESSFUL;
	}

	MEMORY_MAP MemoryMap = { 0 };
	PVOID pVirtual = NULL;

	if (!NT_SUCCESS(MmMdlMapMemory(&MemoryMap, &pVirtual, pEprocess, VirtualAddress, ulReadSize))) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pBuffer, pVirtual, ulReadSize);

	MmUnmapMemory(&MemoryMap);
	return STATUS_SUCCESS;
}

NTSTATUS MmMdlWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulWriteSize) {
		return STATUS_UNSUCCESSFUL;
	}

	MEMORY_MAP MemoryMap = { 0 };
	PVOID pVirtual = NULL;

	if (!NT_SUCCESS(MmMdlMapMemory(&MemoryMap, &pVirtual, pEprocess, VirtualAddress, ulWriteSize))) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pVirtual, pBuffer, ulWriteSize);

	MmUnmapMemory(&MemoryMap);
	return STATUS_SUCCESS;
}

NTSTATUS MmPhysicalReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulReadSize) {
		return STATUS_UNSUCCESSFUL;
	}

	MEMORY_MAP MemoryMap = { 0 };
	PVOID pVirtual = NULL;

	if (!NT_SUCCESS(MmPhysicalMapMemory(&MemoryMap, &pVirtual, pEprocess, VirtualAddress, ulReadSize))) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pBuffer, pVirtual, ulReadSize);

	MmUnmapMemory(&MemoryMap);
	return STATUS_SUCCESS;
}

NTSTATUS MmPhysicalWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulWriteSize) {
		return STATUS_UNSUCCESSFUL;
	}

	MEMORY_MAP MemoryMap = { 0 };
	PVOID pVirtual = NULL;

	if (!NT_SUCCESS(MmPhysicalMapMemory(&MemoryMap, &pVirtual, pEprocess, VirtualAddress, ulWriteSize))) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pVirtual, pBuffer, ulWriteSize);

	MmUnmapMemory(&MemoryMap);
	return STATUS_SUCCESS;
}

NTSTATUS MmCr3ReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulReadSize) {
		return STATUS_UNSUCCESSFUL;
	}

	ULONGLONG ullProcessCr3 = GetProcessCr3(pEprocess);
	if (!ullProcessCr3) {
		return STATUS_UNSUCCESSFUL;
	}
	ULONGLONG ullOldCr3 = 0;
	if (!NT_SUCCESS(SwitchCr3(ullProcessCr3, &ullOldCr3))) {
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pBuffer, VirtualAddress, ulReadSize);

	if (!NT_SUCCESS(SwitchCr3(ullOldCr3, NULL))) {
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

NTSTATUS MmCr3WriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !VirtualAddress || !pBuffer || !ulWriteSize) {
		return STATUS_UNSUCCESSFUL;
	}

	ULONGLONG ullProcessCr3 = GetProcessCr3(pEprocess);
	if (!ullProcessCr3) {
		// TODO: cr3 free allocated r3 memory
		return STATUS_UNSUCCESSFUL;
	}
	ULONGLONG ullOldCr3 = 0;
	if (!NT_SUCCESS(SwitchCr3(ullProcessCr3, &ullOldCr3))) {
		// TODO: cr3 free allocated r3 memory
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(VirtualAddress, pBuffer, ulWriteSize);

	if (!NT_SUCCESS(SwitchCr3(ullOldCr3, NULL))) {
		// TODO: cr3 free allocated r3 memory
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

IOCTL_FUNC(MdlReadMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &ReadMemoryInfo, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulReadSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulReadSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(ulReadSize);
	if (!pBuffer) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(MmMdlReadMemory(ReadMemoryInfo.pEprocess, ReadMemoryInfo.VirtualAddress, pBuffer, ulReadSize))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pR3 = SendDataToR3(g_pTargetProcess, pBuffer, ulReadSize);
	if (!pR3) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	ExFreePool(pBuffer);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}

IOCTL_FUNC(MdlWriteMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		goto Error;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		goto Error;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		goto Error;
	}

	NTSTATUS Status = MmMdlWriteMemory(
		pWriteMemoryInfo->pEprocess, 
		pWriteMemoryInfo->VirtualAddress,
		pWriteMemoryInfo->Data, 
		pWriteMemoryInfo->ulWriteSize
	);
	if (!NT_SUCCESS(Status)) {
		ExFreePool(pWriteMemoryInfo);
		goto Error;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
	return;

Error:
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
	return;
}

IOCTL_FUNC(PhysicalReadMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &ReadMemoryInfo, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulReadSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulReadSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(ulReadSize);
	if (!pBuffer) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(MmPhysicalReadMemory(ReadMemoryInfo.pEprocess, ReadMemoryInfo.VirtualAddress, pBuffer, ulReadSize))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pR3 = SendDataToR3(g_pTargetProcess, pBuffer, ulReadSize);
	if (!pR3) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ExFreePool(pBuffer);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}

IOCTL_FUNC(PhysicalWriteMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		goto Error;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		goto Error;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		goto Error;
	}

	NTSTATUS Status = MmPhysicalWriteMemory(
		pWriteMemoryInfo->pEprocess,
		pWriteMemoryInfo->VirtualAddress,
		pWriteMemoryInfo->Data,
		pWriteMemoryInfo->ulWriteSize
	);
	if (!NT_SUCCESS(Status)) {
		ExFreePool(pWriteMemoryInfo);
		goto Error;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
	return;

Error:
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
	return;
}

IOCTL_FUNC(Cr3ReadMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	READ_MEMORY_INFO ReadMemoryInfo = { 0 };
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &ReadMemoryInfo, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulReadSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulReadSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(ulReadSize);
	if (!pBuffer) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(MmCr3ReadMemory(ReadMemoryInfo.pEprocess, ReadMemoryInfo.VirtualAddress, pBuffer, ulReadSize))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PVOID pR3 = SendDataToR3(g_pTargetProcess, pBuffer, ulReadSize);
	if (!pR3) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pBuffer);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ExFreePool(pBuffer);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}

IOCTL_FUNC(Cr3WriteMemory) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		goto Error;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		goto Error;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		goto Error;
	}

	NTSTATUS Status = MmCr3WriteMemory(
		pWriteMemoryInfo->pEprocess,
		pWriteMemoryInfo->VirtualAddress,
		pWriteMemoryInfo->Data,
		pWriteMemoryInfo->ulWriteSize
	);
	if (!NT_SUCCESS(Status)) {
		ExFreePool(pWriteMemoryInfo);
		goto Error;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
	return;

Error:
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
	return;
}

PVOID __MdlMapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
) {
	if (!pMemoryMap) {
		return STATUS_UNSUCCESSFUL;
	}

	KAPC_STATE ApcState = { 0 };
	KeAttachProcess(pMemoryMap->pEprocess, &ApcState);

	PMDL pMdl = IoAllocateMdl(pMemoryMap->pVirtualToMap, pMemoryMap->ulLength, FALSE, FALSE, NULL);
	if (!pMdl) {
		goto Error;
	}
	pMemoryMap->Parameter.pMdl = pMdl;

	MmBuildMdlForNonPagedPool(pMdl);
	MmProbeAndLockPages(pMdl, KernelMode, IoModifyAccess);
	pMemoryMap->Flag.Mdl.bIsProbeAndLock = TRUE;

	PVOID p = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
	if (!p) {
		goto Error;
	}

	KeUnstackDetachProcess(&ApcState);
	return p;

Error:
	KeUnstackDetachProcess(&ApcState);
	return NULL;
}

VOID __MdlUnmapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
) {
	if (!pMemoryMap) {
		return;
	}

	KAPC_STATE ApcState = { 0 };
	KeAttachProcess(pMemoryMap->pEprocess, &ApcState);


	if (pMemoryMap->Flag.Mdl.bIsProbeAndLock) {
		MmUnlockPages(pMemoryMap->Parameter.pMdl);
		pMemoryMap->Flag.Mdl.bIsProbeAndLock = FALSE;
	}
	
	if (pMemoryMap->Parameter.pMdl) {
		IoFreeMdl(pMemoryMap->Parameter.pMdl);
		pMemoryMap->Parameter.pMdl = NULL;
	}
	KeUnstackDetachProcess(&ApcState);
}

PVOID __PhysicalMapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
) {
	if (!pMemoryMap) {
		return NULL;
	}

	KAPC_STATE ApcState = { 0 };
	KeAttachProcess(pMemoryMap->pEprocess, &ApcState);

	PHYSICAL_ADDRESS PhysicalAddress = MmGetPhysicalAddress(pMemoryMap->pVirtualToMap);
	if (!PhysicalAddress.QuadPart) {
		goto Error;
	}
	pMemoryMap->Parameter.PhysicalAddress = PhysicalAddress;

	PVOID p = MmMapIoSpace(PhysicalAddress, pMemoryMap->ulLength, MmNonCached);
	if (!p) {
		goto Error;
	}

	KeUnstackDetachProcess(&ApcState);
	return STATUS_SUCCESS;

Error:
	KeUnstackDetachProcess(&ApcState);
	return NULL;
}

VOID __PhysicalUnmapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
) {
	if (!pMemoryMap) {
		return;
	}

	if (pMemoryMap->Parameter.PhysicalAddress.QuadPart) {
		MmUnmapIoSpace(pMemoryMap->pVirtual, pMemoryMap->ulLength);
		pMemoryMap->Parameter.PhysicalAddress.QuadPart = 0;
	}
}

VOID MmUnmapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
) {
	if (!pMemoryMap) {
		return;
	}

	switch (pMemoryMap->Method) {
	case MdlMap:
		__MdlUnmapMemory(pMemoryMap);
		break;
	case PhysicalMap:
		__PhysicalUnmapMemory(pMemoryMap);
		break;
	default:
		break;
	}

	pMemoryMap->pVirtual = NULL;
}

NTSTATUS MmMapMemory(
	_Out_ PMEMORY_MAP pMemoryMap,
	_Out_ PVOID* ppVirtual,
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pVirtualToMap,
	_In_ MAP_METHOD Method,
	_In_ ULONG ulLength
) {
	if (!pMemoryMap || !ppVirtual || !ulLength) {
		return STATUS_UNSUCCESSFUL;
	}

	pMemoryMap->pEprocess = pEprocess;
	pMemoryMap->Method = Method;
	pMemoryMap->ulLength = ulLength;
	pMemoryMap->pVirtualToMap = pVirtualToMap;

	PVOID pVirtual = NULL;
	switch (Method) {
	case MdlMap:
		pVirtual = __MdlMapMemory(pMemoryMap);
		break;
	case PhysicalMap:
		pVirtual = __PhysicalMapMemory(pMemoryMap);
		break;
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}

	if (!pVirtual) {
		MmUnmapMemory(pMemoryMap);
		return STATUS_UNSUCCESSFUL;
	}

	pMemoryMap->pVirtual = pVirtual;
	*ppVirtual = pVirtual;
	return STATUS_SUCCESS;
}