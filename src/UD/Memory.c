#include "Memory.h"
#include "R0S_Utils.h"
#include "Util.h"
#include "R0S_Utils.h"
#include "R0S_Io.h"
#include "PS_UD_InputStruct.h"

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

	KAPC_STATE ApcState;
	KeAttachProcess(pEprocess, &ApcState);

	PMDL pMdl = IoAllocateMdl(VirtualAddress, ulReadSize, FALSE, FALSE, NULL);
	if (!pMdl) {
		KeUnstackDetachProcess(&ApcState);
		return STATUS_UNSUCCESSFUL;
	}
	MmBuildMdlForNonPagedPool(pMdl);
	MmProbeAndLockPages(pMdl, KernelMode, IoModifyAccess);
	PVOID p = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
	if (!p) {
		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);
		KeUnstackDetachProcess(&ApcState);
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pBuffer, p, ulReadSize);

	MmUnlockPages(pMdl);
	IoFreeMdl(pMdl);
	KeUnstackDetachProcess(&ApcState);
	return STATUS_SUCCESS;
}

PVOID MmMdlWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !pBuffer || !ulWriteSize) {
		return NULL;
	}

	KAPC_STATE ApcState;
	KeAttachProcess(pEprocess, &ApcState);

	PVOID VirtualAddress = MmAllocateR3Memory(pEprocess, ulWriteSize);
	if (!VirtualAddress) {
		KeUnstackDetachProcess(&ApcState);
		return NULL;
	}

	PMDL pMdl = IoAllocateMdl(VirtualAddress, ulWriteSize, FALSE, FALSE, NULL);
	if (!pMdl) {
		// TODO: mdl free allocated r3 memory
		KeUnstackDetachProcess(&ApcState);
		return NULL;
	}

	MmBuildMdlForNonPagedPool(pMdl);
	MmProbeAndLockPages(pMdl, KernelMode, IoModifyAccess);
	PVOID p = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
	if (!p) {
		// TODO: mdl free allocated r3 memory
		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);
		KeUnstackDetachProcess(&ApcState);
		return NULL;
	}

	RtlCopyMemory(p, pBuffer, ulWriteSize);

	MmUnlockPages(pMdl);
	IoFreeMdl(pMdl);
	KeUnstackDetachProcess(&ApcState);
	return p;
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

	KAPC_STATE ApcState;
	KeAttachProcess(pEprocess, &ApcState);

	PHYSICAL_ADDRESS PhysicalAddress = MmGetPhysicalAddress(VirtualAddress);
	if (!PhysicalAddress.QuadPart) {
		KeUnstackDetachProcess(&ApcState);
		return STATUS_UNSUCCESSFUL;
	}
	PVOID p = MmMapIoSpace(PhysicalAddress, ulReadSize, MmNonCached);
	if (!p) {
		KeUnstackDetachProcess(&ApcState);
		return STATUS_UNSUCCESSFUL;
	}

	RtlCopyMemory(pBuffer, p, ulReadSize);

	MmUnmapIoSpace(p, ulReadSize);
	KeUnstackDetachProcess(&ApcState);
	return STATUS_SUCCESS;
}

PVOID MmPhysicalWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !pBuffer || !ulWriteSize) {
		return NULL;
	}

	KAPC_STATE ApcState;
	KeAttachProcess(pEprocess, &ApcState);

	PVOID VirtualAddress = MmAllocateR3Memory(pEprocess, ulWriteSize);
	if (!VirtualAddress) {
		return NULL;
	}

	PHYSICAL_ADDRESS PhysicalAddress = MmGetPhysicalAddress(VirtualAddress);
	if (!PhysicalAddress.QuadPart) {
		// TODO: physical free allocated r3 memory
		KeUnstackDetachProcess(&ApcState);
		return NULL;
	}
	PVOID p = MmMapIoSpace(PhysicalAddress, ulWriteSize, MmNonCached);
	if (!p) {
		// TODO: physical free allocated r3 memory
		KeUnstackDetachProcess(&ApcState);
		return NULL;
	}

	RtlCopyMemory(p, pBuffer, ulWriteSize);

	MmUnmapIoSpace(p, ulWriteSize);
	KeUnstackDetachProcess(&ApcState);
	return p;
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

PVOID MmCr3WriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
) {
	if (!pEprocess || !pBuffer || !ulWriteSize) {
		return NULL;
	}

	PVOID pR3 = MmAllocateR3Memory(pEprocess, ulWriteSize);
	if (!pR3) {
		return NULL;
	}
	ULONGLONG ullProcessCr3 = GetProcessCr3(pEprocess);
	if (!ullProcessCr3) {
		// TODO: cr3 free allocated r3 memory
		return NULL;
	}
	ULONGLONG ullOldCr3 = 0;
	if (!NT_SUCCESS(SwitchCr3(ullProcessCr3, &ullOldCr3))) {
		// TODO: cr3 free allocated r3 memory
		return NULL;
	}

	RtlCopyMemory(pR3, pBuffer, ulWriteSize);

	if (!NT_SUCCESS(SwitchCr3(ullOldCr3, NULL))) {
		// TODO: cr3 free allocated r3 memory
		return NULL;
	}
	return pR3;
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
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo  = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3 = MmMdlWriteMemory(pWriteMemoryInfo->pEprocess, pWriteMemoryInfo->Data, pWriteMemoryInfo->ulWriteSize);
	if (!pR3) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
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
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3 = MmPhysicalWriteMemory(pWriteMemoryInfo->pEprocess, pWriteMemoryInfo->Data, pWriteMemoryInfo->ulWriteSize);
	if (!pR3) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
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
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulStructSize = OutputBufferLengthOfIoPackage(&IoPackage);
	if (!ulStructSize) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PWRITE_MEMORY_INFO pWriteMemoryInfo = MmAllocateZeroedNonPagedMemory(ulStructSize);
	if (!pWriteMemoryInfo) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3 = MmCr3WriteMemory(pWriteMemoryInfo->pEprocess, pWriteMemoryInfo->Data, pWriteMemoryInfo->ulWriteSize);
	if (!pR3) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		ExFreePool(pWriteMemoryInfo);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ExFreePool(pWriteMemoryInfo);
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}