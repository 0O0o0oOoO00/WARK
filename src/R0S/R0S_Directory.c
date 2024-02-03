#include "R0S_Directory.h"
#include "R0S_Undocumented.h"

NTSTATUS EnumDirectoryObject(
	_In_ PUNICODE_STRING pDirectory,
	_In_ ENUM_DIRECTORY_OBJECT_CALLBACK Callback,
	_In_ PVOID Parameter
) {
	HANDLE hDriverDirectory = NULL;
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };

	InitializeObjectAttributes(&ObjectAttributes, pDirectory, OBJ_CASE_INSENSITIVE, NULL, NULL);
	if (!NT_SUCCESS(ZwOpenDirectoryObject(&hDriverDirectory, DIRECTORY_QUERY, &ObjectAttributes))) {
		return STATUS_UNSUCCESSFUL;
	}

	ULONG  Context = 0;
	ULONG  ReturnLength = 0;
	POBJECT_DIRECTORY_INFORMATION pDirectoryObjectInfo = ExAllocatePool(NonPagedPool, OBJECT_DIRECTORY_INFORMATION_BUFFER_LENGTH);
	if (!pDirectoryObjectInfo) {
		return STATUS_UNSUCCESSFUL;
	}

	do {
		RtlZeroMemory(pDirectoryObjectInfo, OBJECT_DIRECTORY_INFORMATION_BUFFER_LENGTH);
		NTSTATUS Status = ZwQueryDirectoryObject(
			hDriverDirectory,
			pDirectoryObjectInfo,
			OBJECT_DIRECTORY_INFORMATION_BUFFER_LENGTH,
			TRUE,
			FALSE,
			&Context,
			&ReturnLength
		);

		if (!NT_SUCCESS(Status)) {
			ExFreePool(pDirectoryObjectInfo);
			return STATUS_UNSUCCESSFUL;
		}

		ENUM_STATUS EnumStatus = Callback(pDirectoryObjectInfo, Parameter);
		switch (EnumStatus) {
		case ENUM_CONTINUE:
			break;
		case ENUM_BREAK:
			goto SuccessReturn;
			break;
		case ENUM_ERROR:
			return STATUS_UNSUCCESSFUL;
			break;
		default:
			return STATUS_UNSUCCESSFUL;
			break;
		}
	// TODO: need better while condition
	} while (TRUE);

	ExFreePool(pDirectoryObjectInfo);

SuccessReturn:
	return STATUS_SUCCESS;
}