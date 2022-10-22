/*
* Hook函数集
*
* Author: Maple
* date: 2022-7-3 Create
* Copyright winmoes.com
*/
#include "HookDef.h"
#include <unordered_map>
#include <functional>
#include <vssym32.h>
#include <dwmapi.h>
#include <mutex>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Comctl32.lib")    //Win32 Controls

struct DUIData
{
    HWND hWnd = 0;      //DirectUIHWND
    HWND mainWnd = 0;   //Explorer Window
    HWND TreeWnd = 0;   //TreeView

    //DirectUIHWND
    HDC hDC = 0;
    HDC srcDC = 0;

    SIZE size = { 0,0 };
    bool treeDraw = false;
    bool refresh = true;
};
struct Config
{
    int effType = 0;        //效果类型 0=Blur 1=Acrylic 2=Mica
    COLORREF blendColor = 0;//混合颜色
    bool smallborder = true;
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

private:
    std::mutex m_mutex;
    std::unordered_map<T, T1> m_map;
};

hashMap<DWORD, DUIData> m_DUIList;                  //dui句柄列表
hashMap<DWORD, std::pair<HWND, HDC>> m_ribbonPaint; //用来记录win10 Ribbon区域是否绘制
hashMap<DWORD, bool> m_drawtextState;               //用来记录drawText Alpha修复状态
HBRUSH m_clearBrush = 0;                                       //用于清空DC的画刷
Config m_config;                                               //配置文件


inline COLORREF M_RGBA(BYTE r, BYTE g, BYTE b, BYTE a)
{
    return RGB(r, g, b) | (a << 24);
}

inline BYTE M_GetAValue(COLORREF rgba)
{
    return BYTE(ULONG(rgba >> 24) & 0xff);
}

void RefreshConfig()
{
    if (!m_clearBrush)
        m_clearBrush = CreateSolidBrush(0x00000000);
    
    std::wstring path = GetCurDllDir() + L"\\config.ini";
    auto cfg = GetIniString(path, L"config", L"effect");

    //效果类型
    m_config.effType = _wtoi(cfg.c_str());
    if (m_config.effType < 0 || m_config.effType > 2)
        m_config.effType = 0;

    //22000以后不支持Blur效果
    if (m_config.effType == 0 && g_sysBuildNumber > 22000)
        m_config.effType = 1;

    //颜色RGBA
    auto color = [&path](std::wstring s) -> BYTE
    {
        int c = _wtoi(GetIniString(path, L"blend", s).c_str());
        if (c > 255) c = 255;
        else if (c < 0) c = 0;

        return BYTE(c);
    };
    auto& c = m_config.blendColor;
    c = M_RGBA(color(L"r"), color(L"g"), color(L"b"), color(L"a"));

    //纯黑或者纯白alpha会失效 限制最小或最大值
    if (GetRValue(c) == 255 && GetGValue(c) == 255 && GetBValue(c) == 255)
        c = M_RGBA(249, 249, 249, M_GetAValue(c));
    else if (GetRValue(c) == 0 && GetGValue(c) == 0 && GetBValue(c) == 0)
        c = M_RGBA(40, 40, 40, M_GetAValue(c));

    m_config.smallborder = GetIniString(path, L"config", L"smallBorder") == L"false" ? false : true;

    RefreshWin10BlurFrame(m_config.smallborder);
}

namespace Hook
{
    int hookIndex = 0;

    auto _CreateWindowExW_          = HookDef(CreateWindowExW, MyCreateWindowExW);
    auto _DestroyWindow_            = HookDef(DestroyWindow, MyDestroyWindow);
    auto _BeginPaint_               = HookDef(BeginPaint, MyBeginPaint);
    auto _EndPaint_                 = HookDef(EndPaint, MyEndPaint);
    auto _FillRect_                 = HookDef(FillRect, MyFillRect);
    auto _DrawTextW_                = HookDef(DrawTextW, MyDrawTextW);
    auto _DrawTextExW_              = HookDef(DrawTextExW, MyDrawTextExW);
    auto _ExtTextOutW_              = HookDef(ExtTextOutW, MyExtTextOutW);
    auto _CreateCompatibleDC_       = HookDef(CreateCompatibleDC, MyCreateCompatibleDC);
    auto _GetThemeColor_            = HookDef(GetThemeColor, MyGetThemeColor);
    auto _DrawThemeText_            = HookDef(DrawThemeText, MyDrawThemeText);
    auto _DrawThemeTextEx_          = HookDef(DrawThemeTextEx, MyDrawThemeTextEx);
    auto _DrawThemeBackground_      = HookDef(DrawThemeBackground, MyDrawThemeBackground);
    auto _DrawThemeBackgroundEx_    = HookDef(DrawThemeBackgroundEx, MyDrawThemeBackgroundEx);
    auto _PatBlt_                   = HookDef(PatBlt, MyPatBlt);

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
        if (g_sysBuildNumber >= 22000)
            _DrawThemeBackground_.Attach();

        _DrawThemeBackgroundEx_.Attach();
        _PatBlt_.Attach();
    }

    void HookDetachAll()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }

    //是否为dui线程
    bool IsDUIThread()
    {
        auto iter = m_DUIList.find(GetCurrentThreadId());
        return iter != m_DUIList.end();
    }

    //刷新22H2的窗口标题栏区域
    void OnWindowSize(HWND hWnd, int newHeight)
    {
        MARGINS margin = { -1 };
        if (M_GetAValue(m_config.blendColor) != 0 && m_config.effType == 1) {
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
        bool isBlend = M_GetAValue(m_config.blendColor) != 0;
        if (g_sysBuildNumber >= 22000)
        {
            if (g_sysBuildNumber >= 22500)
            {
                RECT pRect;
                GetWindowRect(hWnd, &pRect);
                OnWindowSize(hWnd, pRect.bottom - pRect.top);
                if (m_config.effType == 1)
                {
                    //禁用winrt私有云母合成效果
                    int type = 0;
                    DwmSetWindowAttribute(hWnd, 1029, &type, sizeof(type));

                    //22621.655
                    DWM_BLURBEHIND blur = { 0 };
                    blur.dwFlags = 1;
                    blur.fEnable = 1;
                    DwmEnableBlurBehindWindow(hWnd, &blur);

                    if (isBlend)
                        StartAero(hWnd, 1, m_config.blendColor, true);
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
            case 2:
                StartAero(hWnd, 2, 0, false);
            }
        }
        else
            StartAero(hWnd, m_config.effType == 1 ? 0 : 1, m_config.blendColor, isBlend);
    }

    //窗口子类化
    LRESULT WINAPI MyWndSubProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        if (message == WM_SIZE)
        {
            int height = HIWORD(lparam);
            OnWindowSize(hWnd, height);
        }
        else if (message == WM_NCCALCSIZE)
        {
            if (!lparam)
                return DefSubclassProc(hWnd, message, wparam, lparam);

            UINT dpi = GetDpiForWindow(hWnd);

            int frameX = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
            int frameY = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
            int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

            NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lparam;

            params->rgrc->right -= frameX + padding;
            params->rgrc->left += frameX + padding;
            params->rgrc->bottom -= frameY + padding;

            return 0;
        }
        //22H2 22621.655 多标签页失去和获取焦点需要刷新
        else if (message == WM_ACTIVATE)
        {
            auto ret = DefSubclassProc(hWnd, message, wparam, lparam);
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
        if (IsDUIThread()) {
            if (ConvertTolower(ClassName) == L"edit")
            {
                SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
        }

        //explorer window 
        if (ClassName == L"DirectUIHWND" && GetWindowClassName(hWndParent) == L"SHELLDLL_DefView")
        {
            //继续查找父级
            HWND parent = GetParent(hWndParent);
            if (GetWindowClassName(parent) == L"ShellTabWindowClass")
            {
                parent = GetParent(parent);

                //设置Blur
                SetWindowBlur(parent);

                //22H2
                if (g_sysBuildNumber >= 22500)
                {
                    SetWindowSubclass(parent, MyWndSubProc, 0, (DWORD_PTR)0);
                }

                //记录到列表中 Add to list
                DWORD tid = GetCurrentThreadId();

                DUIData data;
                data.hWnd = hWnd;
                data.mainWnd = parent;

                m_DUIList[tid] = data;
            }
        }
        //导航树视图
        else if (ClassName == L"SysTreeView32")
        {
            HWND parent = GetParent(hWndParent);//NamespaceTreeControl
            parent = GetParent(parent);         //CtrlNotifySink
            parent = GetParent(parent);         //DirectUIHWND
            parent = GetParent(parent);         //DUIViewWndClassName
            std::wstring mainWndCls = GetWindowClassName(parent);

            if (mainWndCls == L"ShellTabWindowClass")
                m_DUIList[GetCurrentThreadId()].TreeWnd = hWnd;
        }
        return hWnd;
    }

    BOOL MyDestroyWindow(HWND hWnd)
    {
        //查找并删除列表中的记录
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end())
        {
            if (iter->second.hWnd == hWnd) {
                m_DUIList.erase(iter);
            }
        }
        return _DestroyWindow_.Org(hWnd);
    }

    HDC MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
    {
        //开始绘制DUI窗口
        HDC hDC = _BeginPaint_.Org(hWnd, lpPaint);

        auto iter = m_DUIList.find(GetCurrentThreadId());

        if (iter != m_DUIList.end()) {
            if (iter->second.hWnd == hWnd)
            {
                iter->second.srcDC = hDC;
                iter->second.hDC = hDC;
                iter->second.treeDraw = false;
            }
            else if (iter->second.TreeWnd == hWnd)
            {
                iter->second.srcDC = hDC;
                iter->second.hDC = hDC;
                iter->second.treeDraw = true;
            }
        }
        return hDC;
    }

    BOOL MyEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint)
    {
        DWORD curThread = GetCurrentThreadId();
        auto iter = m_DUIList.find(curThread);

        if (iter != m_DUIList.end()) {
            if (iter->second.hWnd == hWnd
                || iter->second.TreeWnd == hWnd)
            {
                iter->second.srcDC = 0;
                iter->second.hDC = 0;
                iter->second.treeDraw = false;
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
        int ret = S_OK;

        DWORD curThread = GetCurrentThreadId();

        auto iter = m_DUIList.find(curThread);
        if (iter != m_DUIList.end()) {
            if (iter->second.hDC == hDC)
            {
                ret = _FillRect_.Org(hDC, lprc, hbr);

                //刷新颜色值
                if (iter->second.refresh)
                {
                    SendMessageW(iter->second.hWnd, WM_THEMECHANGED, 0, 0);

                    //win11 22621.655 多标签页 第一次打开页面需要刷新
                    if (g_sysBuildNumber >= 22600)
                    {
                        SetWindowBlur(iter->second.mainWnd);
                    }

                    iter->second.refresh = false;
                }
                //判断树列表颜色是否被改变
                if (iter->second.treeDraw)
                {
                    COLORREF color = RGB(0, 0, 0);
                    if (!CompareColor(TreeView_GetBkColor(iter->second.TreeWnd), color))
                        TreeView_SetBkColor(iter->second.TreeWnd, color);
                }
                return ret;
            }
            else
                goto Next;
        }
    Next:
        //透明化 Windows 10 Ribbon 在Light模式
        auto ribiter = m_ribbonPaint.find(curThread);
        if (ribiter != m_ribbonPaint.end())
        {
            if (ribiter->second.second == hDC) {
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
        return  _FillRect_.Org(hDC, lprc, hbr);
    }

    BOOL MyPatBlt(HDC hdc, int x, int y, int w, int h, DWORD rop)
    {
        //修复选择矩形框的Alpha
        if (IsDUIThread() && rop == PATCOPY) {
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

    bool AlphaBuffer(HDC hdc, LPRECT pRc, std::function<void(HDC)> fun)
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

            fun(hDC);

            EndBufferedPaint(pbuffer, TRUE);

            return true;
        }
        return false;
    }

    //修复DrawTextW的Alpha
    int MyDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
    {
        if ((format & DT_CALCRECT) || m_drawtextState.find(GetCurrentThreadId()) != m_drawtextState.end())
            return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);

        HRESULT hr = S_OK;
        HDC hDC = 0;
        DTTOPTS dtop = { 0 };
        dtop.dwSize = sizeof(DTTOPTS);
        dtop.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
        dtop.crText = GetTextColor(hdc);
        dtop.pfnDrawTextCallback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
        {
            return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
        };

        auto fun = [&](HDC hDC) {
            HTHEME hTheme = OpenThemeData((HWND)0, L"Menu");
            hr = MyDrawThemeTextEx(hTheme, hDC, 0, 0, lpchText, cchText, format, lprc, &dtop);
            CloseThemeData(hTheme);
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
        static std::unordered_map<DWORD, bool> thList;
        DWORD curTh = GetCurrentThreadId();

        //TreeView绘制的时候最后一个参数是NULL 因此可以直接用DrawText
        if (!lpdtp && !(format & DT_CALCRECT) && thList.find(curTh) == thList.end()
            && m_drawtextState.find(GetCurrentThreadId()) == m_drawtextState.end()) {

            thList.insert(std::make_pair(curTh, true));
            auto ret = MyDrawTextW(hdc, lpchText, cchText, lprc, format);
            thList.erase(curTh);
            return ret;
        }
        return _DrawTextExW_.Org(hdc, lpchText, cchText, lprc, format, lpdtp);
    }

    //修复ExtTextOutW的Alpha
    BOOL MyExtTextOutW(HDC hdc, int x, int y, UINT option, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
    {
        std::wstring str;
        if (lpString) str = lpString;

        static std::unordered_map<DWORD, bool> thList;
        DWORD curTh = GetCurrentThreadId();

        if (IsDUIThread() && !(option & ETO_GLYPH_INDEX) && !(option & ETO_IGNORELANGUAGE)
            && thList.find(curTh) == thList.end() && str != L"" && m_drawtextState.find(curTh) == m_drawtextState.end())
        {
            thList.insert(std::make_pair(curTh, true));

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

                HRESULT hr = S_OK;
                HDC hDC = 0;
                DTTOPTS dtop = { 0 };
                dtop.dwSize = sizeof(DTTOPTS);
                dtop.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
                dtop.crText = GetTextColor(hdc);
                dtop.pfnDrawTextCallback = [](HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPARAM lParam)
                {
                    return _DrawTextW_.Org(hdc, lpchText, cchText, lprc, format);
                };

                //合批绘制文本
                auto fun = [&](HDC hDC) {
                    HTHEME hTheme = OpenThemeData((HWND)0, L"Menu");

                    std::wstring batchStr;
                    bool batch = true;

                    batchStr += lpString[0];

                    int srcExtra = GetTextCharacterExtra(hdc);
                    SetTextCharacterExtra(hDC, lpDx[0]);

                    RECT batchRc = rc;
                    for (int i = 0; i < c; i++)
                    {
                        if (i != 0) {
                            if (lpDx[i] == lpDx[i - 1]) {
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
                                hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);

                                batch = false;
                                batchStr = lpString[i];
                                SetTextCharacterExtra(hDC, lpDx[i]);
                                batchRc.left = rc.left;
                            }
                        }

                        //最后一批
                        if (i == c - 1)
                        {
                            hr = _DrawThemeTextEx_.Org(hTheme, hDC, 0, 0, batchStr.c_str(), batchStr.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &batchRc, &dtop);
                        }

                        rc.left += lpDx[i];
                    }
                    SetTextCharacterExtra(hDC, srcExtra);
                    CloseThemeData(hTheme);
                };
                if (!AlphaBuffer(hdc, &rc, fun))
                {
                    hr = _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
                }

                if (option & ETO_CLIPPED)
                    RestoreDC(hdc, -1);
            }

            thList.erase(curTh);
            return 1;
        }
        return _ExtTextOutW_.Org(hdc, x, y, option, lprect, lpString, c, lpDx);
    }

    HDC MyCreateCompatibleDC(HDC hDC)
    {
        //在绘制DUI之前 会调用CreateCompatibleDC 找到它
        HDC retDC = _CreateCompatibleDC_.Org(hDC);
        auto iter = m_DUIList.find(GetCurrentThreadId());
        if (iter != m_DUIList.end()) {
            if (iter->second.hDC == hDC) {
                iter->second.hDC = retDC;
                return retDC;
            }
        }
        //Win10 Ribbon
        if (g_sysBuildNumber < 22000) {
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
        if (iPropId == TMT_FILLCOLOR && IsDUIThread())
        {
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
        HRESULT ret = S_OK;

        DTTOPTS Options = { sizeof(DTTOPTS) };
        RECT Rect = *pRect;
        ret = MyDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, &Rect, &Options);

        return ret;
    }

    //防止DrawText类API内部递归调用 并进行Alpha修复
    HRESULT MyDrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCTSTR pszText,
        int cchText, DWORD dwTextFlags, LPCRECT pRect, const DTTOPTS* pOptions)
    {
        HRESULT ret = S_OK;

        if (pOptions && !(pOptions->dwFlags & DTT_CALCRECT) && !(pOptions->dwFlags & DTT_COMPOSITED))
        {
            DTTOPTS Options = *pOptions;
            Options.dwFlags |= DTT_COMPOSITED;

            HRESULT ret = S_OK;

            auto fun = [&](HDC hDC)
            {
                auto tid = GetCurrentThreadId();
                m_drawtextState.insert(std::make_pair(tid, true));
                ret = _DrawThemeTextEx_.Org(hTheme, hDC, iPartId, iStateId, pszText, cchText, dwTextFlags,
                    (LPRECT)pRect, &Options);
                m_drawtextState.erase(tid);
            };

            if (!AlphaBuffer(hdc, (LPRECT)pRect, fun))
                goto Org;

            return ret;
        }
        else {
        Org:
            ret = _DrawThemeTextEx_.Org(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, (LPRECT)pRect, pOptions);
        }
        return ret;
    }

    HRESULT MyDrawThemeBackground(HTHEME  hTheme,
        HDC     hdc,
        int     iPartId,
        int     iStateId,
        LPCRECT pRect,
        LPCRECT pClipRect)
    {
        std::wstring kname = GetThemeClassName(hTheme);

        if (kname == L"Rebar" && (iPartId == RP_BACKGROUND || iPartId == RP_BAND) && iStateId == 0)
        {
            return S_OK;
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
                return S_OK;
            }
        }
        else if (kname == L"CommandModule" && IsDUIThread())
        {
            if (iPartId == 1 && iStateId == 0) {

                FillRect(hdc, pRect, m_clearBrush);
                return S_OK;
            }
        }
        else if (kname == L"PreviewPane")
        {
            if (iPartId == 1 && iStateId == 1)
                return S_OK;
        }

        return _DrawThemeBackgroundEx_.Org(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
    }
}
