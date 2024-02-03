#pragma once
#include <ntifs.h>

ULONGLONG GetProcessCr3(
	_In_ PEPROCESS pEprocess
);