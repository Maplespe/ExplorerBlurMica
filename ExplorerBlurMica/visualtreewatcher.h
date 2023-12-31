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

		template <typename T = ::IInspectable>
		auto FromHandle(InstanceHandle handle)
		{
			com_ptr<::IInspectable> obj = nullptr;
			m_xamlDiagnostics->GetIInspectableFromHandle(
				handle, obj.put()
			);
			return obj.as<T>();
		}
		template <typename T = ::IInspectable>
		auto FromIInspectable(com_ptr<T> obj)
		{
			InstanceHandle handle = 0;
			check_hresult(
				m_xamlDiagnostics->GetHandleFromIInspectable(obj.get(), &handle)
			);

			return handle;
		}
		auto FromIInspectable(::IInspectable* obj)
		{
			InstanceHandle handle = 0;
			check_hresult(
				m_xamlDiagnostics->GetHandleFromIInspectable(obj, &handle)
			);

			return handle;
		}
		auto GetXamlDiagnostics() const { return m_xamlDiagnostics; }

		using PropertyCallback = std::function<bool(const PropertyChainValue& property)>;
		using PropertySourceCallback = std::function<bool(const PropertyChainSource& source)>;
		IFACEMETHODIMP WalkInstanceHandle(InstanceHandle handle, PropertyCallback propertyCallback, PropertySourceCallback sourceCallback = nullptr);
	private:
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

		com_ptr<IXamlDiagnostics> m_xamlDiagnostics = nullptr;
	};
}