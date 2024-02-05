#pragma once
#include <ntifs.h>

typedef struct _IO_PACKAGE {
	ULONG IoMethod;
	ULONG ulInputBufferOperatedLength;
	ULONG ulInputBufferLength;
	PVOID pInputBuffer;
	ULONG ulOuttputBufferOperatedLength;
	ULONG ulOutputBufferLength;
	PVOID pOutputBuffer;
}IO_PACKAGE, * PIO_PACKAGE;

typedef struct _IRP_DATA {
	PIRP pIrp;
	PIO_STACK_LOCATION pIoStack;
	NTSTATUS Status;
	ULONG_PTR Information;
}IRP_DATA, * PIRP_DATA;

NTSTATUS InitializeIoPackage(
	_Out_ PIO_PACKAGE pIoPackage,
	_In_ PIRP_DATA pIrpData
);

NTSTATUS WriteToIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_In_ PVOID pBuffer,
	_In_ SIZE_T uLengthToWrite,
	_Out_ SIZE_T* pWrittenLength,
	_In_ BOOLEAN bForce
);

NTSTATUS ReadIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_Out_ PVOID pBuffer,
	_In_ SIZE_T LengthToRead,
	_Out_ SIZE_T* pWrittenLength,
	_In_ BOOLEAN bForce
);

ULONG OutputBufferLengthOfIoPackage(
	_In_ PIO_PACKAGE pIoPackage
);

ULONG InputBufferLengthOfIoPackage(
	_In_ PIO_PACKAGE pIoPackage
);

NTSTATUS ReadEntireIoPackage(
	_In_ PIO_PACKAGE pIoPackage,
	_Out_ PVOID pBuffer,
	_Out_opt_ SIZE_T* pReadLength
);

#define SET_IRP_DATA_STATUS(p, i, s)  \
	(p)->Information = i; \
	(p)->Status = s;

#define IOCTL_FUNC(name) \
	VOID Ioctl##name( \
		_Inout_ PIRP_DATA pIrpData \
	)

NTSTATUS InitializeIrpData(
	_In_ PIRP pIrp,
	_Out_ PIRP_DATA pIrpData
);