#pragma once
#include <ntifs.h>

NTSTATUS DispatchDefault(
	PDEVICE_OBJECT pDeviceObject,
	PIRP pIrp
);

NTSTATUS DispatchDeviceControl(
	PDEVICE_OBJECT pDeviceObject,
	PIRP pIrp
);