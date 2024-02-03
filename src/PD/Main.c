#include <ntifs.h>
#include "R0S.h"

PDRIVER_OBJECT g_pDriverObject = NULL;
PDEVICE_OBJECT g_pDeviceObject = NULL;

VOID DriverUnload(
	_In_ PDRIVER_OBJECT pDriverObject
) {
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ PUNICODE_STRING pRegistryPath
) {
	g_pDriverObject = pDriverObject;
	pDriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}