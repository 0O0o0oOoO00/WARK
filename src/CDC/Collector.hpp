#pragma once
#include <Windows.h>
#include <vector>
#include "R3S_Define.h"
#include "Process.hpp"
#include "Object.hpp"
#include "R3S_TypeDefine.h"

class DLL_EXPORT CollectorDriver {
public:
	HANDLE m_hDriver;

public:
	CollectorDriver();
	~CollectorDriver();

	BOOL Unload();

	Vector<ProcessInfo> CollectProcessInfo();
	Vector<ObjectTypeInfo> CollectObjectTypeInfo();
	ProcessDetailInfo CollectProcessDetailInfo(PVOID pEprocess);
	ObjectTypeDetail CollectObjectTypeDetail(PVOID pObjectType);
};

