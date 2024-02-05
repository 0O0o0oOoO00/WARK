#pragma once
#ifdef RING0
#include <ntifs.h>
#else
#include <windows.h>
#endif // RING0

#pragma pack(8)

typedef struct _READ_MEMORY_INFO {
	PVOID pEprocess;
	PVOID VirtualAddress;
}READ_MEMORY_INFO, * PREAD_MEMORY_INFO;

typedef struct _WRITE_MEMORY_INFO {
	PVOID pEprocess;
	ULONG ulWriteSize;
	UCHAR Data[];
}WRITE_MEMORY_INFO, * PWRITE_MEMORY_INFO;

#pragma pack()