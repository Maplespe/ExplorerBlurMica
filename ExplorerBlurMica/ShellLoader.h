/*
* BHO组件加载器
*
* Author: Maple
* date: 2022-1-31 Create
* Copyright winmoes.com
*/
#pragma once
#include "framework.h"
#include <Unknwn.h>
#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>
#include <string>

extern const std::wstring CLSID_SHELL_BHO_STR;
extern const CLSID CLSID_SHELL_BHO;

extern void OnWindowLoad();

/*
* 一个基本的COM浏览器类 只实现基本接口
*/
class CObjectWithSite : public IObjectWithSite
{
public:
	CObjectWithSite();
	virtual ~CObjectWithSite();

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IObjectWithSite
	STDMETHODIMP SetSite(IUnknown* pUnkSite);
	STDMETHODIMP GetSite(REFIID riid, void** ppvSite) { return E_NOINTERFACE; }

protected:
	long m_ref = 0;
	IWebBrowser2* m_web = nullptr;

	void ReleaseRes();
};

/*
* 类工厂 用来处理浏览器类的创建
*/
class ClassFactory : public IClassFactory
{
public:

	//IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IClassFactory
	IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, LPVOID* ppvObj);
	IFACEMETHODIMP LockServer(BOOL fLock);

private:
	long m_ref = 0;
};