#include <ntifs.h>
#include "Dispatch.h"
#include "PS_CD_DriverDefine.h"
#include "R0S_Driver.h"

PDRIVER_OBJECT g_pDriverObject = NULL;
PDEVICE_OBJECT g_pDeviceObject = NULL;
PEPROCESS g_pTargetProcess = NULL;
PVOID g_pNtoskrnlBase = NULL;

VOID DriverUnload(
	_In_ PDRIVER_OBJECT pDriverObject
) {
	UNICODE_STRING szSymbolicLinkName;
	RtlInitUnicodeString(&szSymbolicLinkName, CD_DEVICE_SYMBOL);
	IoDeleteSymbolicLink(&szSymbolicLinkName);
	IoDeleteDevice(g_pDeviceObject);
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ PUNICODE_STRING pRegistryPath
) {
	g_pDriverObject = pDriverObject;
	pDriverObject->DriverUnload = DriverUnload;
	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		pDriverObject->MajorFunction[i] = DispatchDefault;
	}
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

	g_pNtoskrnlBase = GetNtoskrnlBase(pDriverObject);

	UNICODE_STRING szDeviceName;
	RtlInitUnicodeString(&szDeviceName, CD_DEVICE_NAME);
	if (!NT_SUCCESS(IoCreateDevice(pDriverObject, 0, &szDeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_pDeviceObject))) {
		return STATUS_UNSUCCESSFUL;
	}

	UNICODE_STRING szSymbolicLinkName;
	RtlInitUnicodeString(&szSymbolicLinkName, CD_DEVICE_SYMBOL);
	if (!NT_SUCCESS(IoCreateSymbolicLink(&szSymbolicLinkName, &szDeviceName))) {
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}