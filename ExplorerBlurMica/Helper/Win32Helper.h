/**
 * FileName: Win32Helper.h
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
#include "VersionHelper.h"
#include <inspectable.h>

namespace RAIIHelper
{
	template <typename TLambda>
	class lambda_call
	{
	public:
		lambda_call(const lambda_call&) = delete;
		lambda_call& operator=(const lambda_call&) = delete;
		lambda_call& operator=(lambda_call&& other) = delete;

		inline explicit lambda_call(TLambda&& lambda) noexcept : m_lambda(std::move(lambda))
		{
			static_assert(std::is_same<decltype(lambda()), void>::value, "scope_exit的lambda函数一定不能有返回值");
			static_assert(!std::is_lvalue_reference<TLambda>::value && !std::is_rvalue_reference<TLambda>::value, "scope_exit只能直接与lambda函数一起使用");
		}
		inline lambda_call(lambda_call&& other) noexcept : m_lambda(std::move(other.m_lambda)), m_call(other.m_call)
		{
			other.m_call = false;
		}
		inline ~lambda_call() noexcept
		{
			reset();
		}
		// 确保scope_exit lambda不会被调用
		inline void release() noexcept
		{
			m_call = false;
		}
		// 立刻执行scope_exit lambda如果还没有运行的话；确保它不再次运行
		inline void reset() noexcept
		{
			if (m_call)
			{
				m_call = false;
				m_lambda();
			}
		}
		// 返回true如果scope_exit lambda仍要被执行
		[[nodiscard]] inline explicit operator bool() const noexcept
		{
			return m_call;
		}
	protected:
		TLambda m_lambda;
		bool m_call = true;
	};
	/*
		返回一个对象，该对象在销毁时执行给定的lambda函数，请使用auto捕获返回的对象
		使用reset()提前执行lambda或使用release()避免执行
		在lambda中抛出的异常将引发快速失败
		你可以认为这个对象的作用跟finally差不多
	*/
	template <typename TLambda>
	[[nodiscard]] inline auto scope_exit(TLambda&& lambda) noexcept
	{
		return lambda_call<TLambda>(std::forward<TLambda>(lambda));
	}
}

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
namespace MBox
{
	extern const HMODULE g_user32Module;
	extern const HMODULE g_comctl32Module;

	extern UINT MTBM_ATTACH;
	extern UINT MTBM_DETACH;
	extern const HINSTANCE this_inst;

	namespace Utils
	{
		inline PKUSER_SHARED_DATA get_kernel_shared_info()
		{
			return reinterpret_cast<PKUSER_SHARED_DATA>(0x7FFE0000);
		}
		extern std::string_view get_file_name_ansi(std::string_view path);
		extern HWND GetChildByClass(HWND hwnd, std::wstring_view className);
		extern HWND GetParentByClass(HWND hwnd, std::wstring_view className);
		extern HRESULT get_module_name(HMODULE moduleHandle, LPWSTR moduleName, DWORD size);
		extern HRESULT GetThemeClass(HTHEME hTheme, LPCWSTR pszClassIdList, int cchClass);
		extern HRESULT get_runtime_class_name(::IInspectable* inspectable, LPWSTR className, DWORD size);
		extern void RefreshExplorerTheme();
	}

	template <typename T, bool ignoreNullptr, const void* defaultValue, typename get_name_fn_t, get_name_fn_t get_name_fn, size_t buffer_size = MAX_PATH + 1>
	class obj_name
	{
	public:
		obj_name(const obj_name&) = delete;
		obj_name& operator=(const obj_name&) = delete;
		obj_name& operator=(obj_name&& other) = delete;
		inline bool operator==(std::wstring str) const { return !_wcsicmp(m_name, str.c_str()); }
		inline bool operator==(std::wstring_view str) const { return !_wcsicmp(m_name, str.data()); }
		inline bool operator==(LPCWSTR str) const { return !_wcsicmp(m_name, str); }
		inline T operator=(T obj) { get_name_fn(obj, m_name, buffer_size - 1); return obj; }
		inline obj_name()
		{
			if constexpr (ignoreNullptr || defaultValue != nullptr)
			{
				if constexpr (std::is_same<T, std::wstring_view>::value && defaultValue == nullptr)
				{
					obj_name::operator=(T(L""));
				}
				else
				{
					obj_name::operator=(T(defaultValue));
				}
			}
		}
		inline obj_name(T obj) { obj_name::operator=(obj); }
		~obj_name() = default;
		[[nodiscard]] inline constexpr const wchar_t* const c_str() const { return m_name; }
		[[nodiscard]] inline constexpr const std::wstring_view get() const { return m_name; }
		[[nodiscard]] inline constexpr std::wstring make_wstring() const { return std::wstring(m_name); }
		void* operator new(size_t) = delete;
		void operator delete(void*) = delete;
	private:
		WCHAR m_name[buffer_size] = {};
	};

	template <typename T, bool ignoreNullptr, const void* defaultValue, typename get_name_fn_t, get_name_fn_t get_name_fn, size_t buffer_size = MAX_PATH + 1>
	class obj_name_ansi
	{
	public:
		obj_name_ansi(const obj_name_ansi&) = delete;
		obj_name_ansi& operator=(const obj_name_ansi&) = delete;
		obj_name_ansi& operator=(obj_name_ansi&& other) = delete;
		inline bool operator==(std::string str) const { return !_stricmp(m_name, str.c_str()); }
		inline bool operator==(std::string_view str) const { return !_stricmp(m_name, str.data()); }
		inline bool operator==(LPCSTR str) const { return !_stricmp(m_name, str); }
		inline T operator=(T obj) { get_name_fn(obj, m_name, buffer_size - 1); return obj; }
		inline obj_name_ansi()
		{
			if constexpr (ignoreNullptr || defaultValue != nullptr)
			{
				if constexpr (std::is_same<T, std::string_view>::value && defaultValue == nullptr)
				{
					obj_name_ansi::operator=(T(""));
				}
				else
				{
					obj_name_ansi::operator=(T(defaultValue));
				}
			}
		}
		inline obj_name_ansi(T obj) { obj_name_ansi::operator=(obj); }
		~obj_name_ansi() = default;
		[[nodiscard]] inline constexpr const wchar_t* const c_str() const { return m_name; }
		[[nodiscard]] inline constexpr const std::string_view get() const { return m_name; }
		[[nodiscard]] inline constexpr std::string make_string() const { return std::string(m_name); }
		void* operator new(size_t) = delete;
		void operator delete(void*) = delete;
	private:
		CHAR m_name[buffer_size] = {};
	};

	using atom_name = obj_name<ATOM, false, nullptr, decltype(GetAtomNameW), GetAtomNameW>;
	using window_class = obj_name<HWND, false, nullptr, decltype(GetClassNameW), GetClassNameW>;
	using hmodule_file_name = obj_name<HMODULE, true, nullptr, decltype(GetModuleFileNameW), GetModuleFileNameW>;
	using hmodule_file_name_ansi = obj_name_ansi<HMODULE, true, nullptr, decltype(GetModuleFileNameA), GetModuleFileNameA>;
	using hmodule_name = obj_name<HMODULE, true, nullptr, decltype(Utils::get_module_name), Utils::get_module_name>;
	using htheme_class_name = obj_name<HTHEME, false, nullptr, decltype(Utils::GetThemeClass), Utils::GetThemeClass>;
	using runtime_class_name = obj_name<::IInspectable*, false, nullptr, decltype(Utils::get_runtime_class_name), Utils::get_runtime_class_name>;

	namespace os
	{
		const ULONG buildNumber = Utils::get_kernel_shared_info()->NtBuildNumber;
	}
}