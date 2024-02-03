#pragma once
#include <ntifs.h>

#define SET_IRP_DATA_STATUS(p, i, s)  \
	(p)->Information = i; \
	(p)->Status = s;

typedef struct _IRP_DATA {
	PIRP pIrp;
	PIO_STACK_LOCATION pIoStack;
	NTSTATUS Status;
	ULONG_PTR Information;
}IRP_DATA, * PIRP_DATA;

#define IOCTL_FUNC(name) \
	VOID Ioctl##name( \
		_Inout_ PIRP_DATA pIrpData \
	)

NTSTATUS InitializeIrpData(
	_In_ PIRP pIrp,
	_Out_ PIRP_DATA pIrpData
);