#include "Process.h"
#include "R0S_Io.h"
#include "R0S_Utils.h"
#include "R0S_NtoskrnlStruct.h"
#include "R0S_Define.h"
#include "R0S_Process.h"

PEPROCESS g_pTargetProcess;

ENUM_STATUS SetTargetProcessCallback(
	_In_ PNOK_INCOMPLETE_EPROCESS pEprocess,
	_In_opt_ PVOID Parameter
) {
	if (!pEprocess || !Parameter) {
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
	if (!ulTargetProcessId) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(EnumEprocess(SetTargetProcessCallback, &ulTargetProcessId))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_SUCCESS);
}

IOCTL_FUNC(TerminateProcess) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PEPROCESS pEprocess = NULL;
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &pEprocess, NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!pEprocess || !MmIsAddressValid(pEprocess)) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	HANDLE hProcess = NULL;
	if (!NT_SUCCESS(ObOpenObjectByPointer(pEprocess, OBJ_KERNEL_HANDLE, NULL, 0, *PsProcessType, KernelMode, &hProcess))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, 0, ZwTerminateProcess(hProcess, STATUS_SUCCESS));
}