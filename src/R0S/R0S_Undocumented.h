#pragma once
#include <ntifs.h>

NTSYSAPI NTSTATUS NTAPI ZwQueryInformationProcess(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_ PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength
);

NTSYSAPI NTSTATUS NTAPI ZwQueryDirectoryObject (
	_In_ HANDLE hDirectory,
	_Out_opt_ PVOID pBuffer,
	_In_ ULONG ulLength,
	_In_ BOOLEAN bReturnSingleEntry,
	_In_ BOOLEAN bRestartScan,
	_Inout_ PULONG pContext,
	_Out_opt_ PULONG pReturnLength
);

NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(
    _In_ PUNICODE_STRING pObjectName,
    _In_ ULONG ulAttributes,
    _In_opt_ PACCESS_STATE pPassedAccessState,
    _In_opt_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_TYPE pObjectType,
    _In_ KPROCESSOR_MODE AccessMode,
    _Inout_opt_ PVOID pParseContext,
    _Out_ PVOID* ppObject
);

NTSTATUS NTAPI MmCopyVirtualMemory(
    _In_ PEPROCESS pSourceProcess,
    _In_ PVOID pSourceAddress,
    _In_ PEPROCESS pTargetProcess,
    _In_ PVOID pTargetAddress,
    _In_ SIZE_T BufferSize,
    _In_ KPROCESSOR_MODE PreviousMode,
    _Out_ PSIZE_T pReturnSize
);

extern POBJECT_TYPE* IoDriverObjectType;
extern POBJECT_TYPE* PsProcessType;
// extern POBJECT_TYPE* ObTypeIndexTable;