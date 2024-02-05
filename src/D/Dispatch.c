#include "Dispatch.h"
#include "ControlCode.h"
#include "Ioctl.h"
#include "Process.h"
#include "Object.h"

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
	case IOCTL_SET_TARGET_PROCESS:
		IoctlSetTargetProcess(&IrpData);
		break;
	case IOCTL_COLLECT_PROCESS_INFO:
		IoctlCollectProcessInfo(&IrpData);
		break;
	case IOCTL_COLLECT_OBJECT_TYPE_INFO:
		IoctlCollectObjectTypeInfo(&IrpData);
		break;
	case IOCTL_COLLECT_PROCESS_DETAIL:
		IoctlCollectProcessDetail(&IrpData);
		break;
	case IOCTL_COLLECT_OBJECT_TYPE_DETAIL:
		IoctlCollectObjectTypeDetail(&IrpData);
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