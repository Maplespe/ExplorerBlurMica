/**
 * FileName: GraphicsHelper.h
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
#ifdef GDIPVER
#undef GDIPVER
#define GDIPVER 0x0110
#else
#define GDIPVER 0x0110
#endif

#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace MBox
{
	namespace Utils
	{
		extern constexpr HRESULT hresult_from_gpstatus(Gdiplus::GpStatus const status);
	}

	class LazyGdip
	{
		ULONG_PTR						m_gdiplusToken{ 0 };
		Gdiplus::GdiplusStartupOutput	m_gdiplusOutput{};
	public:
		static HRESULT EnsureInitialized();
		static LazyGdip& Instance();
		~LazyGdip() noexcept;
		LazyGdip(const LazyGdip&) = delete;
		LazyGdip& operator=(const LazyGdip&) = delete;

		// 通过无占用的方式读取出位图
		Gdiplus::Bitmap* CreateBitmapFromFile(std::wstring_view fileName) const;
		Gdiplus::Bitmap* CreateBitmapFromMemory(const BYTE* pInit, UINT cbInit) const;
	private:
		LazyGdip() = default;
		HRESULT Startup();
	};
}