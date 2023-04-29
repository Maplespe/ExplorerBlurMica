// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include "ShellLoader.h"
#include "HookDef.h"

HMODULE g_hModule = 0;
bool m_isLoaded = false;      //explorer加载标志
bool m_isInitHook = false;    //Hook初始化标志
ULONG_PTR m_gdiplusToken = 0; //GDI初始化标志
int g_sysBuildNumber = 22000; //系统build版本号

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH && !g_hModule) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);

        if (GetModuleHandleW(L"SettingSyncHost.exe")
            || GetModuleHandleW(L"SkyDrive.exe")
            || GetModuleHandleW(L"FileManager.exe")
            || GetModuleHandleW(L"vmtoolsd.exe")
            || GetModuleHandleW(L"svchost.exe")
            || GetModuleHandleW(L"SearchIndexer.exe")
            || GetModuleHandleW(L"WSHost.exe")
            || GetModuleHandleW(L"wmpnetwk.exe")
            || GetModuleHandleW(L"svchost.exe")
            || GetModuleHandleW(L"dllhost.exe")
            || GetModuleHandleW(L"spoolsv.exe"))
        {
            //FreeLibraryAndExitThread(hModule, 0);
            return FALSE;
        }

        //防止别的程序意外加载
        wchar_t pName[MAX_PATH];
        GetModuleFileNameW(NULL, pName, MAX_PATH);
        std::wstring path = std::wstring(pName);
        std::wstring name = path.substr(path.length() - 12, 12);
        if (ConvertTolower(name) == L"explorer.exe")
            m_isLoaded = true;
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        if (m_isInitHook)
            Hook::HookDetachAll();
        if (m_gdiplusToken)
        {
            Gdiplus::GdiplusShutdown(m_gdiplusToken);
        }
    }
    return TRUE;
}

void OnWindowLoad()
{
    //如果按住ESC键则不加载扩展
    if (GetKeyState(VK_ESCAPE) < 0 || !m_isLoaded)
        return;

    if (!m_gdiplusToken)
    {
        //初始化 Gdiplus Init GdiPlus
        Gdiplus::GdiplusStartupInput StartupInput;
        int ret = Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
    }

    //在开机的时候系统就会加载此动态库 那时候启用HOOK是会失败的 等创建窗口的时候再初始化HOOK
    if (!m_isInitHook)
    {
        m_isInitHook = true;

        //获取系统版本号
        HKEY tkey = 0;
        if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", 0, KEY_READ, &tkey))
        {
            wchar_t dwValue[MAX_PATH];
            DWORD dwSzType = REG_SZ;
            DWORD dwSize = MAX_PATH;
            RegQueryValueExW(tkey, L"CurrentBuildNumber", 0, &dwSzType, (LPBYTE)&dwValue, &dwSize);
            int buildNumber = _wtoi(dwValue);
            if (buildNumber != 0)
                g_sysBuildNumber = buildNumber;
            RegCloseKey(tkey);
        }

        //创建钩子 CreateHook
        if (MH_Initialize() == MH_OK)
        {
            Hook::HookAttachAll();
            MH_EnableHook(MH_ALL_HOOKS);
        }
        else
        {
            MessageBoxW(0, L"Failed to initialize disassembly!\nSuspected duplicate load extension", L"ExplorerBlurMica Error", MB_ICONERROR | MB_OK);
            FreeLibraryAndExitThread(g_hModule, 0);
        }
    }

    RefreshConfig();
}