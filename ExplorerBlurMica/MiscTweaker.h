/**
 * FileName: MiscTweaker.h
 *
 * Copyright (C) 2022-2024 Maplespe
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
#include "HookDef.h"
#include "visualtreewatcher.h"

namespace MBox::MiscTweaker
{
	void Startup();
	void Shutdown();

	extern LRESULT WINAPI MyDefWindowProcW(
		HWND hWnd,
		UINT Msg,
		WPARAM wParam,
		LPARAM lParam
	);

	extern HRESULT WINAPI MyDrawThemeBackground(
		HTHEME hTheme,
		HDC hdc,
		int iPartId,
		int iStateId,
		LPCRECT pRect,
		LPCRECT pClipRect
	);

	extern HRESULT WINAPI MyDrawThemeBackground_Address(
		HTHEME hTheme,
		HDC hdc,
		int iPartId,
		int iStateId,
		LPCRECT pRect,
		LPCRECT pClipRect
	);

	extern HRESULT __stdcall MyCoCreateInstance(
		REFCLSID rclsid,
		LPUNKNOWN pUnkOuter,
		DWORD dwClsContext,
		REFIID riid,
		LPVOID* ppv
	);

	extern HRESULT WINAPI MyGetThemeColor_WinUI(
		HTHEME hTheme,
		int iPartId,
		int iStateId,
		int iPropId,
		COLORREF* pColor
	);

	extern HRESULT WINAPI MyGetThemeColor_WASDK(
		HTHEME hTheme,
		int iPartId,
		int iStateId,
		int iPropId,
		COLORREF* pColor
	);

	void OnVisualTreeChange(
		com_ptr<VisualTreeWatcher> visualTreeWatcher,
		const ParentChildRelation& parentChildRelation,
		const VisualElement& element,
		VisualMutationType mutationType
	);
}