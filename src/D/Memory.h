#pragma once
#include <ntifs.h>
#include "Ioctl.h"

NTSTATUS MmMdlReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

PVOID MmMdlWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

NTSTATUS MmPhysicalReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

PVOID MmPhysicalWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

NTSTATUS MmCr3ReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

PVOID MmCr3WriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

IOCTL_FUNC(MdlReadMemory);
IOCTL_FUNC(MdlWriteMemory);

IOCTL_FUNC(PhysicalReadMemory);
IOCTL_FUNC(PhysicalWriteMemory);

IOCTL_FUNC(Cr3ReadMemory);
IOCTL_FUNC(Cr3WriteMemory);