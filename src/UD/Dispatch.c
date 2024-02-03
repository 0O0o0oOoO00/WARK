#include "Dispatch.h"
#include "R0S_Ioctl.h"
#include "PS_UD_ControlCode.h"
#include "Memory.h"
#include "Process.h"

PDEVICE_OBJECT g_pDeviceObject;

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

	switch (IrpData.pIoStack->Parameters.DeviceIoControl.IoControlCode) {
	case UD_IOCTL_SET_TARGET_PROCESS:
		IoctlSetTargetProcess(&IrpData);
		break;
	case UD_IOCTL_MDL_READ_MEMORY:
		IoctlMdlReadMemory(&IrpData);
		break;
	case UD_IOCTL_MDL_WRITE_MEMORY:
		IoctlMdlWriteMemory(&IrpData);
		break;
	case UD_IOCTL_PHYSICAL_READ_MEMORY:
		IoctlPhysicalReadMemory(&IrpData);
		break;
	case UD_IOCTL_PHYSICAL_WRITE_MEMORY:
		IoctlPhysicalWriteMemory(&IrpData);
		break;
	case UD_IOCTL_CR3_READ_MEMORY:
		IoctlCr3ReadMemory(&IrpData);
		break;
	case UD_IOCTL_CR3_WRITE_MEMORY:
		IoctlCr3WriteMemory(&IrpData);
		break;
	case UD_IOCTL_TERMINATE_PROCESS:
		IoctlTerminateProcess(&IrpData);
		break;
	default:
		goto FinishIrp;
		break;
	}

	Status = IrpData.Status;
	Information = IrpData.Information;

FinishIrp:
	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = Information;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}