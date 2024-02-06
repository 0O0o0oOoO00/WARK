#include "Module.hpp"
#include "Util.hpp"

ModuleInfo::ModuleInfo(
	PMODULE_INFO pModuleInfo
) {
	if (!pModuleInfo) {
		return;
	}

	m_pDllBase = pModuleInfo->pDllBase;
	m_pEntryPoint = pModuleInfo->pEntryPoint;
	m_ulSizeOfImage = pModuleInfo->ulSizeOfImage;
	m_usLoadCount = pModuleInfo->usLoadCount;

	if (pModuleInfo->pBaseDllName) {
		m_szBaseDllName = WSTRING(pModuleInfo->pBaseDllName);
		FreeMemory(pModuleInfo->pBaseDllName);
	}
	
	if (pModuleInfo->pFullDllName) {
		m_szFullDllName = WSTRING(pModuleInfo->pFullDllName);
		FreeMemory(pModuleInfo->pFullDllName);
	}
	FreeMemory(pModuleInfo);
}

