#pragma once
#include <Windows.h>
#include "R3S_Define.h"
#include "R3S_TypeDefine.h"

class DLL_EXPORT UtilerDriver {
public:
	HANDLE m_hDriver;

public:
	UtilerDriver();
	~UtilerDriver();

	BOOL Unload();

	PVOID MdlReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID MdlWriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	PVOID PhysicalReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID PhysicalWriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	PVOID Cr3ReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID Cr3WriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	BOOL TerminateProcess(PVOID pEprocess);
};

