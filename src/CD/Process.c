#include "Process.h"
#include "R0S_Process.h"
#include "R0S_Undocumented.h"
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Utils.h"
#include "PS_CD_OutputStruct.h"
#include "R0S_Io.h"
#include "R0S_Define.h"

PEPROCESS g_pTargetProcess;

typedef struct _PROCESS_DETAIL_INFO{
	PEPROCESS pTargetEprocess;
	PPROCESS_DETAIL pProcessDetail;
}PROCESS_DETAIL_INFO, * PPROCESS_DETAIL_INFO;

ENUM_STATUS CollectProcessCallback(
	_In_ PNOK_INCOMPLETE_EPROCESS pEprocess,
	_In_opt_ PVOID Parameter
) {
	if (!pEprocess || ! Parameter) {
		return ENUM_ERROR;
	}

	PVECTOR pProcessInfoVector = (PVECTOR)Parameter;
	PROCESS_INFO ProcessInfo = { 0 };

	ProcessInfo.pEprocess = pEprocess;
	ProcessInfo.ProcessId = pEprocess->UniqueProcessId;
	ProcessInfo.ullOwnerProcessId = pEprocess->OwnerProcessId;
	ProcessInfo.CreateTime = pEprocess->CreateTime;

	if (pEprocess->ImageFilePointer) {
		UNICODE_STRING DosName;
		if (!NT_SUCCESS(IoVolumeDeviceToDosName(pEprocess->ImageFilePointer->DeviceObject, &DosName))) {
			return ENUM_ERROR;
		}
		UNICODE_STRING FullName;
		if (!NT_SUCCESS(RtlEmptyUnicodeString(&FullName, MultipleUnicodeStringLengthBytes(&DosName, &(pEprocess->ImageFilePointer->FileName))))) {
			return ENUM_ERROR;
		}
		if (!NT_SUCCESS(AppendMultipleUnicodeStringToString(&FullName, &DosName, &(pEprocess->ImageFilePointer->FileName)))) {
			return ENUM_ERROR;
		}

		PVOID pR3String = SendDataToR3(g_pTargetProcess, FullName.Buffer, FullName.MaximumLength);
		if (!pR3String) {
			return ENUM_ERROR;
		}
		ProcessInfo.pFullFileName = pR3String;
		
		RtlFreeUnicodeStringBuffer(&FullName);
	}

	PVOID pR3ProcessInfo = SendDataToR3(g_pTargetProcess, &ProcessInfo, sizeof(PROCESS_INFO));
	if (!pR3ProcessInfo) {
		return ENUM_ERROR;
	}
	if (!NT_SUCCESS(VectorPush(pProcessInfoVector, &pR3ProcessInfo))) {
		return ENUM_ERROR;
	}

	return ENUM_CONTINUE;
}

IOCTL_FUNC(CollectProcessInfo) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVECTOR pProcessInfoVector = NewVector(sizeof(PPROCESS_INFO));
	if (!pProcessInfoVector) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(EnumEprocess(CollectProcessCallback, pProcessInfoVector))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3Vector = SendVectorContentToR3(g_pTargetProcess, pProcessInfoVector);
	if (!pR3Vector) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3Vector, sizeof(PVOID), NULL, FALSE))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, VectorItemsCount(pProcessInfoVector), STATUS_SUCCESS);
	FreeVector(pProcessInfoVector);
}

ENUM_STATUS SetTargetProcessCallback(
	_In_ PNOK_INCOMPLETE_EPROCESS pEprocess,
	_In_opt_ PVOID Parameter
) {
	if (!pEprocess) {
		return ENUM_ERROR;
	}
	ULONG ulTargetProcessId = *(PULONG)Parameter;
	if (pEprocess->UniqueProcessId == ulTargetProcessId) {
		g_pTargetProcess = pEprocess;
		return ENUM_BREAK;
	}
	return ENUM_CONTINUE;
}

IOCTL_FUNC(SetTargetProcess) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	ULONG ulTargetProcessId = 0;
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &ulTargetProcessId, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(EnumEprocess(SetTargetProcessCallback, &ulTargetProcessId))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}

ENUM_STATUS CollectProcessDetailCallback(
	_In_ PNOK_INCOMPLETE_EPROCESS pEprocess,
	_In_opt_ PVOID Parameter
) {
	if (!pEprocess || !Parameter) {
		return ENUM_ERROR;
	}

	PPROCESS_DETAIL_INFO pProcessDetailInfo = (PPROCESS_DETAIL_INFO)Parameter;
	if (!pProcessDetailInfo->pProcessDetail || !pProcessDetailInfo->pTargetEprocess) {
		return ENUM_ERROR;
	}

	if (pEprocess != pProcessDetailInfo->pTargetEprocess) {
		return ENUM_CONTINUE;
	}

	PPROCESS_DETAIL pProcessDetail = pProcessDetailInfo->pProcessDetail;

	pProcessDetail->Eprocess.pEprocess = pEprocess;
	pProcessDetail->Eprocess.UniqueProcessId = pEprocess->UniqueProcessId;
	pProcessDetail->Eprocess.CreateTime = pEprocess->CreateTime;
	pProcessDetail->Eprocess.ullPeakVirtualSize = pEprocess->PeakVirtualSize;
	pProcessDetail->Eprocess.ullVirtualSize = pEprocess->VirtualSize;
	pProcessDetail->Eprocess.ullMmReserved = pEprocess->MmReserved;
	pProcessDetail->Eprocess.ulCookie = pEprocess->Cookie;
	pProcessDetail->Eprocess.ullOwnerProcessId = pEprocess->OwnerProcessId;
	pProcessDetail->Eprocess.pDebugPort = pEprocess->DebugPort;
	pProcessDetail->Eprocess.ucPriorityClass = pEprocess->PriorityClass;
	pProcessDetail->Eprocess.pSecurityPort = pEprocess->SecurityPort;
	pProcessDetail->Eprocess.VadHint = pEprocess->VadHint;
	pProcessDetail->Eprocess.ullVadCount = pEprocess->VadCount;
	pProcessDetail->Eprocess.ullVadPhysicalPages = pEprocess->VadPhysicalPages;
	pProcessDetail->Eprocess.ullVadPhysicalPagesLimit = pEprocess->VadPhysicalPagesLimit;
	pProcessDetail->Eprocess.ullSecurityDomain = pEprocess->SecurityDomain;
	pProcessDetail->Eprocess.ullParentSecurityDomain = pEprocess->ParentSecurityDomain;

	pProcessDetail->Pcb.pPcb = &(pEprocess->Pcb);
	pProcessDetail->Pcb.ullDirectoryTableBase = pEprocess->Pcb.DirectoryTableBase;
	pProcessDetail->Pcb.ulKernelTime = pEprocess->Pcb.KernelTime;
	pProcessDetail->Pcb.ulUserTime = pEprocess->Pcb.UserTime;
	pProcessDetail->Pcb.ulReadyTime = pEprocess->Pcb.ReadyTime;
	pProcessDetail->Pcb.ullUserDirectoryTableBase = pEprocess->Pcb.UserDirectoryTableBase;
	pProcessDetail->Pcb.ullKernelWaitTime = pEprocess->Pcb.KernelWaitTime;
	pProcessDetail->Pcb.ullUserWaitTime = pEprocess->Pcb.UserWaitTime;

	if (pEprocess->Peb && MmIsAddressValid(pEprocess->Peb)) {
		pProcessDetail->Peb.pPeb = pEprocess->Peb;
		pProcessDetail->Peb.ucInheritedAddressSpace = pEprocess->Peb->InheritedAddressSpace;
		pProcessDetail->Peb.ucBeingDebugged = pEprocess->Peb->BeingDebugged;
		pProcessDetail->Peb.pImageBaseAddress = pEprocess->Peb->ImageBaseAddress;
		pProcessDetail->Peb.pProcessHeap = pEprocess->Peb->ProcessHeap;
		pProcessDetail->Peb.ulSystemReserved = pEprocess->Peb->SystemReserved;
		pProcessDetail->Peb.ulNumberOfProcessors = pEprocess->Peb->NumberOfProcessors;
		pProcessDetail->Peb.ulNumberOfHeaps = pEprocess->Peb->NumberOfHeaps;
		pProcessDetail->Peb.ulMaximumNumberOfHeaps = pEprocess->Peb->MaximumNumberOfHeaps;
		pProcessDetail->Peb.ulSessionId = pEprocess->Peb->SessionId;
	}
	return ENUM_BREAK;
}

IOCTL_FUNC(CollectProcessDetail) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PEPROCESS pEprocess = 0;
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &pEprocess, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	PROCESS_DETAIL ProcessDetail = { 0 };
	PROCESS_DETAIL_INFO ProcessDetailInfo = { 0 };
	ProcessDetailInfo.pTargetEprocess = pEprocess;
	ProcessDetailInfo.pProcessDetail = &ProcessDetail;

	if (!NT_SUCCESS(EnumEprocess(CollectProcessDetailCallback, &ProcessDetailInfo))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3 = SendDataToR3(g_pTargetProcess, &ProcessDetail, sizeof(PROCESS_DETAIL));
	if (!pR3) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	
	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}