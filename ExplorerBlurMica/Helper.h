/*
* WinAPI助手
*
* Author: Maple
* date: 2021-7-13 Create
* Copyright winmoes.com
*/

#pragma once
#include "framework.h"
#include <Uxtheme.h>

#include <comdef.h>
#include <gdiplus.h>

#include <intrin.h>

#pragma comment(lib, "GdiPlus.lib")
#pragma comment(lib, "uxtheme.lib")

inline COLORREF M_RGBA(BYTE r, BYTE g, BYTE b, BYTE a)
{
    return RGB(r, g, b) | (a << 24);
}

inline BYTE M_GetAValue(COLORREF rgba)
{
    return BYTE(ULONG(rgba >> 24) & 0xff);
}

//获取当前dll所在目录
extern std::wstring GetCurDllDir();

//判断文件是否存在
extern bool FileIsExist(std::wstring FilePath);

//获取窗口标题
extern std::wstring GetWindowTitle(HWND hWnd);

//获取窗口类名
extern std::wstring GetWindowClassName(HWND hWnd);

//读取配置文件内容
extern std::wstring GetIniString(const std::wstring& FilePath, const std::wstring& AppName, const std::wstring& KeyName);

//转换到小写
extern std::wstring ConvertTolower(std::wstring str);

/*对指定窗口附加Aero效果
* @param hwnd	 - 窗口句柄
* @param win10	 - 是否使用win10函数
* @param type	 - 效果类型 0=Blur 1=Acrylic 2=Mica (win10 1803以上可用) 3=Acrylic Clear
* @param color	 - 窗口背景混合色
*/
extern void StartAero(HWND hwnd, int type, COLORREF color = 0, bool blend = false);

//获取主题类名
extern std::wstring GetThemeClassName(HTHEME hTheme);

//检查函数调用者源
bool CheckCaller(HMODULE caller, void* address = _ReturnAddress());
bool CheckCaller(LPCWSTR caller, void* address = _ReturnAddress());

//计算 Windows 10 Ribbon DPI高度
UINT CalcRibbonHeightForDPI(HWND hWnd, UINT src, bool normal = true, bool offset = true);

//读取注册表SZ值
extern std::wstring RegGetSZ(HKEY hKey, LPCWSTR SubKey, LPCWSTR KeyName);

//刷新Windows10窗口Blur窗口边框设置
void RefreshWin10BlurFrame(bool blurType);

/*Exported entry 126. uxtheme.dll
* DrawThremeTextEx的内部替代函数
*/
extern HRESULT DrawTextWithGlow
(
    HDC hdcMem,
    LPCWSTR pszText,
    UINT cch,
    const RECT* prc,
    DWORD dwFlags,
    COLORREF crText,
    COLORREF crGlow,
    UINT nGlowRadius,
    UINT nGlowIntensity,
    BOOL fPreMultiply,
    DTT_CALLBACK_PROC pfnDrawTextCallback,
    LPARAM lParam
);

//Exported entry 159. dwmapi.dll
extern HRESULT DwmUpdateAccentBlurRect(HWND hWnd, RECT* prc);

//获取系统颜色模式
extern bool SysIsLightMode();

class RoundRectPath : public Gdiplus::GraphicsPath
{

public:
    RoundRectPath(Gdiplus::Rect rc, float round)
    {
        using namespace Gdiplus;
        REAL x = (float)rc.X, y = (float)rc.Y;
        REAL width = (float)rc.Width, height = (float)rc.Height;

        REAL elWid = 2 * round;
        REAL elHei = 2 * round;
        //if (elWid > width) {
        //    elWid = width;
        //    elWid += elWid / 2;
        //}
        //if (elHei > height) {
        //    elHei = height;
        //    elHei += elHei / 2;
        //}

        AddArc(x, y, elWid, elHei, 180, 90); // 左上角圆弧
        AddLine(x + round, y, x + width - round, y); // 上边
        AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // 右上角圆弧
        AddLine(x + width, y + round, x + width, y + height - round);// 右边
        AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // 右下角圆弧
        AddLine(x + width - round, y + height, x + round, y + height); // 下边
        AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
        AddLine(x, y + round, x, y + height - round);

        CloseAllFigures();

    }
};