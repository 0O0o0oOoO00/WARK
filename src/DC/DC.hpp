#pragma once
#include <Windows.h>
#include "Define.hpp"
#include "Process.hpp"
#include "Object.hpp"

class DLL_EXPORT Driver {
public:
	HANDLE m_hDriver;

public:
	Driver();
	~Driver();

	BOOL Unload();

	Vector<ProcessInfo> CollectProcessInfo();
	Vector<ObjectTypeInfo> CollectObjectTypeInfo();
	ProcessDetailInfo CollectProcessDetailInfo(PVOID pEprocess);
	ObjectTypeDetail CollectObjectTypeDetail(PVOID pObjectType);

	PVOID MdlReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID MdlWriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	PVOID PhysicalReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID PhysicalWriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	PVOID Cr3ReadMemory(PVOID pEprocess, PVOID VirtualAddress, ULONG ulReadLength);
	PVOID Cr3WriteMemory(PVOID pEprocess, PVOID pBuffer, ULONG ulWriteLength);

	BOOL TerminateProcess(PVOID pEprocess);
};
