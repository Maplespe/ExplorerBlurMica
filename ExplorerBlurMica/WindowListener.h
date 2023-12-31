/**
 * FileName: WindowListener.h
 *
 * Copyright (C) 2022-2023 Maplespe、ALTaleX
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

namespace MBox
{
	struct IWindowListenerCallback
	{
		virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) = 0;
	};

	class WindowListenerImpl
	{
	protected:
		HWND m_hwnd = nullptr;
		mutable std::shared_ptr<IWindowListenerCallback> m_listener = nullptr;

		WindowListenerImpl(HWND hwnd);
		~WindowListenerImpl();

		static LRESULT CALLBACK s_SubclassProc(
			HWND hWnd,
			UINT uMsg,
			WPARAM wParam,
			LPARAM lParam,
			UINT_PTR uIdSubclass,
			DWORD_PTR dwRefData
		);
	public:
		HWND GetHwnd() const { return m_hwnd; }
		template <typename T = IWindowListenerCallback>
		std::shared_ptr<T> GetListener() const
		{
			return std::dynamic_pointer_cast<T>(m_listener);
		}

		static void Detach(HWND hwnd);
		static WindowListenerImpl* Get(HWND hwnd);
	};

	class WindowListener : public WindowListenerImpl
	{
		static std::unordered_map<ULONG_PTR, std::vector<HWND>> s_windowList;
		ULONG_PTR m_token = 0;
	public:
		WindowListener(HWND hwnd, ULONG_PTR token) : WindowListenerImpl(hwnd), m_token(token)
		{
			s_windowList[token].push_back(hwnd);
		}
		~WindowListener()
		{
			for (auto it = s_windowList[m_token].begin(); it != s_windowList[m_token].end(); it++) if (*it == m_hwnd) { it = s_windowList[m_token].erase(it); break; }
		}

		static void DetachAll(ULONG_PTR token)
		{
			for (const auto& window : s_windowList[token])
			{
				WindowListenerImpl::Detach(window);
			}
			s_windowList.erase(token);
		}
		static WindowListener* Attach(HWND hwnd, ULONG_PTR token, std::function<void(const WindowListener* ptr, bool newlyCreated, std::shared_ptr<IWindowListenerCallback>& listener)> callback)
		{
			WindowListener* windowListener = static_cast<WindowListener*>(WindowListenerImpl::Get(hwnd));
			if (windowListener)
			{
				if (callback)
				{
					callback(windowListener, false, windowListener->m_listener);
				}
			}
			else
			{
				auto ptr = std::make_unique<WindowListener>(hwnd, token);
				if (callback)
				{
					callback(ptr.get(), true, ptr->m_listener);
				}
				windowListener = ptr.release();
			}

			return windowListener;
		}
		static WindowListener* Get(HWND hwnd)
		{
			return static_cast<WindowListener*>(WindowListenerImpl::Get(hwnd));
		}
	};
}