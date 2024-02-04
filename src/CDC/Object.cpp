#include "Object.hpp"
#include "Collector.hpp"
#include "PS_CD_ControlCode.h"
#include "R3S_Util.h"

ObjectTypeInfo::ObjectTypeInfo(
	OBJECT_TYPE_INFO* pObjectTypeInfo
) {
	if (!pObjectTypeInfo) {
		return;
	}
	m_pObjectType = pObjectTypeInfo->pObjectType;
	m_pDefaultObject = pObjectTypeInfo->pDefaultObject;
	m_ucIndex = pObjectTypeInfo->ucIndex;
	m_ulTotalNumberOfObjects = pObjectTypeInfo->ulTotalNumberOfObjects;
	m_ulTotalNumberOfHandles = pObjectTypeInfo->ulTotalNumberOfHandles;
	if (pObjectTypeInfo->szName) {
		m_szName = WSTRING(pObjectTypeInfo->szName);
		FreeMemory(pObjectTypeInfo->szName);
	}
	FreeMemory(pObjectTypeInfo);
}

ObjectTypeDetail::ObjectTypeDetail(
	OBJECT_TYPE_DETAIL* pObjectTypeDetail
) {
	if (!pObjectTypeDetail) {
		return;
	}

	m_pObjectType = pObjectTypeDetail->pObjectType;
	m_usObjectTypeFlags = pObjectTypeDetail->usObjectTypeFlags;
	m_ulObjectTypeCode = pObjectTypeDetail->ulObjectTypeCode;
	m_ulInvalidAttributes = pObjectTypeDetail->ulInvalidAttributes;
	m_ulValidAccessMask = pObjectTypeDetail->ulValidAccessMask;
	m_ulRetainAccess = pObjectTypeDetail->ulRetainAccess;
	m_ulPoolType = pObjectTypeDetail->ulPoolType;
	m_ulDefaultPagedPoolCharge = pObjectTypeDetail->ulDefaultPagedPoolCharge;
	m_ulDefaultNonPagedPoolCharge = pObjectTypeDetail->ulDefaultNonPagedPoolCharge;
	m_pDumpProcedure = pObjectTypeDetail->pDumpProcedure;
	m_pOpenProcedure = pObjectTypeDetail->pOpenProcedure;
	m_pCloseProcedure = pObjectTypeDetail->pCloseProcedure;
	m_pDeleteProcedure = pObjectTypeDetail->pDeleteProcedure;
	m_pParseProcedure = pObjectTypeDetail->pParseProcedure;
	m_pParseProcedureEx = pObjectTypeDetail->pParseProcedureEx;
	m_pSecurityProcedure = pObjectTypeDetail->pSecurityProcedure;
	m_pQueryNameProcedure = pObjectTypeDetail->pQueryNameProcedure;
	m_pOkayToCloseProcedure = pObjectTypeDetail->pOkayToCloseProcedure;
	m_ulWaitObjectFlagMask = pObjectTypeDetail->ulWaitObjectFlagMask;
	m_usWaitObjectFlagOffset = pObjectTypeDetail->usWaitObjectFlagOffset;
	m_usWaitObjectPointerOffset = pObjectTypeDetail->usWaitObjectPointerOffset;
}

