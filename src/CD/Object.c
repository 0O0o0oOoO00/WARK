#include "Object.h"
#include "R0S_Object.h"
#include "R0S_Vector.h"
#include "PS_CD_OutputStruct.h"
#include "R0S_Utils.h"
#include "R0S_Io.h"

PEPROCESS g_pTargetProcess;
PVOID g_pNtoskrnlBase;

typedef struct _COLLECT_OBJECT_TYPE_DETAIL_INFO {
	POBJECT_TYPE pObjectType;
	PVOID pR3Detail;
}COLLECT_OBJECT_TYPE_DETAIL_INFO, * PCOLLECT_OBJECT_TYPE_DETAIL_INFO;

ENUM_STATUS CollectObjectCallback(
	_In_ PNOK_INCOMPLETE_OBJECT_TYPE pObjectType, 
	_In_opt_ PVOID Parameter
) {
	if (!pObjectType || !Parameter) {
		return ENUM_ERROR;
	}

	PVECTOR pR3ObjectInfoVector = (PVECTOR)Parameter;
	OBJECT_TYPE_INFO ObjectTypeInfo = { 0 };

	ObjectTypeInfo.pObjectType = pObjectType;
	ObjectTypeInfo.pDefaultObject = pObjectType->DefaultObject;
	ObjectTypeInfo.ucIndex = pObjectType->Index;
	ObjectTypeInfo.ulTotalNumberOfObjects = pObjectType->TotalNumberOfObjects;
	ObjectTypeInfo.ulTotalNumberOfHandles = pObjectType->TotalNumberOfHandles;
	if (pObjectType->Name.Buffer) {
		PWCHAR szR3Name = SendDataToR3(g_pTargetProcess, pObjectType->Name.Buffer, pObjectType->Name.MaximumLength);
		if (!szR3Name) {
			return ENUM_ERROR;
		}
		ObjectTypeInfo.szName = szR3Name;
	}
	POBJECT_TYPE_INFO pR3ObjectTypeInfo = SendDataToR3(g_pTargetProcess, &ObjectTypeInfo, sizeof(OBJECT_TYPE_INFO));
	if (!pR3ObjectTypeInfo) {
		return ENUM_ERROR;
	}

	if (!NT_SUCCESS(VectorPush(pR3ObjectInfoVector, &pR3ObjectTypeInfo))) {
		return ENUM_ERROR;
	}
	return ENUM_CONTINUE;
}

IOCTL_FUNC(CollectObjectTypeInfo) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVECTOR pR3ObjectInfoVector = NewVector(sizeof(POBJECT_TYPE_INFO));
	if (!pR3ObjectInfoVector) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	if (!NT_SUCCESS(EnumObject(g_pNtoskrnlBase, CollectObjectCallback, pR3ObjectInfoVector))) {
		FreeVector(pR3ObjectInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	PVOID pR3 = SendVectorContentToR3(g_pTargetProcess, pR3ObjectInfoVector);
	if (!pR3) {
		FreeVector(pR3ObjectInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}
	
	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &pR3, sizeof(PVOID), NULL, FALSE))) {
		FreeVector(pR3ObjectInfoVector);
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, VectorItemsCount(pR3ObjectInfoVector), STATUS_SUCCESS);
	FreeVector(pR3ObjectInfoVector);
}

ENUM_STATUS CollectObjectDetailCallback(
	_In_ PNOK_INCOMPLETE_OBJECT_TYPE pObjectType,
	_In_opt_ PVOID Parameter
) {
	if (!pObjectType || !Parameter) {
		return ENUM_ERROR;
	}
	PCOLLECT_OBJECT_TYPE_DETAIL_INFO pInfo = (PCOLLECT_OBJECT_TYPE_DETAIL_INFO)Parameter;
	if (pInfo->pObjectType != pObjectType) {
		return ENUM_CONTINUE;
	}

	OBJECT_TYPE_DETAIL Detail = { 0 };
	NOK_INCOMPLETE_OBJECT_TYPE_INITIALIZER TypeInfo = pObjectType->TypeInfo;

	Detail.pObjectType = pObjectType;
	Detail.usObjectTypeFlags = TypeInfo.ObjectTypeFlags;
	Detail.ulObjectTypeCode = TypeInfo.ObjectTypeCode;
	Detail.ulInvalidAttributes = TypeInfo.InvalidAttributes;
	Detail.ulValidAccessMask = TypeInfo.ValidAccessMask;
	Detail.ulRetainAccess = TypeInfo.RetainAccess;
	Detail.ulPoolType = TypeInfo.PoolType;
	Detail.ulDefaultPagedPoolCharge = TypeInfo.DefaultPagedPoolCharge;
	Detail.ulDefaultNonPagedPoolCharge = TypeInfo.DefaultNonPagedPoolCharge;
	Detail.pDumpProcedure = TypeInfo.DumpProcedure;
	Detail.pOpenProcedure = TypeInfo.OpenProcedure;
	Detail.pCloseProcedure = TypeInfo.CloseProcedure;
	Detail.pDeleteProcedure = TypeInfo.DeleteProcedure;
	Detail.pParseProcedure = TypeInfo.ParseProcedure;
	Detail.pParseProcedureEx = TypeInfo.ParseProcedureEx;
	Detail.pSecurityProcedure = TypeInfo.SecurityProcedure;
	Detail.pQueryNameProcedure = TypeInfo.QueryNameProcedure;
	Detail.pOkayToCloseProcedure = TypeInfo.OkayToCloseProcedure;
	Detail.ulWaitObjectFlagMask = TypeInfo.WaitObjectFlagMask;
	Detail.usWaitObjectFlagOffset = TypeInfo.WaitObjectFlagOffset;
	Detail.usWaitObjectPointerOffset = TypeInfo.WaitObjectPointerOffset;

	PVOID pR3Detail = SendDataToR3(g_pTargetProcess, &Detail, sizeof(OBJECT_TYPE_DETAIL));
	if (!pR3Detail) {
		return ENUM_ERROR;
	}
	pInfo->pR3Detail = pR3Detail;

	return ENUM_BREAK;
}

IOCTL_FUNC(CollectObjectTypeDetail) {
	IO_PACKAGE IoPackage;
	if (!NT_SUCCESS(InitializeIoPackage(&IoPackage, pIrpData))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	COLLECT_OBJECT_TYPE_DETAIL_INFO Info = { 0 };
	if (!NT_SUCCESS(ReadEntireIoPackage(&IoPackage, &(Info.pObjectType), NULL))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(EnumObject(g_pNtoskrnlBase, CollectObjectDetailCallback, &Info))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	if (!NT_SUCCESS(WriteToIoPackage(&IoPackage, &(Info.pR3Detail), sizeof(POBJECT_TYPE_DETAIL), NULL, FALSE))) {
		SET_IRP_DATA_STATUS(pIrpData, 0, STATUS_UNSUCCESSFUL);
		return;
	}

	SET_IRP_DATA_STATUS(pIrpData, sizeof(POBJECT_TYPE_DETAIL), STATUS_SUCCESS);
}