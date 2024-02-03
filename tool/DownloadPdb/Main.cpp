#include <Windows.h>
#include <DbgHelp.h>
#include <string>
#include <comutil.h>
#include <vector>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "urlmon.lib")

#define LOG(f, ...) printf(f, __VA_ARGS__)
#define SUCCESS(f, ...) LOG(("[+] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define INFO(f, ...) LOG(("[*] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define WARNING(f, ...) LOG(("[!] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__)
#define FAIL(f, ...) LOG(("[-] " + CSTRING(f) + "\n").c_str(), __VA_ARGS__);exit(-1)

template<typename T>
using VECTOR = std::vector<T>;
using CSTRING = std::string;
using WSTRING = std::wstring;
using BSTR_T = _bstr_t;

#define HEX std::hex

WSTRING g_szSavePath(L".\\");
WSTRING g_szDownloadServer(L"https://msdl.microsoft.com/download/symbols");

WSTRING C2W(
    _In_ PCHAR pCstring
) {
    BSTR_T t = pCstring;
    PWCHAR pWchar = (PWCHAR)t;
    WSTRING szWstring = pWchar;
    return szWstring;
}

BOOLEAN DownloadPdb(
    _In_ WSTRING& szFile
) {
    INFO("Start to parse pe file: %ls", szFile.c_str());
    SYMSRV_INDEX_INFOW Info = { 0 };
    CHAR szGuid[128] = { 0 };
    Info.sizeofstruct = sizeof(SYMSRV_INDEX_INFOW);
    if (!SymSrvGetFileIndexInfoW(szFile.c_str(), &Info, NULL)) {
        FAIL("Get guid of pe file failed, pe file: %ls", szFile.c_str());
    }
    sprintf_s(szGuid, sizeof(szGuid),
        "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X",
        Info.guid.Data1,
        Info.guid.Data2,
        Info.guid.Data3,
        Info.guid.Data4[0],
        Info.guid.Data4[1],
        Info.guid.Data4[2],
        Info.guid.Data4[3],
        Info.guid.Data4[4],
        Info.guid.Data4[5],
        Info.guid.Data4[6],
        Info.guid.Data4[7],
        Info.age);
    SUCCESS("Pe file guid: %s", szGuid);
    
    WSTRING szPdbFile(Info.pdbfile);
    WSTRING szDownloadUrl =
        g_szDownloadServer + L"/" +
        szPdbFile + L"/" +
        C2W(szGuid) + L"/" +
        szPdbFile;
    INFO("Download url: %ls", szDownloadUrl.c_str());
    WSTRING szSaveFile(g_szSavePath + szPdbFile);
    INFO("Strat to download pdb of pe file: %ls", szFile.c_str());
    HRESULT hr = URLDownloadToFile(NULL, szDownloadUrl.c_str(), szSaveFile.c_str(), 0, NULL);
    if (FAILED(hr)) {
        FAIL("Download pdb fail with error 0x%llX, pe file: %ls", hr, szFile.c_str());
    }
    SUCCESS("Download success, pdb saved to %ls", szSaveFile.c_str());
    return TRUE;
}

int main(int argc, char* argv[]) {
    VECTOR<WSTRING> PeFileVector;
    CSTRING szCommandInput;
    if (argc == 1) {
        FAIL("At least one pe file required !");
    } else if (argc == 2) {
        szCommandInput = CSTRING(argv[1]);
        if (szCommandInput == "-s") {
            FAIL("Miss save dir and must set at least one pe file to download !");
        }
        PeFileVector.push_back(C2W(argv[1]));
    } else {
        for (ULONG i = 1; i < argc; i++) {
            szCommandInput = CSTRING(argv[i]);
            if (szCommandInput == "-s") {
                g_szSavePath = C2W(argv[++i]);
            }
            else {
                PeFileVector.push_back(C2W(argv[i]));
            }
        }
    }

    INFO("Download server: %ls", g_szDownloadServer.c_str());
    for (WSTRING& i: PeFileVector) {
        DownloadPdb(i);
    }

	return 0;
}