#include "Dispatch.h"
#include "ControlCode.h"
#include "Ioctl.h"
#include "Process.h"
#include "Object.h"
#include "Memory.h"
#include "Module.h"

PDEVICE_OBJECT g_pDeviceObject;

static CONST IOCTL_FUNCTION IOCTL_FUNC_LIST[] = {
	IoctlSetTargetProcess,
	IoctlCollectProcessInfo,
	IoctlCollectObjectTypeInfo,
	IoctlCollectProcessDetail,
	IoctlCollectObjectTypeDetail,
	IoctlMdlReadMemory,
	IoctlMdlWriteMemory,
	IoctlPhysicalReadMemory,
	IoctlPhysicalWriteMemory,
	IoctlCr3ReadMemory,
	IoctlCr3WriteMemory,
	IoctlTerminateProcess,
	IoctlCollectModuleInfo
};

#define IOCTL_FUNC_COUNT (sizeof(IOCTL_FUNC_LIST) / sizeof(IOCTL_FUNCTION))

NTSTATUS DispatchDefault(
	PDEVICE_OBJECT pDeviceObject,
	PIRP pIrp
) {
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(
	PDEVICE_OBJECT pDeviceObject,
	PIRP pIrp
) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	ULONG_PTR Information = -1;

	if (pDeviceObject != g_pDeviceObject) {
		Status = STATUS_SUCCESS;
		Information = 0;
		goto FinishIrp;
	}
	IRP_DATA IrpData;
	InitializeIrpData(pIrp, &IrpData);
	IOCTL_CODE ControlCode = { 0 };
	ControlCode.Code = IrpData.pIoStack->Parameters.DeviceIoControl.IoControlCode;
	ULONG ulIoctlFuncIndex = ControlCode.Function - IOCTL_FUNCTION_CODE_RESERVED;
	if (ulIoctlFuncIndex >= IOCTL_FUNC_COUNT) {
		Status = STATUS_UNSUCCESSFUL;
		Information = 0;
		goto FinishIrp;
	}

	IOCTL_FUNC_LIST[ulIoctlFuncIndex](&IrpData);

	Status = IrpData.Status;
	Information = IrpData.Information;

FinishIrp:
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = Information;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}