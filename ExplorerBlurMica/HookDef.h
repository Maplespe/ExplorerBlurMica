/*
* Hook函数定义
*
* Author: Maple
* date: 2021-7-13 Create
* Copyright winmoes.com
*/

#pragma once
#include "framework.h"
#include "minhook\MinHook.h"
#include "Helper.h"

//刷新配置
extern void RefreshConfig();

namespace Hook {

    extern int hookIndex;

    inline void _HookError_()
    {
        MessageBoxW(0, (L"Failed to create hook <" + std::to_wstring(hookIndex) + L">").c_str(), L"ExplorerBlurMica Error", MB_ICONERROR | MB_OK);
    }

    template<typename T>
    class HookFun
    {
    public:
        HookFun(void* target, void* detour)
        {
            m_target = target;
            m_detour = detour;
        }

        HookFun(LPCWSTR moduleName, LPCSTR funcName, void* detour)
        {
            m_moduleName = moduleName, m_funName = (LPSTR)funcName;
            m_isModule = true;
            m_detour = detour;
        }

        ~HookFun()
        {
            Detach();
        }

        void Attach()
        {
            MH_STATUS ret = MH_OK;
            if (m_isModule)
                ret = MH_CreateHookApi(m_moduleName.c_str(), m_funName, m_detour, (void**)&m_original);
            else
                ret = MH_CreateHook(m_target, m_detour, (void**)&m_original);
            if (ret != MH_OK)
            {
                _HookError_();
            }
            else
                m_isAttach = true;

            hookIndex++;
        }

        void Detach()
        {
            if (m_isAttach)
                MH_RemoveHook(m_target);
        }

        template <typename... Args>
        inline auto Org(Args&& ... args)
        {
            return m_original(args...);
        }

    private:

        std::wstring m_moduleName;
        LPSTR m_funName;

        bool m_isModule = false, m_isAttach = false;
        void* m_target = nullptr;
        void* m_detour = nullptr;

        T m_original = nullptr;
    };

    //Window
    extern HWND MyCreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
    extern BOOL MyDestroyWindow(HWND);

    //Paint
    extern HDC MyBeginPaint(HWND, LPPAINTSTRUCT);
    extern BOOL MyEndPaint(HWND, const PAINTSTRUCT*);

    //GDI Draw
    extern int MyFillRect(HDC, const RECT*, HBRUSH);
    extern BOOL MyPatBlt(HDC, int, int, int, int, DWORD);
    extern int MyDrawTextW(HDC, LPCWSTR, int, LPRECT, UINT);
    extern int MyDrawTextExW(HDC, LPWSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS);
    extern BOOL MyExtTextOutW(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);

    //GDI Fun
    extern HDC MyCreateCompatibleDC(HDC);

    //Visual Theme
    extern HRESULT MyGetThemeColor(HTHEME, int, int, int, COLORREF*);
    extern HRESULT MyDrawThemeText(HTHEME, HDC, int, int, LPCTSTR, int, DWORD, DWORD, LPCRECT);
    extern HRESULT MyDrawThemeTextEx(HTHEME, HDC, int, int, LPCTSTR, int, DWORD, LPCRECT, const DTTOPTS*);
    extern HRESULT MyDrawThemeBackgroundEx(HTHEME, HDC, int, int, LPCRECT, const DTBGOPTS*);

    template<typename T>
    inline auto HookDef(T tar, void* det)
    {
        return HookFun<T>(tar, det);
    }
    template<typename T>
    inline auto HookDef(LPCWSTR mname, LPCSTR fname, T det)
    {
        return HookFun<T>(mname, fname, det);
    }

    extern void HookAttachAll();
    extern void HookDetachAll();
}
