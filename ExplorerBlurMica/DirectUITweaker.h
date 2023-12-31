/**
 * FileName: DirectUITWeaker.h
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
#include "WindowListener.h"

namespace MBox::DTweaker
{
	void DUIRequireVerticalLine();
	void DUIRequireHorizontalLine();
	void DUIRequireOutline();

	void WINAPI My_DirectUI_Element_PaintBackground_Filter(
		DirectUI::Element* This,
		HDC hdc,
		DirectUI::Value* value,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);

	void WINAPI My_DirectUI_Element_PaintBackground_CleanUp(
		DirectUI::Element* This,
		HDC hdc,
		DirectUI::Value* value,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);

	class BlinkEraserListener : public IWindowListenerCallback
	{
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		BlinkEraserListener() = default;
		~BlinkEraserListener() = default;
	};

	class CabinetWClassListener : public IWindowListenerCallback
	{
		using BackdropEffectUpdateCallback = std::function<void(HWND hwnd, const SIZE& size)>;
		using BackdropEffectApplyCallback = std::function<void(HWND hwnd, const SIZE& size)>;
		BackdropEffectUpdateCallback m_callback1 = nullptr;
		BackdropEffectApplyCallback m_callback2 = nullptr;
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		CabinetWClassListener(BackdropEffectUpdateCallback callback1, BackdropEffectApplyCallback callback2) : m_callback1(callback1), m_callback2(callback2) {}
		~CabinetWClassListener() = default;
	};

	class BlackDesktopChildSiteBridgeListener : public IWindowListenerCallback
	{
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		BlackDesktopChildSiteBridgeListener() = default;
		~BlackDesktopChildSiteBridgeListener() = default;
	};
}