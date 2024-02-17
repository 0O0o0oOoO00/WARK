#include "KString.h"
#include "Util.h"

PKSTRING NewEmptyString() {
	PKSTRING pKString = MmAllocateZeroedNonPagedMemory(sizeof(KSTRING));
	pKString->AllocType = OnHeap;
	return pKString;
}

NTSTATUS InitStringWithString(
	_Out_ PKSTRING pBase,
	_In_ PKSTRING pString
) {
	if (!pBase || !pString) {
		return STATUS_UNSUCCESSFUL;
	}

	switch (pString->Type) {
	case NoneContentType:
		return STATUS_SUCCESS;
		break;
	case UnicodeStringType:
		return InitStringWithUnicodeString(pBase, &(pString->UString));
		break;
	case AnsiStringType:
		return InitStringWithAnsiString(pBase, &(pString->AString));
		break;
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS InitStringWithUnicodeString(
	_In_ PKSTRING pString,
	_In_ PUNICODE_STRING pUString
) {
	if (!pString || !pUString) {
		return STATUS_UNSUCCESSFUL;
	}
	if (!pUString->Buffer) {
		goto Return;
	}

	USHORT usBufferSize = pUString->MaximumLength;
	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(usBufferSize);
	RtlInitEmptyUnicodeString(&(pString->UString), pBuffer, usBufferSize);

	RtlCopyUnicodeString(&(pString->UString), pUString);

Return:
	pString->Type = UnicodeStringType;
	return STATUS_SUCCESS;
}

NTSTATUS InitStringWithRawUnicodeString(
	_In_ PKSTRING pString,
	_In_ PWCHAR pRawUString
) {
	if (!pString || !pRawUString) {
		return STATUS_UNSUCCESSFUL;
	}

	UNICODE_STRING UString = { 0 };
	RtlInitUnicodeString(&UString, pRawUString);

	pString->UString.Buffer = MmAllocateZeroedNonPagedMemory(UString.MaximumLength);
	pString->UString.MaximumLength = UString.MaximumLength;

	RtlCopyUnicodeString(&(pString->UString), &UString);

	pString->Type = UnicodeStringType;
	return STATUS_SUCCESS;
}

NTSTATUS InitStringWithAnsiString(
	_In_ PKSTRING pString,
	_In_ PANSI_STRING pAString
) {
	if (!pString || !pAString) {
		return STATUS_UNSUCCESSFUL;
	}
	if (!pAString->Buffer) {
		goto Return;
	}

	USHORT usBufferSize = pAString->MaximumLength;
	PVOID pBuffer = MmAllocateZeroedNonPagedMemory(usBufferSize);
	RtlInitEmptyAnsiString(&(pString->AString), pBuffer, usBufferSize);

	RtlCopyString(&(pString->AString), pAString);

Return:
	pString->Type = AnsiStringType;
	return STATUS_SUCCESS;
}

NTSTATUS InitStringWithRawAnsiString(
	_In_ PKSTRING pString,
	_In_ PCHAR pRawAString
) {
	if (!pString || !pRawAString) {
		return STATUS_UNSUCCESSFUL;
	}

	ANSI_STRING AString = { 0 };
	RtlInitAnsiString(&AString, pRawAString);

	pString->AString.Buffer = MmAllocateZeroedNonPagedMemory(AString.MaximumLength);
	pString->AString.MaximumLength = AString.MaximumLength;

	RtlCopyString(&(pString->AString), &AString);

	pString->Type = AnsiStringType;
	return STATUS_SUCCESS;
}

PKSTRING NewStringWithUnicodeString(
	_In_ PUNICODE_STRING pUString
) {
	if (!pUString) {
		return NULL;
	}
	
	PKSTRING pString = NewEmptyString();
	if (!pString) {
		return NULL;
	}
	if (!NT_SUCCESS(InitStringWithUnicodeString(pString, pUString))) {
		FreeString(pString);
		return NULL;
	}

	return pString;
}

PKSTRING NewStringWithRawUnicodeString(
	_In_ PWCHAR pRawUString
) {
	if (!pRawUString) {
		return NULL;
	}

	PKSTRING pString = NewEmptyString();
	if (!pString) {
		return NULL;
	}
	if (!NT_SUCCESS(InitStringWithRawUnicodeString(pString, pRawUString))) {
		FreeString(pString);
		return NULL;
	}

	return pString;
}

PKSTRING NewStringWithAnsiString(
	_In_ PANSI_STRING pAString
) {
	if (!pAString) {
		return NULL;
	}

	PKSTRING pString = NewEmptyString();
	if (!pString) {
		return NULL;
	}
	if (!NT_SUCCESS(InitStringWithAnsiString(pString, pAString))) {
		FreeString(pString);
		return NULL;
	}

	return pString;
}

PKSTRING NewStringWithRawAnsiString(
	_In_ PCHAR pAString
) {
	if (!pAString) {
		return NULL;
	}

	PKSTRING pString = NewEmptyString();
	if (!pString) {
		return NULL;
	}
	if (!NT_SUCCESS(InitStringWithRawAnsiString(pString, pAString))) {
		FreeString(pString);
		return NULL;
	}

	return pString;
}

VOID FreeString(
	_In_ PKSTRING pString
) {
	if (!pString) {
		return;
	}

	switch (pString->Type) {
	case UnicodeStringType:
		RtlFreeUnicodeString(&(pString->UString));
		break;
	case AnsiStringType:
		RtlFreeAnsiString(&(pString->AString));
		break;
	case NoneContentType:
	default:
		break;
	}

	if (pString->AllocType == OnHeap) {
		ExFreePool(pString);
	}
}

NTSTATUS ExpandStringBufferLength(
	_In_ PKSTRING pString,
	_In_ USHORT usNewLength
) {
	if (!pString || !usNewLength) {
		return STATUS_UNSUCCESSFUL;
	}

	PVOID pNewBuffer = NULL;

	switch (pString->Type) {
	case UnicodeStringType: {
		pNewBuffer = MmAllocateZeroedNonPagedMemory(usNewLength);

		RtlCopyMemory(pNewBuffer, pString->UString.Buffer, pString->UString.MaximumLength);
		ExFreePool(pString->UString.Buffer);

		pString->UString.Buffer = pNewBuffer;
		pString->UString.MaximumLength = usNewLength;

		break;
	}
	case AnsiStringType: {
		pNewBuffer = MmAllocateZeroedNonPagedMemory(usNewLength);

		RtlCopyMemory(pNewBuffer, pString->AString.Buffer, pString->AString.MaximumLength);
		ExFreePool(pString->AString.Buffer);

		pString->AString.Buffer = pNewBuffer;
		pString->AString.MaximumLength = usNewLength;

		break;
	}
	case NoneContentType:
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
}

USHORT RawAnsiStringLength(
	_In_ PCHAR pRawAString
) {
	if (!pRawAString) {
		return 0;
	}

	USHORT usLength = 0;
	for (USHORT i = 0; pRawAString[i] != '\0'; i++) {
		usLength++;
	}
	
	return usLength * sizeof(CHAR);
}

USHORT RawUnicodeStringLength(
	_In_ PWCHAR pRawUString
) {
	if (!pRawUString) {
		return 0;
	}

	USHORT usLength = 0;
	for (USHORT i = 0; pRawUString[i] != L'\0'; i++) {
		usLength++;
	}

	return usLength * sizeof(WCHAR);
}

NTSTATUS AppendUnicodeString(
	_In_ PKSTRING pBase,
	_In_ PUNICODE_STRING pUString
) {
	if (!pBase || !pUString) {
		return STATUS_UNSUCCESSFUL;
	}

	switch (pBase->Type) {
	case NoneContentType: {
		InitStringWithUnicodeString(pBase, pUString);
		break;
	}
	case UnicodeStringType: {
		ExpandStringBufferLength(pBase, pBase->UString.MaximumLength + pUString->MaximumLength);

		RtlAppendUnicodeStringToString(&(pBase->UString), pUString);

		break;
	}
	case AnsiStringType: {
		ExpandStringBufferLength(pBase, pBase->AString.MaximumLength + pUString->MaximumLength);

		ANSI_STRING szAString = { 0 };
		RtlUnicodeStringToAnsiString(&szAString, pUString, TRUE);

		RtlAppendStringToString(&(pBase->AString), &szAString);

		RtlFreeAnsiString(&szAString);

		break;
	}
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
}

NTSTATUS AppendAnsiString(
	_In_ PKSTRING pBase,
	_In_ PANSI_STRING pAString
) {
	if (!pBase || !pAString) {
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS FuncStatus;

	switch (pBase->Type) {
	case NoneContentType: {
		return InitStringWithAnsiString(pBase, pAString);
		break;
	}
	case UnicodeStringType: {
		FuncStatus = ExpandStringBufferLength(pBase, pBase->UString.MaximumLength + pAString->MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		UNICODE_STRING szUString = { 0 };
		FuncStatus = RtlAnsiStringToUnicodeString(&szUString, pAString, TRUE);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->UString), &szUString);
		if (!NT_SUCCESS(FuncStatus)) {
			RtlFreeUnicodeString(&szUString);
			goto Error;
		}

		RtlFreeUnicodeString(&szUString);

		break;
	}
	case AnsiStringType: {
		FuncStatus = ExpandStringBufferLength(pBase, pBase->AString.MaximumLength + pAString->MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->AString), pAString);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		break;
	}
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
Error:
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS AppendRawAnsiString(
	_In_ PKSTRING pBase,
	_In_ PCHAR pRawAString
) {
	if (!pBase || !pRawAString) {
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS FuncStatus;

	ANSI_STRING szAString = { 0 };
	RtlInitAnsiString(&szAString, pRawAString);

	switch (pBase->Type) {
	case NoneContentType: {
		return InitStringWithRawAnsiString(pBase, pRawAString);
		break;
	}
	case UnicodeStringType: {
		UNICODE_STRING szUString = { 0 };
		FuncStatus = RtlAnsiStringToUnicodeString(&szUString, &szAString, TRUE);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}
		
		FuncStatus = ExpandStringBufferLength(pBase, pBase->UString.MaximumLength + szUString.MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			RtlFreeUnicodeString(&szUString);
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->UString), &szUString);
		if (!NT_SUCCESS(FuncStatus)) {
			RtlFreeUnicodeString(&szUString);
			goto Error;
		}

		RtlFreeUnicodeString(&szUString);

		break;
	}
	case AnsiStringType: {
		FuncStatus = ExpandStringBufferLength(pBase, pBase->AString.MaximumLength + szAString.MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->AString), &szAString);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		break;
	}
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
Error:
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS AppendRawUnicodeString(
	_In_ PKSTRING pBase,
	_In_ PWCHAR pRawUString
) {
	if (!pBase || !pRawUString) {
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS FuncStatus;

	UNICODE_STRING szUString = { 0 };
	RtlInitUnicodeString(&szUString, pRawUString);

	switch (pBase->Type) {
	case NoneContentType: {
		return InitStringWithRawUnicodeString(pBase, pRawUString);
		break;
	}
	case UnicodeStringType: {
		FuncStatus = ExpandStringBufferLength(pBase, pBase->UString.MaximumLength + szUString.MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->UString), &szUString);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}

		break;
	}
	case AnsiStringType: {
		ANSI_STRING szAString = { 0 };
		FuncStatus = RtlUnicodeStringToAnsiString(&szAString, &szUString, TRUE);
		if (!NT_SUCCESS(FuncStatus)) {
			goto Error;
		}
		
		FuncStatus = ExpandStringBufferLength(pBase, pBase->AString.MaximumLength + szAString.MaximumLength);
		if (!NT_SUCCESS(FuncStatus)) {
			RtlFreeAnsiString(&szAString);
			goto Error;
		}

		FuncStatus = RtlAppendStringToString(&(pBase->AString), &szAString);
		if (!NT_SUCCESS(FuncStatus)) {
			RtlFreeAnsiString(&szAString);
			goto Error;
		}

		RtlFreeAnsiString(&szAString);
		break;
	}
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_SUCCESS;
Error:
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS AppendString(
	_In_ PKSTRING pBase,
	_In_ PKSTRING pString
) {
	if (!pBase || !pString) {
		return STATUS_UNSUCCESSFUL;
	}

	switch (pString->Type) {
	case NoneContentType:
		return InitStringWithString(pBase, pString);
		break;
	case UnicodeStringType:
		return AppendUnicodeString(pBase, &(pString->UString));
		break;
	case AnsiStringType: 
		return AppendAnsiString(pBase, &(pString->AString));
		break;
	default:
		return STATUS_UNSUCCESSFUL;
		break;
	}
	return STATUS_UNSUCCESSFUL;
}

WCHAR A2U(
	_In_ CHAR AChar
) {
	CHAR szABuffer[2] = { AChar, '\0' };
	ANSI_STRING szAString = {
		.Length = sizeof(CHAR) * 1,
		.MaximumLength = sizeof(CHAR) * 2,
		.Buffer = szABuffer
	};
	UNICODE_STRING szWString = { 0 };
	RtlAnsiStringToUnicodeString(&szWString, &szAString, TRUE);

	WCHAR WChar = szWString.Buffer[0];

	RtlFreeUnicodeString(&szWString);
	return WChar;
}

CHAR U2A(
	_In_ WCHAR WChar
) {
	WCHAR szWBuffer[2] = { WChar, L'\0' };
	UNICODE_STRING szWString = {
		.Length = sizeof(WCHAR) * 1,
		.MaximumLength = sizeof(WCHAR) * 2,
		.Buffer = szWBuffer
	};
	ANSI_STRING szAString = { 0 };
	RtlUnicodeStringToAnsiString(&szAString, &szWString, TRUE);

	CHAR AChar = szAString.Buffer[0];

	RtlFreeAnsiString(&szAString);
	return AChar;
}

PWCHAR* SplitRawUnicodeString(
	_In_ PWCHAR pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 1;
	USHORT usLength = RawUnicodeStringLength(pString);
	if (!usLength) {
		goto Error;
	}
	USHORT usStringLength = usLength / sizeof(WCHAR);

	for (ULONG i = 0; i < usStringLength; i++) {
		if (pString[i] == Separator) {
			ulCount++;
		}
	}

	PWCHAR pStart = pString;
	PWCHAR* pSplited = MmAllocateZeroedNonPagedMemory(sizeof(PWCHAR) * ulCount);
	if (!pSplited) {
		return NULL;
	}
	USHORT usSplitedLength = 0;
	for (ULONG i = 0, j = 0; i <= usStringLength; i++) {
		if (pString[i] == Separator || pString[i] == L'\0') {
			USHORT usPartLength = i * sizeof(WCHAR) - usSplitedLength;
			PWCHAR pPart = MmAllocateZeroedNonPagedMemory(usPartLength + sizeof(WCHAR));
			RtlCopyMemory(pPart, pStart, usPartLength);
			pSplited[j++] = pPart;
			pStart = CALC_ADDRESS(pString, (i + 1) * sizeof(WCHAR));
			usSplitedLength = i * sizeof(WCHAR);
		}
	}
	*pCount = ulCount;
	return pSplited;
Error:
	*pCount = 0;
	return NULL;
}

PUNICODE_STRING* SplitUnicodeString(
	_In_ PUNICODE_STRING pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 0;
	PWCHAR* pSplitedRaw = SplitRawUnicodeString(pString->Buffer, Separator, &ulCount);
	if (!pSplitedRaw) {
		goto Error;
	}
	PUNICODE_STRING* pSplited = MmAllocateZeroedNonPagedMemory(sizeof(PUNICODE_STRING) * ulCount);
	if (!pSplited) {
		ExFreePool(pSplitedRaw);
		goto Error;
	}
	for (ULONG i = 0; i < ulCount; i++) {
		PUNICODE_STRING pPart = MmAllocateZeroedNonPagedMemory(sizeof(UNICODE_STRING));
		RtlInitUnicodeString(pPart, pSplitedRaw[i]);
		pSplited[i] = pPart;
	}

	ExFreePool(pSplitedRaw);

	*pCount = ulCount;
	return pSplited;
Error:
	*pCount = 0;
	return NULL;
}

PCHAR* SplitRawAnsiString(
	_In_ PCHAR pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 1;
	USHORT usLength = RawAnsiStringLength(pString);
	if (!usLength) {
		goto Error;
	}
	USHORT usStringLength = usLength / sizeof(CHAR);

	for (ULONG i = 0; i < usStringLength; i++) {
		if (pString[i] == Separator) {
			ulCount++;
		}
	}

	PCHAR pStart = pString;
	PCHAR* pSplited = MmAllocateZeroedNonPagedMemory(sizeof(PCHAR) * ulCount);
	if (!pSplited) {
		goto Error;
	}
	USHORT usSplitedLength = 0;
	for (ULONG i = 0, j = 0; i <= usStringLength; i++) {
		if (pString[i] == Separator || pString[i] == '\0') {
			USHORT usPartLength = i * sizeof(WCHAR) - usSplitedLength;
			PCHAR pPart = MmAllocateZeroedNonPagedMemory(usPartLength + sizeof(CHAR));
			RtlCopyMemory(pPart, pStart, usPartLength);
			pSplited[j++] = pPart;
			pStart = CALC_ADDRESS(pString, (i + 1) * sizeof(CHAR));
			usSplitedLength = i * sizeof(CHAR);
		}
	}
	*pCount = ulCount;
	return pSplited;
Error:
	*pCount = 0;
	return NULL;
}

PANSI_STRING* SplitAnsiString(
	_In_ PANSI_STRING pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 0;
	PCHAR* pSplitedRaw = SplitRawAnsiString(pString->Buffer, Separator, &ulCount);
	if (!pSplitedRaw) {
		goto Error;
	}

	PANSI_STRING* pSplited = MmAllocateZeroedNonPagedMemory(sizeof(PANSI_STRING) * ulCount);
	if (!pSplited) {
		ExFreePool(pSplitedRaw);
		goto Error;
	}
	for (ULONG i = 0; i < ulCount; i++) {
		PANSI_STRING pPart = MmAllocateZeroedNonPagedMemory(sizeof(ANSI_STRING));
		RtlInitAnsiString(pPart, pSplitedRaw[i]);
		pSplited[i] = pPart;
	}

	ExFreePool(pSplitedRaw);

	*pCount = ulCount;
	return pSplited;
Error:
	*pCount = 0;
	return NULL;
}

PKSTRING* SplitStringWithUnicode(
	_In_ PKSTRING pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 0;
	PKSTRING* pSplictedString = NULL;

	switch (pString->Type) {
	case UnicodeStringType: {
		PUNICODE_STRING* pSplited = SplitUnicodeString(&(pString->UString), Separator, &ulCount);
		if (!pSplited) {
			goto Error;
		}
		
		pSplictedString = MmAllocateZeroedNonPagedMemory(sizeof(PKSTRING) * ulCount);
		if (!pSplictedString) {
			ExFreePool(pSplited);
			goto Error;
		}

		for (ULONG i = 0; i < ulCount; i++) {
			pSplictedString[i] = NewStringWithUnicodeString(pSplited[i]);
		}
		ExFreePool(pSplited);
		break;
	}
	case AnsiStringType: {
		PANSI_STRING* pSplited = SplitAnsiString(&(pString->AString), U2A(Separator), &ulCount);
		if (!pSplited) {
			goto Error;
		}
		
		pSplictedString = MmAllocateZeroedNonPagedMemory(sizeof(PKSTRING) * ulCount);
		if (!pSplictedString) {
			ExFreePool(pSplited);
			goto Error;
		}
		for (ULONG i = 0; i < ulCount; i++) {
			pSplictedString[i] = NewStringWithAnsiString(pSplited[i]);
		}
		ExFreePool(pSplited);
		break;
	}
	default:
		return NULL;
		break;
	}
	*pCount = ulCount;
	return pSplictedString;
Error:
	*pCount = 0;
	return NULL;
}

PKSTRING* SplitStringWithAnsi(
	_In_ PKSTRING pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
) {
	if (!pCount) {
		return NULL;
	}
	if (!pString) {
		goto Error;
	}

	ULONG ulCount = 0;
	PKSTRING* pSplictedString = NULL;

	switch (pString->Type) {
	case UnicodeStringType: {
		PUNICODE_STRING* pSplited = SplitUnicodeString(&(pString->UString), A2U(Separator), &ulCount);
		if (!pSplited) {
			goto Error;
		}

		pSplictedString = MmAllocateZeroedNonPagedMemory(sizeof(PKSTRING) * ulCount);
		if (!pSplictedString) {
			ExFreePool(pSplited);
			goto Error;
		}
		for (ULONG i = 0; i < ulCount; i++) {
			pSplictedString[i] = NewStringWithUnicodeString(pSplited[i]);
		}
		ExFreePool(pSplited);
		break;
	}
	case AnsiStringType: {
		PANSI_STRING* pSplited = SplitAnsiString(&(pString->AString), Separator, &ulCount);
		if (!pSplited) {
			goto Error;
		}
		pSplictedString = MmAllocateZeroedNonPagedMemory(sizeof(PKSTRING) * ulCount);
		if (!pSplictedString) {
			ExFreePool(pSplited);
			goto Error;
		}
		for (ULONG i = 0; i < ulCount; i++) {
			pSplictedString[i] = NewStringWithAnsiString(pSplited[i]);
		}
		ExFreePool(pSplited);
		break;
	}
	default:
		return NULL;
		break;
	}
	*pCount = ulCount;
	return pSplictedString;
Error:
	*pCount = 0;
	return NULL;
}

PUNICODE_STRING GetUStringFromString(
	_In_ PKSTRING pKString
) {
	if (!pKString) {
		goto Error;
	}

	NTSTATUS FuncStatus;

	PUNICODE_STRING pUString = MmAllocateZeroedNonPagedMemory(sizeof(UNICODE_STRING));
	if (!pUString) {
		goto Error;
	}

	switch (pKString->Type) {
	case NoneContentType: {
		break;
	}
	case UnicodeStringType: {
		PWCHAR pURaw = MmAllocateZeroedNonPagedMemory(pKString->UString.MaximumLength);
		if (!pURaw) {
			ExFreePool(pUString);
			goto Error;
		}

		RtlCopyMemory(pURaw, pKString->UString.Buffer, pKString->UString.MaximumLength);

		pUString->Buffer = pURaw;
		pUString->Length = pKString->UString.Length;
		pUString->MaximumLength = pKString->UString.MaximumLength;
		break;
	}
	case AnsiStringType: {
		FuncStatus = RtlAnsiStringToUnicodeString(pUString, &(pKString->AString), TRUE);
		if (!NT_SUCCESS(FuncStatus)) {
			ExFreePool(pUString);
			goto Error;
		}
		return pUString;
		break;
	}
	default:
		break;
	}

	return pUString;
Error:
	return NULL;
}

PANSI_STRING GetAStringFromString(
	_In_ PKSTRING pKString
) {
	if (!pKString) {
		goto Error;
	}

	NTSTATUS FuncStatus;

	PANSI_STRING pAString = MmAllocateZeroedNonPagedMemory(sizeof(ANSI_STRING));
	if (!pAString) {
		goto Error;
	}

	switch (pKString->Type) {
	case NoneContentType: {
		break;
	}
	case UnicodeStringType: {
		FuncStatus = RtlUnicodeStringToAnsiString(pAString, &(pKString->UString), TRUE);
		if (!NT_SUCCESS(FuncStatus)) {
			ExFreePool(pAString);
			goto Error;
		}

		break;
	}
	case AnsiStringType: {
		PCHAR pARaw = MmAllocateZeroedNonPagedMemory(pKString->AString.MaximumLength);
		if (!pARaw) {
			ExFreePool(pAString);
			goto Error;
		}
		RtlCopyMemory(pARaw, pKString->AString.Buffer, pKString->AString.MaximumLength);

		pAString->Buffer = pARaw;
		pAString->Length = pKString->AString.Length;
		pAString->MaximumLength = pKString->AString.MaximumLength;
		break;
	}
	default:
		break;
	}

	return pAString;
Error:
	return NULL;
}

VOID ClearString(
	_In_ PKSTRING pKString
) {
	if (!pKString) {
		return;
	}

	switch (pKString->Type) {
	case NoneContentType: {
		break;
	}
	case UnicodeStringType: {
		RtlFreeUnicodeString(&(pKString->UString));
		break;
	}
	case AnsiStringType: {
		RtlFreeAnsiString(&(pKString->AString));
		break;
	}
	default:
		break;
	}
	pKString->Type = NoneContentType;
}
