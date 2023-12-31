/**
 * FileName: ThemeHelper.h
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
#include "../framework.h"

namespace MBox
{
	extern const HMODULE g_uxthemeModule;

	enum class WINDOWCOMPOSITIONATTRIBUTE
	{
		WCA_UNDEFINED,
		WCA_NCRENDERING_ENABLED,
		WCA_NCRENDERING_POLICY,
		WCA_TRANSITIONS_FORCEDISABLED,
		WCA_ALLOW_NCPAINT,
		WCA_CAPTION_BUTTON_BOUNDS,
		WCA_NONCLIENT_RTL_LAYOUT,
		WCA_FORCE_ICONIC_REPRESENTATION,
		WCA_EXTENDED_FRAME_BOUNDS,
		WCA_HAS_ICONIC_BITMAP,
		WCA_THEME_ATTRIBUTES,
		WCA_NCRENDERING_EXILED,
		WCA_NCADORNMENTINFO,
		WCA_EXCLUDED_FROM_LIVEPREVIEW,
		WCA_VIDEO_OVERLAY_ACTIVE,
		WCA_FORCE_ACTIVEWINDOW_APPEARANCE,
		WCA_DISALLOW_PEEK,
		WCA_CLOAK,
		WCA_CLOAKED,
		WCA_ACCENT_POLICY,
		WCA_FREEZE_REPRESENTATION,
		WCA_EVER_UNCLOAKED,
		WCA_VISUAL_OWNER,
		WCA_HOLOGRAPHIC,
		WCA_EXCLUDED_FROM_DDA,
		WCA_PASSIVEUPDATEMODE,
		WCA_USEDARKMODECOLORS,
		WCA_CORNER_STYLE,
		WCA_PART_COLOR,
		WCA_DISABLE_MOVESIZE_FEEDBACK,
		WCA_LAST
	};
	struct WINDOWCOMPOSITIONATTRIBUTEDATA
	{
		DWORD dwAttribute;
		PVOID pvData;
		SIZE_T cbData;
	};
	enum class ACCENT_STATE
	{
		ACCENT_DISABLED,
		ACCENT_ENABLE_GRADIENT,
		ACCENT_ENABLE_TRANSPARENTGRADIENT,
		ACCENT_ENABLE_BLURBEHIND,	// Removed in Windows 11 22H2+
		ACCENT_ENABLE_ACRYLICBLURBEHIND,
		ACCENT_ENABLE_HOSTBACKDROP,
		ACCENT_INVALID_STATE
	};
	enum class ACCENT_FLAG
	{
		ACCENT_NONE,
		ACCENT_ENABLE_MODERN_ACRYLIC_RECIPE = 1 << 1,	// Windows 11 22H2+
		ACCENT_ENABLE_GRADIENT_COLOR = 1 << 1, // ACCENT_ENABLE_BLURBEHIND
		ACCENT_ENABLE_FULLSCREEN = 1 << 2,
		ACCENT_ENABLE_BORDER_LEFT = 1 << 5,
		ACCENT_ENABLE_BORDER_TOP = 1 << 6,
		ACCENT_ENABLE_BORDER_RIGHT = 1 << 7,
		ACCENT_ENABLE_BORDER_BOTTOM = 1 << 8,
		ACCENT_ENABLE_BLUR_RECT = 1 << 9,	// DwmpUpdateAccentBlurRect, it is conflicted with ACCENT_ENABLE_GRADIENT_COLOR when using ACCENT_ENABLE_BLURBEHIND
		ACCENT_ENABLE_BORDER = ACCENT_ENABLE_BORDER_LEFT | ACCENT_ENABLE_BORDER_TOP | ACCENT_ENABLE_BORDER_RIGHT | ACCENT_ENABLE_BORDER_BOTTOM
	};
	struct ACCENT_POLICY
	{
		DWORD AccentState;
		DWORD AccentFlags;
		DWORD dwGradientColor;
		DWORD dwAnimationId;
	};

	namespace Utils
	{
		extern BOOL(WINAPI* g_pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBUTEDATA*);

		extern HRESULT DrawTextWithGlow(
			HDC hdc,
			LPCWSTR pszText,
			int cchText,
			LPRECT prc,
			UINT dwFlags,
			UINT crText,
			UINT crGlow,
			UINT nGlowRadius,
			UINT nGlowIntensity,
			BOOL bPreMultiply,
			DTT_CALLBACK_PROC pfnDrawTextCallback,
			LPARAM lParam
		);

		extern HRESULT DrawContentOffscreen(
			HDC hdc,
			LPCRECT paintRect,
			LPCRECT clipRect,
			LPCRECT excludeRect,
			std::function<void(HDC, HPAINTBUFFER, RGBQUAD*, int)> callback,
			UCHAR alpha = 0xFF,
			bool noClip = false,
			bool nonClient = false,
			bool useBlt = false,
			bool update = true,
			bool useCompatibleBitmap = false
		);

		extern bool ShouldAppsUseDarkMode();

		inline COLORREF M_RGBA(BYTE r, BYTE g, BYTE b, BYTE a)
		{
			return RGB(r, g, b) | (a << 24);
		}

		inline BYTE M_GetAValue(COLORREF rgba)
		{
			return BYTE(ULONG(rgba >> 24) & 0xff);
		}

	}
}