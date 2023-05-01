/*
* COM Hook定义
*
* Author: Maple
* date: 2021-t-1 Create
* Copyright winmoes.com
*/
#pragma once
#include "framework.h"
#include <UIRibbon.h>

namespace COMHook
{
    DEFINE_UIPROPERTYKEY(UI_PKEY_DarkModeRibbon, VT_BOOL, 2004)

	struct IUIFrameworkVtbl
    {
        BEGIN_INTERFACE

		DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
            HRESULT(STDMETHODCALLTYPE* QueryInterface)(
                IUIFramework* This,
                /* [in] */ REFIID riid,
                /* [annotation][iid_is][out] */
                _COM_Outptr_  void** ppvObject);

        DECLSPEC_XFGVIRT(IUnknown, AddRef)
            ULONG(STDMETHODCALLTYPE* AddRef)(
                IUIFramework* This);

        DECLSPEC_XFGVIRT(IUnknown, Release)
            ULONG(STDMETHODCALLTYPE* Release)(
                IUIFramework* This);

        DECLSPEC_XFGVIRT(IUIFramework, Initialize)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* Initialize)(
                IUIFramework* This,
                /* [in] */ HWND frameWnd,
                /* [in] */ IUIApplication* application);

        DECLSPEC_XFGVIRT(IUIFramework, Destroy)
            HRESULT(STDMETHODCALLTYPE* Destroy)(
                IUIFramework* This);

        DECLSPEC_XFGVIRT(IUIFramework, LoadUI)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* LoadUI)(
                IUIFramework* This,
                /* [in] */ HINSTANCE instance,
                /* [in] */ LPCWSTR resourceName);

        DECLSPEC_XFGVIRT(IUIFramework, GetView)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* GetView)(
                IUIFramework* This,
                UINT32 viewId,
                REFIID riid,
                /* [annotation][iid_is][out] */
                _Outptr_  void** ppv);

        DECLSPEC_XFGVIRT(IUIFramework, GetUICommandProperty)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* GetUICommandProperty)(
                IUIFramework* This,
                UINT32 commandId,
                /* [in] */ REFPROPERTYKEY key,
                /* [out] */ PROPVARIANT* value);

        DECLSPEC_XFGVIRT(IUIFramework, SetUICommandProperty)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* SetUICommandProperty)(
                IUIFramework* This,
                UINT32 commandId,
                /* [in] */ REFPROPERTYKEY key,
                /* [in] */ REFPROPVARIANT value);

        DECLSPEC_XFGVIRT(IUIFramework, InvalidateUICommand)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* InvalidateUICommand)(
                IUIFramework* This,
                UINT32 commandId,
                UI_INVALIDATIONS flags,
                /* [annotation][in] */
                _In_opt_  const PROPERTYKEY* key);

        DECLSPEC_XFGVIRT(IUIFramework, FlushPendingInvalidations)
            HRESULT(STDMETHODCALLTYPE* FlushPendingInvalidations)(
                IUIFramework* This);

        DECLSPEC_XFGVIRT(IUIFramework, SetModes)
            /* [annotation] */
            _Check_return_
            HRESULT(STDMETHODCALLTYPE* SetModes)(
                IUIFramework* This,
                INT32 iModes);

        END_INTERFACE
    };

    interface IUIFramework
    {
        CONST_VTBL IUIFrameworkVtbl* lpVtbl;
    };

	struct IPropertyStoreVtbl
    {
        BEGIN_INTERFACE

    	DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
            HRESULT(STDMETHODCALLTYPE* QueryInterface)(
                __RPC__in IPropertyStore* This,
                /* [in] */ __RPC__in REFIID riid,
                /* [annotation][iid_is][out] */
                _COM_Outptr_  void** ppvObject);

        DECLSPEC_XFGVIRT(IUnknown, AddRef)
            ULONG(STDMETHODCALLTYPE* AddRef)(
                __RPC__in IPropertyStore* This);

        DECLSPEC_XFGVIRT(IUnknown, Release)
            ULONG(STDMETHODCALLTYPE* Release)(
                __RPC__in IPropertyStore* This);

        DECLSPEC_XFGVIRT(IPropertyStore, GetCount)
            HRESULT(STDMETHODCALLTYPE* GetCount)(
                __RPC__in IPropertyStore* This,
                /* [out] */ __RPC__out DWORD* cProps);

        DECLSPEC_XFGVIRT(IPropertyStore, GetAt)
            HRESULT(STDMETHODCALLTYPE* GetAt)(
                __RPC__in IPropertyStore* This,
                /* [in] */ DWORD iProp,
                /* [out] */ __RPC__out PROPERTYKEY* pkey);

        DECLSPEC_XFGVIRT(IPropertyStore, GetValue)
            HRESULT(STDMETHODCALLTYPE* GetValue)(
                __RPC__in IPropertyStore* This,
                /* [in] */ __RPC__in REFPROPERTYKEY key,
                /* [out] */ __RPC__out PROPVARIANT* pv);

        DECLSPEC_XFGVIRT(IPropertyStore, SetValue)
            HRESULT(STDMETHODCALLTYPE* SetValue)(
                __RPC__in IPropertyStore* This,
                /* [in] */ __RPC__in REFPROPERTYKEY key,
                /* [in] */ __RPC__in REFPROPVARIANT propvar);

        DECLSPEC_XFGVIRT(IPropertyStore, Commit)
            HRESULT(STDMETHODCALLTYPE* Commit)(
                __RPC__in IPropertyStore* This);

        END_INTERFACE
    };

    interface IPropertyStore
    {
        CONST_VTBL IPropertyStoreVtbl* lpVtbl;
    };
}