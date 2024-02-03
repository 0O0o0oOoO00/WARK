#include <Windows.h>
#include <Dia2.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <comutil.h>  

#pragma comment(lib, "comsuppw.lib")

template<typename T>
using VECTOR = std::vector<T>;
using CSTRING = std::string;
using WSTRING = std::wstring;
using FSTREAM = std::fstream;
using BSTR_T = _bstr_t;

#define HEX std::hex
#define FOUT std::ios::out

#define LOG(f, ...) printf(f, __VA_ARGS__)
#define SUCCESS(f, ...) LOG(("[+] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define INFO(f, ...) LOG(("[*] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define WARNING(f, ...) LOG(("[!] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define FAIL(f, ...) LOG(("[-] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__);exit(-1)

class Symbol {
public:
    CSTRING m_szSymbolName;
    DWORD m_dwOffset;

public:
    Symbol(
        LPCCH pSymbolName,
        DWORD dwOffset
    ) {
        m_szSymbolName = CSTRING(pSymbolName);
        m_dwOffset = dwOffset;
    }
    ~Symbol() {}
};

CSTRING W2C(
    _In_ LPCWCH pWstring
) {
    BSTR_T t = pWstring;
    LPCCH pCchar = (LPCCH)t;
    CSTRING szCstring = pCchar;
    return szCstring;
}

WSTRING C2W(
    _In_ LPCCH pCstring
) {
    BSTR_T t = pCstring;
    LPCWCH pWchar = (LPCWCH)t;
    WSTRING szWstring = pWchar;
    return szWstring;
}

IDiaDataSource* InitDiaSource() {
    IDiaDataSource* pSource = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, __uuidof(IDiaDataSource), (void**)&pSource);
    if (FAILED(hr)) {
        FAIL("Create dia source instance failed with error 0x%llX", hr);
    }
    SUCCESS("Create dia source instance success");
    return pSource;
}

IDiaSession* LoadPdb(
    _In_ IDiaDataSource* pSource,
    _In_ LPCCH pPdbFile
) {
    if (!pSource) {
        FAIL("Invalid dia source, pSource: 0x%llX", pSource);
    }
    if (!pPdbFile) {
        FAIL("Pdb file must be set !");
    }
    HRESULT hr = pSource->loadDataFromPdb(C2W(pPdbFile).c_str());
    IDiaSession* pSession = NULL;
    if (FAILED(hr)) {
        FAIL("Load date from pdb failed with error 0x%llX, file: %s", hr, pPdbFile);
    }
    SUCCESS("Load date from pdb success");
    hr = pSource->openSession(&pSession);
    if (FAILED(hr)) {
        FAIL("Open session failed with error 0x%llX", hr);
    }
    SUCCESS("Open session success");
    return pSession;
}

VECTOR<Symbol> DumpSymbolsOffset(
    _In_ IDiaSession* pSession
) {
    if (!pSession) {
        FAIL("Invalid dia source, pSession: 0x%llX", pSession);
    }

    HRESULT hr = S_OK;
    VECTOR<Symbol> SymbolVector = *new VECTOR<Symbol>;
    IDiaEnumSymbolsByAddr* pEnumSymsByAddr = NULL;
    IDiaSymbol* pSymbol = NULL;
    ULONG ulCelt = 0;
    BSTR pSymbolName = NULL;

    hr = pSession->getSymbolsByAddr(&pEnumSymsByAddr);
    if (FAILED(hr)) {
        FAIL("Get symbols by address failed with error 0x%llX", hr);
    }
    SUCCESS("Get symbols by address success");
    hr = pEnumSymsByAddr->symbolByRVA(0, &pSymbol);
    if (FAILED(hr)) {
        FAIL("Get first symbol failed with error 0x%llX", hr);
    }
    SUCCESS("Get first symbol success");
    INFO("Start to enum symbol");
    DWORD dwOffset = 0;
    hr = pSymbol->get_name(&pSymbolName);
    if (FAILED(hr)) {
        FAIL("Get symbol name failed with error 0x%llX", hr);
    }
    hr = pSymbol->get_relativeVirtualAddress(&dwOffset);
    if (FAILED(hr)) {
        FAIL("Get symbol relative virtual address failed with error 0x%llX", hr);
    }
    INFO("Offset: 0x%llx, name: %ls", dwOffset, pSymbolName);
    SymbolVector.push_back(Symbol(W2C(pSymbolName).c_str(), dwOffset));
    pSymbol->Release();

    while (pEnumSymsByAddr->Next(1, &pSymbol, &ulCelt) == S_OK) {
        hr = pSymbol->get_name(&pSymbolName);
        if (FAILED(hr)) {
            FAIL("Get symbol name failed with error 0x%llX", hr);
        }
        hr = pSymbol->get_relativeVirtualAddress(&dwOffset);
        if (FAILED(hr)) {
            FAIL("Get symbol relative virtual address failed with error 0x%llX", hr);
        }
        INFO("Offset: 0x%llx, name: %ls", dwOffset, pSymbolName);
        SymbolVector.push_back(Symbol(W2C(pSymbolName).c_str(), dwOffset));
        pSymbol->Release();
    }
    return SymbolVector;
}

VOID OutputToFile(
    _In_ VECTOR<Symbol>& SymbolVector,
    _In_ LPCCH pOutputFile
) {
    if (!pOutputFile) {
        FAIL("Output file must be set !");
    }

    INFO("Start to save symbol offset to file: %s", pOutputFile);

    FSTREAM OutputFile(pOutputFile, FOUT);
    ULONG ulCount = 0;
    for (Symbol& i: SymbolVector) {
        if (i.m_szSymbolName.find('@') != -1 ||
            i.m_szSymbolName.find('?') != -1 ||
            i.m_szSymbolName.find('`') != -1 ||
            i.m_szSymbolName.find('\'') != -1 ||
            i.m_szSymbolName.find(':') != -1 ||
            i.m_szSymbolName.find('<') != -1 ||
            i.m_szSymbolName.find('>') != -1 ||
            i.m_szSymbolName.find(',') != -1 ||
            i.m_szSymbolName.find('$') != -1
            ) {
            WARNING("Skip to save cpp or unknown symbol: %s", i.m_szSymbolName.c_str());
            continue;
        }
        if (i.m_dwOffset == 0) {
            WARNING("Skip to save non-offset symbol: %s", i.m_szSymbolName.c_str());
            continue;
        }
        if (i.m_szSymbolName.find('_') == 0) {
            OutputFile << "#define OFFSET" << i.m_szSymbolName << " 0x" << HEX << i.m_dwOffset << "\n";
        } else {
            OutputFile << "#define OFFSET_" << i.m_szSymbolName << " 0x" << HEX << i.m_dwOffset << "\n";
        }
        ulCount++;
    }
    OutputFile.close();
    SUCCESS("%d symbols offset saved !", ulCount);
}

VOID FormatSymbolName(
    _In_ VECTOR<Symbol>& SymbolVector
) {
    for (Symbol& i: SymbolVector) {
        LONG Pos = i.m_szSymbolName.find("-");
        while (Pos != -1) {
            i.m_szSymbolName.replace(Pos, 1, "_");
            Pos = i.m_szSymbolName.find("-");
        }
        Pos = i.m_szSymbolName.find(0x7f);
        while (Pos != -1) {
            i.m_szSymbolName.replace(Pos, 1, "");
            Pos = i.m_szSymbolName.find(0x7f);
        }
    }
}

int main(int argc, char* argv[]) {
    CSTRING szPdbFile;
    CSTRING szOutputFile;
    CSTRING szCommandInput;
    if (argc == 1) {
        FAIL("Pdb file must be set !");
    } else if (argc == 2) {
        szCommandInput = CSTRING(argv[1]);
        if (szCommandInput == "-o") {
            FAIL("Miss output file and must set pdb file");
        }
        szPdbFile = CSTRING(argv[1]);
    } else {
        for (ULONG i = 1; i < argc; i++) {
            szCommandInput = CSTRING(argv[i]);
            if (szCommandInput == "-o") {
                szOutputFile = CSTRING(argv[++i]);
            } else {
                szPdbFile = CSTRING(argv[i]);
            }
        }
    }
    if (szPdbFile.empty()) {
        FAIL("Pdb file must be set !");
    }
    if (szOutputFile.empty()) {
        szOutputFile = CSTRING(szPdbFile).replace(szPdbFile.find(".pdb"), 4, ".h");
    }

    if (FAILED(CoInitialize(NULL))) {
        FAIL("Initialize failed !");
    }
    IDiaDataSource* pSource = InitDiaSource();

    IDiaSession* pSession = LoadPdb(pSource, szPdbFile.c_str());

    VECTOR<Symbol> SymbolVector = DumpSymbolsOffset(pSession);

    FormatSymbolName(SymbolVector);

    OutputToFile(SymbolVector, szOutputFile.c_str());

    CoUninitialize();
    return 0;
}