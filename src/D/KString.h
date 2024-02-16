#pragma once
#include <ntifs.h>
#include "Define.h"

typedef enum _STRING_TYPE {
	NoneContentType = 0,
	UnicodeStringType,
	AnsiStringType
}STRING_TYPE, * PSTRING_TYPE;

typedef struct _KSTRING {
	STRING_TYPE Type;
	ALLOC_TYPE AllocType;
	union {
		UNICODE_STRING UString;
		ANSI_STRING AString;
	};
}KSTRING, * PKSTRING;

PKSTRING NewEmptyString();

VOID FreeString(
	_In_ PKSTRING pString
);

NTSTATUS InitStringWithString(
	_Out_ PKSTRING pBase,
	_In_ PKSTRING pString
);
NTSTATUS InitStringWithUnicodeString(
	_In_ PKSTRING pString,
	_In_ PUNICODE_STRING pUString
);
NTSTATUS InitStringWithRawUnicodeString(
	_In_ PKSTRING pString,
	_In_ PWCHAR pRawUString
);
NTSTATUS InitStringWithAnsiString(
	_In_ PKSTRING pString,
	_In_ PANSI_STRING pAString
);
NTSTATUS InitStringWithRawAnsiString(
	_In_ PKSTRING pString,
	_In_ PCHAR pRawAString
);

PKSTRING NewStringWithUnicodeString(
	_In_ PUNICODE_STRING pUString
);
PKSTRING NewStringWithRawUnicodeString(
	_In_ PWCHAR pRawUString
);
PKSTRING NewStringWithAnsiString(
	_In_ PANSI_STRING pAString
);
PKSTRING NewStringWithRawAnsiString(
	_In_ PCHAR pAString
);

NTSTATUS ExpandStringBufferLength(
	_In_ PKSTRING pString,
	_In_ USHORT usNewLength
);

USHORT RawAnsiStringLength(
	_In_ PCHAR pRawAString
);
USHORT RawUnicodeStringLength(
	_In_ PWCHAR pRawUString
);

NTSTATUS AppendUnicodeString(
	_In_ PKSTRING pBase,
	_In_ PUNICODE_STRING pUString
);
NTSTATUS AppendAnsiString(
	_In_ PKSTRING pBase,
	_In_ PANSI_STRING pAString
);
NTSTATUS AppendRawAnsiString(
	_In_ PKSTRING pBase,
	_In_ PCHAR pRawAString
);
NTSTATUS AppendRawUnicodeString(
	_In_ PKSTRING pBase,
	_In_ PWCHAR pRawUString
);
NTSTATUS AppendString(
	_In_ PKSTRING pBase,
	_In_ PKSTRING pString
);

WCHAR A2U(
	_In_ CHAR AChar
);
CHAR U2A(
	_In_ WCHAR WChar
);

PWCHAR* SplitRawUnicodeString(
	_In_ PWCHAR pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
);
PUNICODE_STRING* SplitUnicodeString(
	_In_ PUNICODE_STRING pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
);
PCHAR* SplitRawAnsiString(
	_In_ PCHAR pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
);
PANSI_STRING* SplitAnsiString(
	_In_ PANSI_STRING pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
);
PKSTRING* SplitStringWithUnicode(
	_In_ PKSTRING pString,
	_In_ WCHAR Separator,
	_Out_ PULONG pCount
);
PKSTRING* SplitStringWithAnsi(
	_In_ PKSTRING pString,
	_In_ CHAR Separator,
	_Out_ PULONG pCount
);

PUNICODE_STRING GetUStringFromString(
	_In_ PKSTRING pKString
);
PANSI_STRING GetAStringFromString(
	_In_ PKSTRING pKString
);

VOID ClearString(
	_In_ PKSTRING pKString
);