#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

using WSTRING = std::wstring;
using STRING = std::string;

template<typename T>
using Vector = std::vector<T>;