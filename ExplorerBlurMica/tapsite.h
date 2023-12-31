/**
 * FileName: tapsite.h
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
#include "framework.h"
#include "Helper/comdef.h"
#include "Helper/WinRTHelper.h"
#include "visualtreewatcher.h"

namespace MBox
{
	// {233A5395-98B8-40C9-8DEE-1BD9158B34CA}
	constexpr CLSID CLSID_TAPSite =
	{ 0x233a5395, 0x98b8, 0x40c9, { 0x8d, 0xee, 0x1b, 0xd9, 0x15, 0x8b, 0x34, 0xca } };

	struct TAPSite : implements<TAPSite, IObjectWithSite, winrt::non_agile>
	{
	public:
		HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override;
		HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override;
		static HRESULT ShutdownVisualTreeWatcher();
	private:
		static com_ptr<VisualTreeWatcher> s_VisualTreeWatcher;

		com_ptr<IUnknown> m_site = nullptr;
	};
}