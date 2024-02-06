#pragma once
#include <Windows.h>
#include "Define.hpp"
#include "OutputStruct.h"

class DLL_EXPORT ModuleInfo {
public:
	WSTRING m_szFullDllName;
	WSTRING m_szBaseDllName;
	PVOID m_pDllBase;
	PVOID m_pEntryPoint;
	ULONG m_ulSizeOfImage;
	USHORT m_usLoadCount;
public:
	ModuleInfo() = default;
	ModuleInfo(PMODULE_INFO pModuleInfo);
	~ModuleInfo() {}

private:

};