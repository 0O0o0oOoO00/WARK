#include "Module.h"
#include "Undocument.h"
#include "KVector.h"
#include "OutputStruct.h"
#include "Util.h"
#include "Ioctl.h"

PEPROCESS g_pTargetProcess;

NTSTATUS EnumModule(
	_In_ ENUM_MODULE fnCallback,
	_In_opt_ PVOID Parameter
) {
	if (!fnCallback) {
		return STATUS_UNSUCCESSFUL;
	}

	PKLDR_DATA_TABLE_ENTRY pKldr = CONTAINING_RECORD(&PsLoadedModuleList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	PLIST_ENTRY pFirstList = PsLoadedModuleList.Flink;
	PLIST_ENTRY pList = pFirstList;

	do {
		pKldr = CONTAINING_RECORD(pList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		ENUM_STATUS EnumStatus = fnCallback(pKldr, Parameter);
		switch (EnumStatus) {
		case ENUM_CONTINUE:
			break;
		case ENUM_BREAK:
			goto SuccessReturn;
			break;
		case ENUM_ERROR:
			return STATUS_UNSUCCESSFUL;
			break;
		default:
			return STATUS_UNSUCCESSFUL;
			break;
		}
		pList = pList->Flink;
	} while (pList != pFirstList);
SuccessReturn:
	return STATUS_SUCCESS;
}

PVOID GetNtoskrnlBase() {
	UNICODE_STRING szNtoskrnlName = { 0 };
	RtlInitUnicodeString(&szNtoskrnlName, NTOSKRNL_NAME);

	PKLDR_DATA_TABLE_ENTRY pKldr = CONTAINING_RECORD(&PsLoadedModuleList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	PLIST_ENTRY pFirstList = &PsLoadedModuleList;
	PLIST_ENTRY pList = pFirstList;
	do {
		pKldr = CONTAINING_RECORD(pList, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (RtlCompareUnicodeString(&szNtoskrnlName, &(pKldr->BaseDllName), TRUE) == 0) {
			return pKldr->DllBase;
		}
		pList = pList->Flink;
	} while (pList != pFirstList);
}

ENUM_STATUS CollectModuleInfoCallback(
	_In_ PKLDR_DATA_TABLE_ENTRY pKldrDataTableEntry, 
	_In_opt_ PVOID Parameter
) {
	if (!pKldrDataTableEntry || !Parameter) {
		return ENUM_ERROR;
	}

	PKVECTOR pVector = (PKVECTOR)Parameter;
	MODULE_INFO Info = { 0 };

	Info.pDllBase = pKldrDataTableEntry->DllBase;
	Info.pEntryPoint = pKldrDataTableEntry->EntryPoint;
	Info.ulSizeOfImage = pKldrDataTableEntry->SizeOfImage;
	Info.usLoadCount = pKldrDataTableEntry->LoadCount;

	if (pKldrDataTableEntry->BaseDllName.Buffer) {
		PVOID pR3BaseDllName = SendStringToR3(
			g_pTargetProcess,
			&(pKldrDataTableEntry->BaseDllName)
			);
		if (!pR3BaseDllName) {
			goto Error;
		}
		Info.pBaseDllName = pR3BaseDllName;
	}

	if (pKldrDataTableEntry->FullDllName.Buffer) {
		PVOID pR3FullDllName = SendStringToR3(
			g_pTargetProcess,
			&(pKldrDataTableEntry->FullDllName)
			);
		if (!pR3FullDllName) {
			goto Error;
		}
		Info.pFullDllName = pR3FullDllName;
	}

	PVOID pR3Info = SendDataToR3(g_pTargetProcess, &Info, sizeof(MODULE_INFO));
	if (!pR3Info) {
		goto Error;
	}

	if (!NT_SUCCESS(VectorPush(pVector, &pR3Info))) {
		goto Error;
	}

	return ENUM_CONTINUE;
Error:
	return ENUM_ERROR;
}

IOCTL_FUNC(CollectModuleInfo) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PKVECTOR pModuleInfoVector = NewVector(sizeof(PMODULE_INFO));
	if (!pModuleInfoVector) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(EnumModule(CollectModuleInfoCallback, pModuleInfoVector))) {
		FreeVector(pModuleInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3InfoVector = SendVectorContentToR3(g_pTargetProcess, pModuleInfoVector);
	if (!pR3InfoVector) {
		FreeVector(pModuleInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3InfoVector, sizeof(PVOID), NULL, FALSE))) {
		FreeVector(pModuleInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, VectorItemsCount(pModuleInfoVector), STATUS_SUCCESS);
	FreeVector(pModuleInfoVector);
}
