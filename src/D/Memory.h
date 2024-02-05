#pragma once
#include <ntifs.h>
#include "Ioctl.h"

NTSTATUS MmMdlReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

NTSTATUS MmMdlWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

NTSTATUS MmPhysicalReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

NTSTATUS MmPhysicalWriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

NTSTATUS MmCr3ReadMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_Out_ PVOID pBuffer,
	_In_ ULONG ulReadSize
);

NTSTATUS MmCr3WriteMemory(
	_In_ PEPROCESS pEprocess,
	_In_ PVOID VirtualAddress,
	_In_ PVOID pBuffer,
	_In_ ULONG ulWriteSize
);

IOCTL_FUNC(MdlReadMemory);
IOCTL_FUNC(MdlWriteMemory);

IOCTL_FUNC(PhysicalReadMemory);
IOCTL_FUNC(PhysicalWriteMemory);

IOCTL_FUNC(Cr3ReadMemory);
IOCTL_FUNC(Cr3WriteMemory);

typedef enum _MAP_METHOD {
	MdlMap,
	PhysicalMap
}MAP_METHOD, * PMAP_METHOD;

typedef struct _MDL_MAP_FLAG {
	BOOLEAN bIsProbeAndLock;
}MDL_MAP_FLAG, * PMDL_MAP_FLAG;

typedef struct _PHYSICAL_MAP_FLAG {
	BOOLEAN Placeholder;
}PHYSICAL_MAP_FLAG, * PPHYSICAL_MAP_FLAG;

typedef struct _MEMORY_MAP {
	PEPROCESS pEprocess;
	MAP_METHOD Method;
	union {
		PMDL pMdl;
		PHYSICAL_ADDRESS PhysicalAddress;
	} Parameter;
	union {
		MDL_MAP_FLAG Mdl;
		PHYSICAL_MAP_FLAG Physical;
	} Flag;
	ULONG ulLength;
	PVOID pVirtualToMap;
	PVOID pVirtual;
}MEMORY_MAP, * PMEMORY_MAP;

NTSTATUS MmMapMemory(
	_Out_ PMEMORY_MAP pMemoryMap,
	_Out_ PVOID* ppVirtual,
	_In_ PEPROCESS pEprocess,
	_In_ PVOID pVirtualToMap,
	_In_ MAP_METHOD Method,
	_In_ ULONG ulLength
);

VOID MmUnmapMemory(
	_Inout_ PMEMORY_MAP pMemoryMap
);

#define MmMdlMapMemory(pMemoryMap, ppVirtual, pEprocess, pVirtualToMap, ulLength) \
	MmMapMemory(pMemoryMap, ppVirtual, pEprocess, pVirtualToMap, MdlMap, ulLength)

#define MmPhysicalMapMemory(pMemoryMap, ppVirtual, pEprocess, pVirtualToMap, ulLength) \
	MmMapMemory(pMemoryMap, ppVirtual, pEprocess, pVirtualToMap, PhysicalMap, ulLength)
