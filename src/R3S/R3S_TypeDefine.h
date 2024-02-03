#pragma once

#include <Windows.h>

#ifdef __cplusplus
#include <string>
#include <vector>

using WSTRING = std::wstring;
using STRING = std::string;

template<typename T>
using Vector = std::vector<T>;

#endif // __cplusplus
