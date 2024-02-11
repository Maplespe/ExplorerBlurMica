/**
 * FileName: HookDef.h
 *
 * Copyright (C) 2022-2024 Maplespe、ALTaleX
 *
 * This file is part of MToolBox and ExplorerBlurMica.
 * ExplorerBlurMica is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation, either version 3
 * of the License, or any later version.
 *
 * ExplorerBlurMica is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/
#pragma once
#include "Helper/DirectUIHelper.h"
#include "HookHelper/MHookHelper.h"
#include "DirectUITweaker.h"

namespace MBox
{
	extern bool g_enableCustomScrollBar;
	extern bool g_enableClearAddress;
	extern bool g_enableClearWinUI;
	extern bool g_enableShowLine;

	void WINAPI My_DirectUI_Element_PaintBackground(
		DirectUI::Element* This,
		HDC hdc,
		DirectUI::Value* value,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);

	inline MiniHookDispatcher g_DirectUI_Element_PaintBackground_hookDispatcher =
	{
		DirectUI::g_pfnElement_PaintBackground,
		std::array
		{
			detour_info(DTweaker::My_DirectUI_Element_PaintBackground_Filter, call_type::before),
			detour_info(DTweaker::My_DirectUI_Element_PaintBackground_CleanUp, call_type::final),
			detour_info(My_DirectUI_Element_PaintBackground, call_type::before)
		}
	};

	void WINAPI My_DirectUI_Element_Paint(
		DirectUI::Element* This,
		HDC hdc,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);

	HRESULT WINAPI My_DwmExtendFrameIntoClientArea(
		HWND hWnd,
		const MARGINS* pMarInset
	);

	HRESULT WINAPI My_DwmExtendFrameIntoClientArea_startallback(
		HWND hWnd,
		const MARGINS* pMarInset
	);

	HRESULT WINAPI MyDwmSetWindowAttribute(
		HWND hwnd,
		DWORD dwAttribute,
		LPCVOID pvAttribute,
		DWORD cbAttribute
	);

	HWND WINAPI My_CreateWindowExW_explorerframe(
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
	);

	HWND WINAPI My_CreateWindowExW_dui70(
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
	);

	int WINAPI My_DrawTextW(
		HDC hdc,
		LPCWSTR lpchText,
		int cchText,
		LPRECT lprc,
		UINT format
	);

	BOOL WINAPI My_ExtTextOutW(
		HDC hdc,
		int x,
		int y,
		UINT options,
		const RECT* lprect,
		LPCWSTR lpString,
		UINT c,
		const INT* lpDx
	);

	HRESULT WINAPI My_DrawThemeText(
		HTHEME  hTheme,
		HDC     hdc,
		int     iPartId,
		int     iStateId,
		LPCWSTR pszText,
		int     cchText,
		DWORD   dwTextFlags,
		DWORD   dwTextFlags2,
		LPCRECT pRect
	);

	HRESULT WINAPI My_DrawThemeTextEx(
		HTHEME        hTheme,
		HDC           hdc,
		int           iPartId,
		int           iStateId,
		LPCWSTR       pszText,
		int           cchText,
		DWORD         dwTextFlags,
		LPRECT        pRect,
		const DTTOPTS* pOptions
	);

	BOOL WINAPI My_InflateRect(
		LPRECT lprc,
		int dx,
		int dy
	);

	HWND WINAPI My_CreateWindowExW_shell32(
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
	);

	//RibbonTweaker
	namespace RibbonTweaker
	{
		HDC WINAPI My_BeginPaint(
			HWND hWnd,
			LPPAINTSTRUCT lpPaint
		);
		int WINAPI My_FillRect(
			HDC hDC,
			const RECT* lprc,
			HBRUSH hbr
		);
		int WINAPI My_DrawTextW(
			HDC hdc,
			LPCWSTR lpchText,
			int cchText,
			LPRECT lprc,
			UINT format
		);
		BOOL WINAPI My_StretchBlt(
			HDC   hdcDest,
			int   xDest,
			int   yDest,
			int   wDest,
			int   hDest,
			HDC   hdcSrc,
			int   xSrc,
			int   ySrc,
			int   wSrc,
			int   hSrc,
			DWORD rop
		);
		BOOL WINAPI My_BitBlt(
			HDC   hdc,
			int   x,
			int   y,
			int   cx,
			int   cy,
			HDC   hdcSrc,
			int   x1,
			int   y1,
			DWORD rop
		);
		BOOL WINAPI My_GdiAlphaBlend(
			HDC           hdcDest,
			int           xoriginDest,
			int           yoriginDest,
			int           wDest,
			int           hDest,
			HDC           hdcSrc,
			int           xoriginSrc,
			int           yoriginSrc,
			int           wSrc,
			int           hSrc,
			BLENDFUNCTION ftn
		);
	}
}