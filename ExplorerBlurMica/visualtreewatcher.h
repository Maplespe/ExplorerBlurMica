/**
 * FileName: visualtreewatcher.h
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

namespace MBox
{
	struct VisualTreeWatcher : implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
	{
	public:
		VisualTreeWatcher(com_ptr<IUnknown> site);
		VisualTreeWatcher(const VisualTreeWatcher&) = delete;
		VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;
		VisualTreeWatcher(VisualTreeWatcher&&) = delete;
		VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;
		~VisualTreeWatcher();
		HRESULT Stop() noexcept;
		void Start();

		template <typename T = ::IInspectable>
		auto FromHandle(InstanceHandle handle)
		{
			com_ptr<IXamlDiagnostics> xamlDiagnostics = nullptr;
			{
				std::lock_guard lock(m_adviceMutex);
				if (m_stopping.load() || !m_xamlDiagnostics) throw_hresult(E_ABORT);
				xamlDiagnostics = m_xamlDiagnostics;
			}

			com_ptr<::IInspectable> obj = nullptr;
			xamlDiagnostics->GetIInspectableFromHandle(
				handle, obj.put()
			);
			return obj.as<T>();
		}
		template <typename T = ::IInspectable>
		auto FromIInspectable(com_ptr<T> obj)
		{
			com_ptr<IXamlDiagnostics> xamlDiagnostics = nullptr;
			{
				std::lock_guard lock(m_adviceMutex);
				if (m_stopping.load() || !m_xamlDiagnostics) throw_hresult(E_ABORT);
				xamlDiagnostics = m_xamlDiagnostics;
			}

			InstanceHandle handle = 0;
			check_hresult(
				xamlDiagnostics->GetHandleFromIInspectable(obj.get(), &handle)
			);

			return handle;
		}
		auto FromIInspectable(::IInspectable* obj)
		{
			com_ptr<IXamlDiagnostics> xamlDiagnostics = nullptr;
			{
				std::lock_guard lock(m_adviceMutex);
				if (m_stopping.load() || !m_xamlDiagnostics) throw_hresult(E_ABORT);
				xamlDiagnostics = m_xamlDiagnostics;
			}

			InstanceHandle handle = 0;
			check_hresult(
				xamlDiagnostics->GetHandleFromIInspectable(obj, &handle)
			);

			return handle;
		}
		auto GetXamlDiagnostics() const
		{
			std::lock_guard lock(m_adviceMutex);
			return m_xamlDiagnostics;
		}

		using PropertyCallback = std::function<bool(const PropertyChainValue& property)>;
		using PropertySourceCallback = std::function<bool(const PropertyChainSource& source)>;
		IFACEMETHODIMP WalkInstanceHandle(InstanceHandle handle, PropertyCallback propertyCallback, PropertySourceCallback sourceCallback = nullptr);
	private:
		HRESULT StartVisualTreeChangeAdvise() noexcept;

		HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
			ParentChildRelation relation,
			VisualElement element,
			VisualMutationType mutationType
		) override;
		HRESULT STDMETHODCALLTYPE OnElementStateChanged(
			InstanceHandle element,
			VisualElementState elementState,
			LPCWSTR context
		) noexcept override;

		mutable std::mutex m_adviceMutex;
		com_ptr<IXamlDiagnostics> m_xamlDiagnostics = nullptr;
		com_ptr<IVisualTreeService3> m_visualTreeService = nullptr;
		bool m_advised = false;
		std::atomic_bool m_stopping = false;
	};
}
