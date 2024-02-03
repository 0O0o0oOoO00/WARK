#pragma once
#include <Windows.h>
#include <string>
#include "R3S_Define.h"
#include "PS_CD_OutputStruct.h"
#include "R3S_TypeDefine.h"

class DLL_EXPORT ObjectTypeInfo {
public:
	PVOID m_pObjectType;
	PVOID m_pDefaultObject;
	UCHAR m_ucIndex;
	ULONG m_ulTotalNumberOfObjects;
	ULONG m_ulTotalNumberOfHandles;
	WSTRING m_szName;

public:
	ObjectTypeInfo() = default;
	ObjectTypeInfo(OBJECT_TYPE_INFO* pObjectTypeInfo);
	~ObjectTypeInfo() {}
};

class DLL_EXPORT ObjectTypeDetail {
public:
	USHORT m_usObjectTypeFlags;
	ULONG m_ulObjectTypeCode;
	ULONG m_ulInvalidAttributes;
	ULONG m_ulValidAccessMask;
	ULONG m_ulRetainAccess;
	ULONG m_ulPoolType;
	ULONG m_ulDefaultPagedPoolCharge;
	ULONG m_ulDefaultNonPagedPoolCharge;
	PVOID m_pDumpProcedure;
	PVOID m_pOpenProcedure;
	PVOID m_pCloseProcedure;
	PVOID m_pDeleteProcedure;
	PVOID m_pParseProcedure;
	PVOID m_pParseProcedureEx;
	PVOID m_pSecurityProcedure;
	PVOID m_pQueryNameProcedure;
	PVOID m_pOkayToCloseProcedure;
	ULONG m_ulWaitObjectFlagMask;
	USHORT m_usWaitObjectFlagOffset;
	USHORT m_usWaitObjectPointerOffset;

public:
	ObjectTypeDetail() = default;
	ObjectTypeDetail(OBJECT_TYPE_DETAIL* pObjectTypeDetail);
	~ObjectTypeDetail() {}
};

