/*
* Hook函数集
*
* Author: Maple
* date: 2022-7-3 Create
* Copyright winmoes.com
*/
#include "HookDef.h"
#include "ShellLoader.h"
#include "ComHookDef.h"
#include <unordered_map>
#include <functional>
#include <vssym32.h>
#include <dwmapi.h>
#include <mutex>
#include <iostream>
#include <propvarutil.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Comctl32.lib")    //Win32 Controls

struct DUIData
{
    HWND mainWnd = 0;   //Explorer Window

    struct treeViewData
    {
        HDC hDC = nullptr;
        HWND hWnd = nullptr;
    };

    struct itemViewData
    {
        HDC hDC = nullptr;
        HWND lineWnd = nullptr;
        treeViewData treeData;
    };

    SIZE size = { 0,0 };
    bool refresh = true;
	bool treeDraw = false;
    //std::wstring path;

    //DirectUIHWND
    std::unordered_map<HWND, itemViewData> duiList;
};
struct Config
{
    int effType = 0;        //效果类型 0=Blur 1=Acrylic 2=Mica
    COLORREF blendColor = 0;//混合颜色
    bool smallborder = false;
    bool showLine = false;
    bool darkRibbon = true;
    bool clearAddress = false;
    bool clearBarBg = false;
};

//线程安全的hashmap
template<typename T, typename T1>
class hashMap
{
public:
    hashMap() = default;

    auto find(T _name)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_map.find(_name);
    }

    template <typename Args>
    auto erase(Args&& _it)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_map.erase(_it);
    }

    template <typename Args>
    auto insert(Args&& _value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_map.insert(_value);
    }

    auto& operator[](const T& _value)
    {
        return m_map[_value];
    }

    auto clear()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_map.clear();
    }

    auto end()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_map.end();
    }

    auto size()
    {
        return m_map.size();
    }

private:
    std::mutex m_mutex;
    std::unordered_map<T, T1> m_map;
};

hashMap<DWORD, DUIData> m_DUIList;                  //dui句柄列表
hashMap<DWORD, std::pair<HWND, HDC>> m_ribbonPaint; //用来记录win10 Ribbon区域是否绘制
hashMap<DWORD, bool> m_drawtextState;               //用来记录drawText Alpha修复状态
HBRUSH m_clearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);  //用于清空DC的画刷
Config m_config;                                    //配置文件

COLORREF GetColorCfg(bool light)
{
    std::wstring path = GetCurDllDir() + L"\\config.ini";
    auto color = [&path, &light](std::wstring s) -> BYTE
    {
        int c = _wtoi(GetIniString(path, light ? L"light" : L"dark", s).c_str());
        if (c > 255) c = 255;
        else if (c < 0) c = 0;

        return BYTE(c);
    };
    //纯黑或者纯白alpha会失效 限制最小或最大值
    auto c = M_RGBA(color(L"r"), color(L"g"), color(L"b"), color(L"a"));
    if (GetRValue(c) == 255 && GetGValue(c) == 255 && GetBValue(c) == 255)
        c = M_RGBA(249, 249, 249, M_GetAValue(c));
    else if (GetRValue(c) == 0 && GetGValue(c) == 0 && GetBValue(c) == 0)
        c = M_RGBA(40, 40, 40, M_GetAValue(c));
    return c;
}

void RefreshConfig()
{
    std::wstring path = GetCurDllDir() + L"\\config.ini";
    auto cfg = GetIniString(path, L"config", L"effect");

    //效果类型
    m_config.effType = _wtoi(cfg.c_str());
    if (m_config.effType < 0 || m_config.effType > 3)
        m_config.effType = 0;

    //22000以后不支持Blur效果
    if (m_config.effType == 0 && g_sysBuildNumber > 22000)
        m_config.effType = 1;

    m_config.blendColor = GetColorCfg(SysIsLightMode());
    m_config.smallborder = GetIniString(path, L"config", L"smallBorder") == L"true" ? true : false;
    m_config.showLine = GetIniString(path, L"config", L"showLine") == L"true" ? true : false;
    m_config.darkRibbon = GetIniString(path, L"config", L"darkRibbon") == L"false" ? false : true;
    m_config.clearAddress = GetIniString(path, L"config", L"clearAddress") == L"true" ? true : false;
    m_config.clearBarBg = GetIniString(path, L"config", L"clearBarBg") == L"true" ? true : false;

    RefreshWin10BlurFrame(m_config.smallborder);
}

void OnDocComplete(std::wstring path, DWORD threadID)
{
    auto iter = m_DUIList.find(threadID);
    if (iter != m_DUIList.end())
    {
        iter->second.treeDraw = true;
        //iter->second.path = std::move(path);
       /* if(path.find(L"::{26EE0668-A00A-44D7-9371-BEB064C98683}") != std::wstring::npos)
        {
            StartAero(iter->second.mainWnd, -1);
        }*/
    }
    //std::wcout << L"path[" << threadID << L"] " << path << L"\n";
}

namespace Hook
{
    int hookIndex = 0;

    auto _CreateWindowExW_ = HookDef(CreateWindowExW, MyCreateWindowExW);
    auto _DestroyWindow_ = HookDef(DestroyWindow, MyDestroyWindow);
    auto _BeginPaint_ = HookDef(BeginPaint, MyBeginPaint);
    auto _EndPaint_ = HookDef(EndPaint, MyEndPaint);
    auto _FillRect_ = HookDef(FillRect, MyFillRect);
    auto _DrawTextW_ = HookDef(DrawTextW, MyDrawTextW);
    auto _DrawTextExW_ = HookDef(DrawTextExW, MyDrawTextExW);
    auto _ExtTextOutW_ = HookDef(ExtTextOutW, MyExtTextOutW);
    auto _CreateCompatibleDC_ = HookDef(CreateCompatibleDC, MyCreateCompatibleDC);
    auto _GetThemeColor_ = HookDef(GetThemeColor, MyGetThemeColor);
    auto _DrawThemeText_ = HookDef(DrawThemeText, MyDrawThemeText);
    auto _DrawThemeTextEx_ = HookDef(DrawThemeTextEx, MyDrawThemeTextEx);
    auto _DrawThemeBackground_ = HookDef(DrawThemeBackground, MyDrawThemeBackground);
    auto _DrawThemeBackgroundEx_ = HookDef(DrawThemeBackgroundEx, MyDrawThemeBackgroundEx);
    auto _PatBlt_ = HookDef(PatBlt, MyPatBlt);
    auto _CoCreateInstance_ = HookDef(CoCreateInstance, MyCoCreateInstance);

    HRESULT(WINAPI* IPropertyStore_SetValue_ORG)(void*, const PROPERTYKEY&, const PROPVARIANT&) = nullptr;

    void HookAttachAll()
    {
        _CreateWindowExW_.Attach();
        _DestroyWindow_.Attach();
        _BeginPaint_.Attach();
        _EndPaint_.Attach();
        _FillRect_.Attach();
        _DrawTextW_.Attach();
        _DrawTextExW_.Attach();
        _ExtTextOutW_.Attach();
        _CreateCompatibleDC_.Attach();
        _GetThemeColor_.Attach();
        _DrawThemeText_.Attach();
        _DrawThemeTextEx_.Attach();

        //修复win11暗黑模式下地址栏不透明
        //if (g_sysBuildNumber >= 22000)
        _DrawThemeBackground_.Attach();

        _DrawThemeBackgroundEx_.Attach();
        _PatBlt_.Attach();
        _CoCreateInstance_.Attach();

#ifdef _DEBUG
        AllocConsole();
        FILE* pOut = NULL;
        freopen_s(&pOut, "conout$", "w", stdout);
        freopen_s(&pOut, "conout$", "w", stderr);
        std::wcout.imbue(std::locale("chs"));
#endif // _DEBUG

    }

    void HookDetachAll()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }

    //检查排除路径和线程
    bool IsExcludePath()
    {
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter == m_DUIList.end()) return true;
        /*if (iter->second.path.find(L"::{26EE0668-A00A-44D7-9371-BEB064C98683}") != std::wstring::npos)
        {
            return false;
        }*/
        if (iter->second.mainWnd == nullptr)
        {
            return true;
        }
        return false;
    }

    //刷新22H2的窗口标题栏区域
    void OnWindowSize(HWND hWnd, int newHeight)
    {
        MARGINS margin = { -1 };
        if ((M_GetAValue(m_config.blendColor) != 0 && m_config.effType == 1) || m_config.effType == 3)
        {
            margin.cyTopHeight = GetSystemMetricsForDpi(SM_CYCAPTION, GetDpiForWindow(hWnd)) + 10;
        }
        else
        {
            margin = { 0 };
            margin.cyTopHeight = newHeight;
        }
        DwmExtendFrameIntoClientArea(hWnd, &margin);
        DwmFlush();
    }

    //设置窗口效果
    void SetWindowBlur(HWND hWnd)
    {
        bool isBlend = M_GetAValue(m_config.blendColor) != 0 && m_config.effType != 3;
        if (g_sysBuildNumber >= 22000)
        {
            if (g_sysBuildNumber >= 22500)
            {
                RECT pRect;
                GetWindowRect(hWnd, &pRect);
                OnWindowSize(hWnd, pRect.bottom - pRect.top);
                if (m_config.effType == 1 || m_config.effType == 3)
                {
                    //禁用winrt私有云母合成效果
                    int type = 0;
                    DwmSetWindowAttribute(hWnd, 1029, &type, sizeof(type));

                    //22621.655
                    DWM_BLURBEHIND blur = { 0 };
                    blur.dwFlags = 1;
                    blur.fEnable = 1;
                    DwmEnableBlurBehindWindow(hWnd, &blur);

                    if (isBlend || m_config.effType == 3)
                        StartAero(hWnd, m_config.effType, m_config.blendColor, isBlend);
                    else
                    {
                        DWM_SYSTEMBACKDROP_TYPE type1 = DWMSBT_TRANSIENTWINDOW;
                        DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &type1, sizeof(type1));
                    }
                }
                else
                {
                    //启用私有云母效果
                    int type = 1;
                    DwmSetWindowAttribute(hWnd, 1029, &type, sizeof(type));
                }
                return;
            }

            switch (m_config.effType)
            {
            case 0:
            {
                int type = 0;
                DwmSetWindowAttribute(hWnd, 1029, &type, sizeof(type));

                StartAero(hWnd, 0, m_config.blendColor, isBlend);
                break;
            }
            case 1:
            {
                //取消标题栏的Mica效果
                int type = 0;
                HRESULT hr = DwmSetWindowAttribute(hWnd, 1029, &type, sizeof(type));

                StartAero(hWnd, 1, m_config.blendColor, isBlend);

                //设置标题栏颜色
                COLORREF color = m_config.blendColor;
                DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
                break;
            }
            default:
                StartAero(hWnd, m_config.effType, 0, false);
                break;
            }
        }
        else
            StartAero(hWnd, m_config.effType, m_config.blendColor, isBlend);
    }

    //窗口子类化
    LRESULT WINAPI MyWndSubProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        if (dwRefData == 0)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                int height = HIWORD(lparam);
                OnWindowSize(hWnd, height);
            }
            break;

            //case WM_NCCALCSIZE:
            //{
            //    /*if (!lparam)
            //        return DefSubclassProc(hWnd, message, wparam, lparam);

            //    UINT dpi = GetDpiForWindow(hWnd);

            //    int frameX = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
            //    int frameY = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
            //    int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

            //    NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lparam;

            //    params->rgrc->right -= frameX + padding;
            //    params->rgrc->left += frameX + padding;
            //    params->rgrc->bottom -= frameY + padding;*/

            //    return 0;
            //}
            //22H2 22621.655 多标签页失去和获取焦点需要刷新
            case WM_ACTIVATE:
            {
                auto ret = DefSubclassProc(hWnd, message, wparam, lparam);
                RECT pRect;
                GetWindowRect(hWnd, &pRect);
                OnWindowSize(hWnd, pRect.bottom - pRect.top);
                return ret;
            }
            case WM_SYSCOMMAND:
            {
                if (wparam == SC_MAXIMIZE || wparam == SC_RESTORE)
                {
                    auto ret = DefSubclassProc(hWnd, message, wparam, lparam);
                    RECT pRect;
                    GetWindowRect(hWnd, &pRect);
                    OnWindowSize(hWnd, pRect.bottom - pRect.top);
                    return ret;
                }
            }
            break;
            }
        }
        else if (dwRefData == 1)
        {
            if (message == WM_WINDOWPOSCHANGED)
            {
                auto ret = DefSubclassProc(hWnd, message, wparam, lparam);

                LPWINDOWPOS pos = (LPWINDOWPOS)lparam;

                RECT rc = { -1 };
                if (!IsZoomed(hWnd))
                {
                    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc));
                    OffsetRect(&rc, -rc.left, -rc.top);

                    int width = pos->cx - (rc.right - rc.left);
                    width /= 2;

                    rc.left += width;
                    rc.right += width;
                }
                DwmUpdateAccentBlurRect(hWnd, &rc);

                return ret;
            }
        }
        /*else if(dwRefData == 2 && message == WM_PAINT)
        {
            auto ret = DefSubclassProc(hWnd, message, wparam, lparam);
            auto iter = m_DUIList.find(GetCurrentThreadId());
            if (iter != m_DUIList.end())
            {
                PAINTSTRUCT ps;
                HDC hDC = _BeginPaint_.Org(hWnd, &ps);
                RECT rc;

                GetClientRect(hWnd, &rc);
                rc.right += 20;
                FillRect(hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                _EndPaint_.Org(hWnd, &ps);

            }
            return ret;
        }*/
        if (message == WM_SETTINGCHANGE)
        {
            if (lparam && CompareStringOrdinal((LPCWCH)lparam, -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
            {
                m_config.blendColor = GetColorCfg(SysIsLightMode());
                SetWindowBlur(hWnd);
            }
        }
        else if(message == WM_DPICHANGED)
        {
            auto ret = DefSubclassProc(hWnd, message, wparam, lparam);
            auto iter = m_DUIList.find(GetCurrentThreadId());
            if (iter != m_DUIList.end())
            {
                iter->second.treeDraw = true;
            }

            RECT pRect;
            GetWindowRect(hWnd, &pRect);
            OnWindowSize(hWnd, pRect.bottom - pRect.top);
            return ret;
        }
        return DefSubclassProc(hWnd, message, wparam, lparam);
    }

    HWND MyCreateWindowExW(
        DWORD     dwExStyle,
        LPCWSTR   lpClassName,
        LPCWSTR   lpWindowName,
        DWORD     dwStyle,
        int       X,
        int       Y,
        int       nWidth,
        int       nHeight,
        HWND      hWndParent,
        HMENU     hMenu,
        HINSTANCE hInstance,
        LPVOID    lpParam
    )
    {

        HWND hWnd = _CreateWindowExW_.Org(dwExStyle, lpClassName, lpWindowName, dwStyle,
            X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

        std::wstring ClassName;
        if (hWnd)
        {
            ClassName = GetWindowClassName(hWnd);
        }

        //修复Blur下Edit的Alpha
        if (!IsExcludePath()) 
        {
            if (ConvertTolower(ClassName) == L"edit")
            {
                SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
        }

        /*if(ClassName == L"UIRibbonCommandBarDock")
        {
			SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            InvalidateRect(hWnd, nullptr, TRUE);
            std::wcout << L"UIRibbonCommandBarDock\n";
        }*/

        thread_local HWND lastDUI = nullptr;
        //explorer window 
        if (ClassName == L"DirectUIHWND" && GetWindowClassName(hWndParent) == L"SHELLDLL_DefView")
        {
            //继续查找父级
            HWND parent = GetParent(hWndParent);
            if (GetWindowClassName(parent) == L"ShellTabWindowClass")
            {
                DWORD tid = GetCurrentThreadId();
                auto& node = m_DUIList[tid].duiList[hWnd];
                node.hDC = nullptr;
                if(!node.treeData.hWnd)
                {
                    auto enumChild = [](HWND hWnd, std::vector<HWND>& list)
                    {
                        list.clear();
                        auto enumproc = [](HWND hWnd, LPARAM lParam)
                        {
                            auto list = (std::vector<HWND>*)lParam;
                            list->push_back(hWnd);
                            return TRUE;
                        };
                        EnumChildWindows(hWnd, enumproc, (LPARAM)&list);
                        if (list.empty())
                            return false;
                        return true;
                    };

                    auto FindChildWindow = [&enumChild](HWND hWnd, auto name)
                    {
                        if (!hWnd)
                            return (HWND)nullptr;
                        std::vector<HWND> childList;
                        enumChild(hWnd, childList);
                        for(auto& h : childList)
                        {
                            if (GetWindowClassName(h) == name)
                                return h;
                        }
                        return (HWND)nullptr;
                    };

                    //从文件资源管理器窗口枚举TabWindow
                    HWND duiHwnd = FindChildWindow(m_DUIList[tid].mainWnd, L"ShellTabWindowClass");
                    //从DUIViewWndClassName枚举DirectUIHWND
                    duiHwnd = FindChildWindow(duiHwnd, L"DUIViewWndClassName");
                    duiHwnd = FindChildWindow(duiHwnd, L"DirectUIHWND");
                    if(duiHwnd)
                    {
                        node.lineWnd = duiHwnd;
                        //查找TreeView
                        std::vector<HWND> childList;
                        enumChild(hWnd, childList);
                        for (auto& h : childList)
                        {
                            if (GetWindowClassName(h) == L"CtrlNotifySink")
                            {
                                HWND tree = FindChildWindow(h, L"NamespaceTreeControl");
                                if (tree)
                                    break;
                            }
                        }
                        duiHwnd = FindChildWindow(duiHwnd, L"SysTreeView32");
                        if(duiHwnd)
                        {
                            node.treeData.hWnd = duiHwnd;
                            m_DUIList[tid].treeDraw = true;
                        }
                    }
                }
                lastDUI = hWnd;
            }
        }
        //查找主窗口
        else if (ClassName == L"SHELLDLL_DefView" && GetWindowClassName(hWndParent) == L"ShellTabWindowClass")
        {
            HWND mainWnd = GetParent(hWndParent);
            //设置Blur
            SetWindowBlur(mainWnd);

            //22H2
            if (g_sysBuildNumber >= 22500)
            {
                SetWindowSubclass(mainWnd, MyWndSubProc, 0, (DWORD_PTR)0);
            }
            //win 10
            else if (g_sysBuildNumber < 22000)
            {
                SetWindowSubclass(mainWnd, MyWndSubProc, 0, (DWORD_PTR)1);
            }
            else
                SetWindowSubclass(mainWnd, MyWndSubProc, 0, (DWORD_PTR)3);

            //记录到列表中 Add to list
            DWORD tid = GetCurrentThreadId();
            m_DUIList[tid].mainWnd = mainWnd;
        }
        /*else if (ClassName == L"DirectUIHWND" && GetWindowClassName(hWndParent) == L"DUIViewWndClassName")
        {
            SetWindowSubclass(hWnd, MyWndSubProc, 0, (DWORD_PTR)2);
        }*/
        //导航树视图
        else if (ClassName == L"SysTreeView32" && lastDUI)
        {
            HWND parent = GetParent(hWndParent);//NamespaceTreeControl
            parent = GetParent(parent);         //CtrlNotifySink

            DWORD tid = GetCurrentThreadId();

            if (!m_config.showLine)
            {
                m_DUIList[tid].duiList[lastDUI].lineWnd = parent;
            }
            parent = GetParent(parent);         //DirectUIHWND
            parent = GetParent(parent);         //DUIViewWndClassName
            std::wstring mainWndCls = GetWindowClassName(parent);

            if (mainWndCls == L"ShellTabWindowClass")
                m_DUIList[tid].duiList[lastDUI].treeData.hWnd = hWnd;

            lastDUI = nullptr;
        }
        return hWnd;
    }

    BOOL MyDestroyWindow(HWND hWnd)
    {
        //查找并删除列表中的记录
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end())
        {
            auto _iter = iter->second.duiList.find(hWnd);
            if(_iter != iter->second.duiList.end())
            {
                iter->second.duiList.erase(_iter);
                //std::wcout << L"free:" << iter->second.duiList.size() << L"," << m_DUIList.size() << L"\n";
            }
            //释放窗口列表node
            if (iter->second.mainWnd == hWnd)
                m_DUIList.erase(iter);
        }
        return _DestroyWindow_.Org(hWnd);
    }

    HDC MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
    {
        const HDC hDC = _BeginPaint_.Org(hWnd, lpPaint);
        const auto iter = m_DUIList.find(GetCurrentThreadId());

        if (iter == m_DUIList.end()) return hDC;

        for (auto& _iter : iter->second.duiList)
        {
            //DUI视图开始绘制
            if (_iter.first == hWnd)
            {
                _iter.second.hDC = hDC;
                break;
            }
            //TreeView开始绘制
            if (_iter.second.treeData.hWnd == hWnd)
            {
                _iter.second.treeData.hDC = hDC;
                break;
            }
        }
        return hDC;
    }

    BOOL MyEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint)
    {
        DWORD curThread = GetCurrentThreadId();
        auto iter = m_DUIList.find(curThread);

        if (iter != m_DUIList.end()) 
        {
            //DUI
            for (auto& _iter : iter->second.duiList)
            {
                if (_iter.first == hWnd)
                {
                    _iter.second.hDC = nullptr;
                    break;
                }
                if(_iter.second.treeData.hWnd == hWnd)
                {
	                _iter.second.treeData.hDC = nullptr;
                    iter->second.treeDraw = false;
	                break;
                }
                //清除分隔线
                if(!m_config.showLine && _iter.second.lineWnd == hWnd)
                {
	                RECT rcWind, rcTree, rcDst;
	                GetWindowRect(hWnd, &rcWind);
	                GetWindowRect(_iter.second.treeData.hWnd, &rcTree);
	                rcDst.left = rcTree.left - rcWind.left;
	                rcDst.top = rcTree.top - rcWind.top;
	                rcDst.right = rcDst.left + (rcTree.right - rcTree.left) + 20;
	                rcDst.bottom = rcDst.top + (rcTree.bottom - rcTree.top);
	                FillRect(lpPaint->hdc, &rcDst, m_clearBrush);

	                if (_iter.first)
	                {
		                GetWindowRect(_iter.first, &rcTree);
		                rcDst.left = rcTree.left - rcWind.left + (rcTree.right - rcTree.left);
		                rcDst.right = rcDst.left + 10;
		                FillRect(lpPaint->hdc, &rcDst, m_clearBrush);
	                }
                    break;
                }
            }
        }

        auto ribiter = m_ribbonPaint.find(curThread);
        if (ribiter != m_ribbonPaint.end())
            m_ribbonPaint.erase(ribiter);

        BOOL ret = _EndPaint_.Org(hWnd, lpPaint);
        return ret;
    }

    int MyFillRect(HDC hDC, const RECT* lprc, HBRUSH hbr)
    {
	    DWORD curThread = GetCurrentThreadId();

        auto iter = m_DUIList.find(curThread);
        if (iter != m_DUIList.end()) 
        {
            for (auto& _iter : iter->second.duiList)
            {
                if (_iter.second.hDC != hDC && _iter.second.treeData.hDC != hDC)
                    continue;

                int ret = _FillRect_.Org(hDC, lprc, hbr);

                //刷新颜色值
                if (iter->second.refresh)
                {
                    SendMessageW(_iter.first, WM_THEMECHANGED, 0, 0);

                    //win11 22621.655 多标签页 第一次打开页面需要刷新
                    if (g_sysBuildNumber >= 22600)
                    {
                        SetWindowBlur(iter->second.mainWnd);
                    }
                    InvalidateRect(_iter.first, nullptr, TRUE);
                    iter->second.refresh = false;
                }
                //判断树列表颜色是否被改变
                //if (iter->second.treeDraw)
                //{
                    COLORREF color = RGB(0, 0, 0);
                    if (TreeView_GetBkColor(_iter.second.treeData.hWnd) != color)
                    {
                        TreeView_SetBkColor(_iter.second.treeData.hWnd, color);
                    }
                //}
                return ret;
            }
        }
        //透明化 Windows 10 Ribbon 在Light模式
        auto ribiter = m_ribbonPaint.find(curThread);
        if (ribiter != m_ribbonPaint.end())
        {
            if (ribiter->second.second == hDC)
            {
                RECT rcWnd;
                GetWindowRect(ribiter->second.first, &rcWnd);

                if (lprc->bottom == CalcRibbonHeightForDPI(ribiter->second.first, 26, false)
                    || lprc->bottom == CalcRibbonHeightForDPI(ribiter->second.first, 23, false, false)
                    || lprc->bottom == 1
                    || (lprc->bottom - lprc->top == CalcRibbonHeightForDPI(ribiter->second.first, 1)
                        && (rcWnd.right - rcWnd.left) == lprc->right))
                {
                    hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
                }
            }
        }
        return _FillRect_.Org(hDC, lprc, hbr);
    }

    BOOL MyPatBlt(HDC hdc, int x, int y, int w, int h, DWORD rop)
    {
        //修复选择矩形框的Alpha
        if (!IsExcludePath() && rop == PATCOPY)
        {
            //检测调用线程
            static std::unordered_map<DWORD, bool> thList;

            DWORD curThread = GetCurrentThreadId();
            if (thList.find(curThread) != thList.end())
                return _PatBlt_.Org(hdc, x, y, w, h, rop);

            thList.insert(std::make_pair(curThread, true));
            //获取DC颜色画刷
            HBRUSH hbr = (HBRUSH)SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
            LOGBRUSH lbr;
            GetObjectW(hbr, sizeof(lbr), &lbr);

            //设置GDIP画刷颜色
            Gdiplus::SolidBrush brush(Gdiplus::Color::Transparent);
            brush.SetColor(Gdiplus::Color::MakeARGB(200, GetRValue(lbr.lbColor),
                GetGValue(lbr.lbColor), GetBValue(lbr.lbColor)));

            //使用GDIP缓冲区绘制
            Gdiplus::Graphics gp(hdc);
            gp.FillRectangle(&brush, Gdiplus::Rect(x, y, w, h));

            //设置回原画刷
            SelectObject(hdc, hbr);

            thList.erase(curThread);
            return TRUE;
        }
        return _PatBlt_.Org(hdc, x, y, w, h, rop);
    }

    bool AlphaBuffer(HDC hdc, LPCRECT pRc, std::function<void(HDC)> fun)
    {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        BP_PAINTPARAMS bpParam;
        bpParam.cbSize = sizeof(BP_PAINTPARAMS);
        bpParam.dwFlags = BPPF_ERASE;
        bpParam.prcExclude = nullptr;
        bpParam.pBlendFunction = &bf;
        HDC hDC = 0;
        HPAINTBUFFER pbuffer = BeginBufferedPaint(hdc, pRc, BPBF_TOPDOWNDIB, &bpParam, &hDC);
        if (pbuffer && hDC && fun)
        {
            //设置原DC信息
            SelectObject(hDC, GetCurrentObject(hdc, OBJ_FONT));
            SetBkMode(hDC, GetBkMode(hdc));
            SetBkColor(hDC, GetBkColor(hdc));
            SetTextAlign(hDC, GetTextAlign(hdc));
            SetTextCharacterExtra(hDC, GetTextCharacterExtra(hdc));
            //SetTextColor(hDC, GetTextColor(hdc));

            fun(hDC);

            EndBufferedPaint(pbuffer, TRUE);

            return true;
        }
        return false;
    }

    //修复DrawTextW的Alpha
    int MyDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
    {
        if (format & DT_CALCRECT || m_drawtextState.find(GetCurrentThreadId()) != m_drawtextState.end())
            return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);

        HRESULT hr = S_OK;

        auto fun = [&](HDC hDC)
        {
            hr = DrawTextWithGlow(hDC, lpchText, cchText, lprc, format,
                GetTextColor(hdc), 0, 0, 0, 0,
                [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
                {
                    return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
                },
                0);

        };
        if (!AlphaBuffer(hdc, lprc, fun))
        {
            hr = _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
        }

        return hr;
    }

    //修复DrawTextExW的Alpha
    int MyDrawTextExW(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp)
    {
        thread_local bool isCurThread = false;

        //TreeView绘制的时候最后一个参数是NULL 因此可以直接用DrawText
        if (!lpdtp && !(format & DT_CALCRECT) && !isCurThread
            && m_drawtextState.find(GetCurrentThreadId()) == m_drawtextState.end()) {

            isCurThread = true;
            auto ret = MyDrawTextW(hdc, lpchText, cchText, lprc, format);
            isCurThread = false;
            return ret;
        }
        return _DrawTextExW_.Org(hdc, lpchText, cchText, lprc, format, lpdtp);
    }

    //修复ExtTextOutW的Alpha
    BOOL MyExtTextOutW(HDC hdc, int x, int y, UINT option, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
    {
        std::wstring str;
        if (lpString) str = lpString;

        thread_local bool isCurThread = false;

        if (!IsExcludePath() && !(CheckCaller(L"msctf.dll")) && !(option & ETO_GLYPH_INDEX) && !(option & ETO_IGNORELANGUAGE)
            && !isCurThread && !str.empty() && m_drawtextState.find(GetCurrentThreadId()) == m_drawtextState.end())
        {
            isCurThread = true;

            RECT rect = { 0 };
            if ((option & ETO_OPAQUE || option & ETO_CLIPPED) && lprect)
                rect = *lprect;
            else
                _DrawTextW_.Org(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);

            if (!lpDx) {
                MyDrawTextW(hdc, lpString, c, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
            }
            else
            {
                RECT rc = { x, y, x + (rect.right - rect.left), y + (rect.bottom - rect.top) };

                if (option & ETO_CLIPPED)
                {
                    SaveDC(hdc);
                    IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
                }

                COLORREF crText = GetTextColor(hdc);
                auto callback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
                {
                    return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
                };

                HRESULT hr = S_OK;
                //合批绘制文本
                auto fun = [&](HDC hDC)
            	{
                    std::wstring batchStr;
                    bool batch = true;

                    batchStr += lpString[0];

                    int srcExtra = GetTextCharacterExtra(hdc);
                    SetTextCharacterExtra(hDC, lpDx[0]);

                    RECT batchRc = rc;
                    for (int i = 0; i < c; i++)
                    {
                        if (i != 0) 
                        {
                            if (lpDx[i] == lpDx[i - 1]) 
                            {
                                if (!batch)
                                {
                                    batch = true;
                                    SetTextCharacterExtra(hDC, lpDx[i]);
                                }
                                batchStr += lpString[i];
                            }
                            else
                            {
                                //先绘制上一批
                                hr = DrawTextWithGlow(hDC, batchStr.c_str(), (int)batchStr.length(), &batchRc, DT_LEFT | DT_TOP | DT_SINGLELINE,
                                    crText, 0, 0, 0, 0, callback, 0);
                                //hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);

                                batch = false;
                                batchStr = lpString[i];
                                SetTextCharacterExtra(hDC, lpDx[i]);
                                batchRc.left = rc.left;
                            }
                        }

                        //最后一批
                        if (i == c - 1)
                        {
                            hr = DrawTextWithGlow(hDC, batchStr.c_str(), (int)batchStr.length(), &batchRc, DT_LEFT | DT_TOP | DT_SINGLELINE,
                                crText, 0, 0, 0, 0, callback, 0);
                            //hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);
                        }

                        rc.left += lpDx[i];
                    }
                    SetTextCharacterExtra(hDC, srcExtra);
                };
                //fun(hdc);
                if (!AlphaBuffer(hdc, &rc, fun))
                {
                    hr = _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
                }

                if (option & ETO_CLIPPED)
                    RestoreDC(hdc, -1);
            }

            isCurThread = false;
            return 1;
        }
        return _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
    }

    HDC MyCreateCompatibleDC(HDC hDC)
    {
        //在绘制DUI之前 会调用CreateCompatibleDC 找到它
        HDC retDC = _CreateCompatibleDC_.Org(hDC);
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end()) 
        {
            HWND sHwnd = WindowFromDC(hDC);
            for(auto& _iter : iter->second.duiList)
            {
	            if(sHwnd == _iter.first)
	            {
                    _iter.second.hDC = retDC;
                    return retDC;
	            }
            	if(sHwnd == _iter.second.treeData.hWnd)
                {
                    _iter.second.treeData.hDC = retDC;
                    return retDC;
                }
            }
        }
        //Win10 Ribbon
        if (g_sysBuildNumber < 22000) 
        {
            HWND wnd = WindowFromDC(hDC);
            if (wnd && GetWindowClassName(wnd) == L"NetUIHWND")
                m_ribbonPaint[GetCurrentThreadId()] = std::make_pair(wnd, retDC);
        }
        return retDC;
    }

    HRESULT MyGetThemeColor(
        HTHEME   hTheme,
        int      iPartId,
        int      iStateId,
        int      iPropId,
        COLORREF* pColor
    )
    {
        HRESULT ret = _GetThemeColor_.Org(hTheme, iPartId, iStateId, iPropId, pColor);

        std::wstring kname = GetThemeClassName(hTheme);

        //将主要控件的背景色设置为黑色 以供API透明化Blur效果
        if (iPropId == TMT_FILLCOLOR && !IsExcludePath())
        {
            //std::wcout << kname << L" p:" << iPartId << L" s:" << iStateId << std::endl;
            //DUI视图、底部状态栏、导航栏
            if (((kname == L"ItemsView" || kname == L"ExplorerStatusBar" || kname == L"ExplorerNavPane")
                && (iPartId == 0 && iStateId == 0))
                //ReadingPane是预览面板的背景色
                || (kname == L"ReadingPane" && iPartId == 1 && iStateId == 0)//FILL Color
                || (kname == L"ProperTree" && iPartId == 2 && iStateId == 0) //DirectUIHWND 树属性视图的背景色
                )
            {
                *pColor = RGB(0, 0, 0);
            }
        }
        return ret;
    }

    //转换为DrawThemeTextEx调用
    HRESULT MyDrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
        int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
    {
        DTTOPTS Options = { sizeof(DTTOPTS) };
        RECT Rect = *pRect;

        _GetThemeColor_.Org(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &Options.crText);
        HRESULT ret = MyDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, &Rect, &Options);

        return ret;
    }

    //防止DrawText类API内部递归调用 并进行Alpha修复
    HRESULT MyDrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
        int cchText, DWORD dwTextFlags, LPCRECT pRect, const DTTOPTS* pOptions)
    {
        HRESULT ret = S_OK;

        if (pOptions && !(pOptions->dwFlags & DTT_CALCRECT) && !(pOptions->dwFlags & DTT_COMPOSITED))
        {
            auto fun = [&](HDC hDC)
            {
                auto tid = GetCurrentThreadId();
                m_drawtextState.insert(std::make_pair(tid, true));

                COLORREF color = pOptions->crText;

                if (!(dwTextFlags & DTT_TEXTCOLOR))
                    _GetThemeColor_.Org(hTheme, iPartId, iStateId, TMT_TEXTCOLOR, &color);

                ret = DrawTextWithGlow(hDC, pszText, cchText, pRect, dwTextFlags,
                    color, 0, 0, 0, 0, pOptions->pfnDrawTextCallback, pOptions->lParam);
                //ret = _DrawThemeTextEx_.Org(hTheme, hDC, iPartId, iStateId, pszText, cchText, dwTextFlags,
                //    (LPRECT)pRect, pOptions);
                m_drawtextState.erase(tid);
            };

            if (!AlphaBuffer(hdc, pRect, fun))
                goto Org;

            return ret;
        }
    Org:
        ret = _DrawThemeTextEx_.Org(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
        return ret;
    }

    bool PaintScroll(HDC hdc, int iPartId, int iStateId, LPCRECT pRect)
    {
        if ((iPartId == SBP_UPPERTRACKVERT || iPartId == SBP_LOWERTRACKVERT
            || iPartId == SBP_UPPERTRACKHORZ || iPartId == SBP_LOWERTRACKHORZ
            || iPartId == SBP_ARROWBTN)
            && (iStateId == 1 || iStateId == 2 || iStateId == 3 || iStateId == 5 || iStateId == 9 || iStateId == 13))
        {
            return true;
        }
        if (iPartId == SBP_THUMBBTNVERT || iPartId == SBP_THUMBBTNHORZ)
        {
            Gdiplus::Graphics draw(hdc);
            draw.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            UINT dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            float roundValue = ((float)dpiY / 96.f) * (iStateId == 1 ? 3.f : 4.f);

            Gdiplus::SolidBrush brush(Gdiplus::Color(135, 135, 135));

            int x = pRect->left;
            int y = pRect->top;
            int width = pRect->right - pRect->left;
            int height = pRect->bottom - pRect->top;

            if (iPartId == SBP_THUMBBTNVERT)
            {
                int newWidth = int((float)width / (iStateId == 1 ? 2.4f : 1.8f));
                x += (width - newWidth) / 2;
                width = newWidth;
            }
            else
            {
                int newHeight = int((float)height / (iStateId == 1 ? 2.4f : 1.8f));
                y += (height - newHeight) / 2;
                height = newHeight;
            }

            Gdiplus::Rect rc(x, y, width, height);

            RoundRectPath path(rc, roundValue);
            Gdiplus::Rect rc1 = rc;
            rc1.Width += (INT)roundValue;
            RoundRectPath pathClip(rc1, roundValue);
            draw.SetClip(&pathClip);
            draw.FillPath(&brush, &path);
            draw.ResetClip();
            return true;
        }
    }

    HRESULT MyDrawThemeBackground(HTHEME  hTheme,
        HDC     hdc,
        int     iPartId,
        int     iStateId,
        LPCRECT pRect,
        LPCRECT pClipRect)
    {
        if (!IsExcludePath())
        {
            std::wstring kname = GetThemeClassName(hTheme);

            //td::wcout << kname << L" p:" << iPartId << L" s:" << iStateId << std::endl;

            if (kname == L"Rebar" && (iPartId == RP_BACKGROUND || iPartId == RP_BAND) && iStateId == 0)
            {
                _FillRect_.Org(hdc, pRect, m_clearBrush);
                return S_OK;
            }
            if (m_config.clearAddress && (kname == L"AddressBand" || kname == L"SearchBox")
                && iPartId == 1 && (iStateId == 1 || iStateId == 2))
            {
                return S_OK;
            }
        	if (m_config.clearBarBg && kname == L"ScrollBar")
            {
                if (PaintScroll(hdc, iPartId, iStateId, pRect))
                    return S_OK;
            }
        }
        return _DrawThemeBackground_.Org(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
    }

    HRESULT MyDrawThemeBackgroundEx(
        HTHEME         hTheme,
        HDC            hdc,
        int            iPartId,
        int            iStateId,
        LPCRECT        pRect,
        const DTBGOPTS* pOptions
    )
    {
    	std::wstring kname = GetThemeClassName(hTheme);
        if (!IsExcludePath())
        {

            //std::wcout << kname << L" p:" << iPartId << L" s:" << iStateId << std::endl;

            //Blur模式下 透明化 Header、Rebar、预览面板、命令模块
            if (kname == L"Header") {
                if ((iPartId == HP_HEADERITEM && iStateId == HIS_NORMAL)
                    || (iPartId == HP_HEADERITEM && iStateId == HIS_SORTEDNORMAL)
                    || (iPartId == 0 && iStateId == HIS_NORMAL))
                    return S_OK;
            }
            else if (kname == L"Rebar")
            {
                if ((iPartId == RP_BACKGROUND || iPartId == RP_BAND) && iStateId == 0)
                {
                    //FillRect(hdc, pRect, m_clearBrush);
                    return S_OK;
                }
            }
            else if (kname == L"PreviewPane")
            {
                if (iPartId == 1 && iStateId == 1)
                    return S_OK;
            }
            else if (kname == L"CommandModule")
            {
                if (iPartId == 1 && iStateId == 0) {

                    _FillRect_.Org(hdc, pRect, m_clearBrush);
                    return S_OK;
                }
            }
            else if (kname == L"ControlPanel")
            {
                if (iPartId == 2 && iStateId == 0) {

                    _FillRect_.Org(hdc, pRect, m_clearBrush);
                    return S_OK;
                }
            }
            else if(m_config.clearBarBg && kname == L"ScrollBar")
            {
                if (PaintScroll(hdc, iPartId, iStateId, pRect))
                    return S_OK;
            }
        }
        return _DrawThemeBackgroundEx_.Org(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
    }

    HRESULT MyCoCreateInstance(const IID& rclsid, LPUNKNOWN pUnkOuter,
        DWORD dwClsContext, const IID& riid, LPVOID* ppv)
    {
        auto ret = _CoCreateInstance_.Org(rclsid, pUnkOuter, dwClsContext, riid, ppv);

        if (m_config.darkRibbon && SUCCEEDED(ret) && rclsid == CLSID_UIRibbonFramework)
        {
            IUIFramework* ribbon = (IUIFramework*)*ppv;
            IPropertyStore* prop = nullptr;
            ribbon->QueryInterface(&prop);

            auto hookProp = (COMHook::IPropertyStore*)prop;

            MH_CreateHook(hookProp->lpVtbl->SetValue, IPropertyStore_SetValue, (void**)&IPropertyStore_SetValue_ORG);
            MH_EnableHook(hookProp->lpVtbl->SetValue);


            prop->Release();
            //std::wcout << "Create Ribbon\n";
        }

        return ret;
    }

    HRESULT IPropertyStore_SetValue(void* _this, const PROPERTYKEY& key, const PROPVARIANT& value)
    {
        if (key == COMHook::UI_PKEY_DarkModeRibbon)
        {
            PROPVARIANT v;
            InitPropVariantFromBoolean(TRUE, &v);
            return IPropertyStore_SetValue_ORG(_this, key, v);

            /*auto hookProp = (COMHook::IPropertyStore*)_this;

            hookProp->lpVtbl->Commit((IPropertyStore*)hookProp);

            PROPVARIANT v1;
            InitPropVariantFromInt32(UI_HSB(231, 100, 69), &v1);

            IPropertyStore_SetValue_ORG(_this, UI_PKEY_GlobalTextColor, v1);

            return S_OK;*/
        }
        return IPropertyStore_SetValue_ORG(_this, key, value);
    }
}
