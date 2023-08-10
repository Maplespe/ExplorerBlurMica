/*
* WinAPI助手
*
* Author: Maple
* date: 2021-7-13 Create
* Copyright winmoes.com
*/
#include "Helper.h"
#include <algorithm>
#include <ShellScalingApi.h>

std::wstring GetCurDllDir()
{
	wchar_t sPath[MAX_PATH];
	GetModuleFileNameW(g_hModule, sPath, MAX_PATH);
	std::wstring path = sPath;
	path = path.substr(0, path.rfind(L"\\"));

	return path;
}

bool FileIsExist(std::wstring FilePath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFileW(FilePath.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		return true;
	}
	return false;
}

std::wstring GetWindowTitle(HWND hWnd)
{
	int size = GetWindowTextLengthW(hWnd) + 1;
	wchar_t* pText = new wchar_t[size];
	GetWindowTextW(hWnd, pText, size);
	return pText;
}

std::wstring GetWindowClassName(HWND hWnd)
{
	wchar_t* pText = new wchar_t[MAX_PATH];
	GetClassNameW(hWnd, pText, MAX_PATH);
	return pText;
}

std::wstring GetIniString(const std::wstring& FilePath, const std::wstring& AppName, const std::wstring& KeyName)
{
	if (FileIsExist(FilePath)) 
	{
		HANDLE pFile = CreateFileW(FilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		LARGE_INTEGER fileSize;
		GetFileSizeEx(pFile, &fileSize);

		wchar_t* data = new wchar_t[fileSize.QuadPart];
		ZeroMemory(data, sizeof(wchar_t) * fileSize.QuadPart);
		GetPrivateProfileStringW(AppName.c_str(), KeyName.c_str(), nullptr, data, (DWORD)fileSize.QuadPart, FilePath.c_str());

		std::wstring ret = data;
		delete[] data;

		CloseHandle(pFile);
		return ret;
	}
	return {};
}

std::wstring ConvertTolower(std::wstring str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

struct ACCENTPOLICY
{
	int nAccentState;
	int nFlags;
	int nColor;
	int nAnimationId;
};

struct WINCOMPATTRDATA
{
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
};

typedef BOOL(WINAPI* PSWCA)(HWND, WINCOMPATTRDATA*);
const auto SetWindowCompositionAttribute = (PSWCA)GetProcAddress(LoadLibraryW(L"user32.dll"), "SetWindowCompositionAttribute");

void StartAero(HWND hwnd, int type, COLORREF color, bool blend)
{
	if (SetWindowCompositionAttribute)
	{
		//ACCENT_ENABLE_BLURBEHIND 3
		//ACCENT_ENABLE_ACRYLICBLURBEHIND 4
		ACCENTPOLICY policy = { type == 0 ? 3 : 4, 0, 0, 0 };
		if (type == -1)
			policy.nAccentState = 0;//ACCENT_DISABLED
		if (blend)
		{
			policy.nFlags = 3;
			policy.nColor = (int)color;
		}
		else if (type == 1)
		{
			policy.nFlags = 1;
			policy.nColor = 0x00FFFFFF;
		}
		else
		{
			policy.nFlags = 0;
			policy.nColor = 0;
		}
		if(type > 0 || (type == 0 && M_GetAValue(color) == 0))
			policy.nFlags |= 3584;

		WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
		SetWindowCompositionAttribute(hwnd, &data);
	}
}

std::wstring GetThemeClassName(HTHEME hTheme)
{
	typedef HRESULT(WINAPI* GetThemeClass)(HTHEME, LPCTSTR, int);
	static auto getname = (GetThemeClass)GetProcAddress(GetModuleHandleW(L"uxtheme"), MAKEINTRESOURCEA(74));

	std::wstring ret;
	if (getname)
	{
		wchar_t buffer[255] = { 0 };
		HRESULT hr = getname(hTheme, buffer, 255);
		return SUCCEEDED(hr) ? buffer : L"";
	}
	return ret;
}

HMODULE GetContainingModule(_In_ PVOID pvAddr)
{
	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory(&mbi, sizeof(mbi));

	__try {
		if (VirtualQuery(pvAddr, &mbi, sizeof(mbi)) <= 0) {
			SetLastError(ERROR_BAD_EXE_FORMAT);
			return NULL;
		}

		// Skip uncommitted regions and guard pages.
		//
		if ((mbi.State != MEM_COMMIT) ||
			((mbi.Protect & 0xff) == PAGE_NOACCESS) ||
			(mbi.Protect & PAGE_GUARD)) {
			SetLastError(ERROR_BAD_EXE_FORMAT);
			return NULL;
		}

		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)mbi.AllocationBase;
		if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			SetLastError(ERROR_BAD_EXE_FORMAT);
			return NULL;
		}

		PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader +
			pDosHeader->e_lfanew);
		if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
			SetLastError(ERROR_INVALID_EXE_SIGNATURE);
			return NULL;
		}
		if (pNtHeader->FileHeader.SizeOfOptionalHeader == 0) {
			SetLastError(ERROR_EXE_MARKED_INVALID);
			return NULL;
		}
		SetLastError(NO_ERROR);

		return (HMODULE)pDosHeader;
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
		SetLastError(ERROR_INVALID_EXE_SIGNATURE);
		return NULL;
	}
}

bool CheckCaller(HMODULE caller, void* address)
{
	HMODULE modules = GetContainingModule(address);
	return caller == modules;
}

bool CheckCaller(LPCWSTR caller, void* address)
{
	return CheckCaller(GetModuleHandleW(caller), address);
}

UINT CalcRibbonHeightForDPI(HWND hWnd, UINT src, bool normal, bool offsets)
{
	static auto GetWindowDPI = [](HWND hwnd) -> UINT
	{
		typedef UINT(WINAPI* pfnGetWindowDPI)(HWND);
		static const auto& GetWindowDPI = (pfnGetWindowDPI)GetProcAddress(GetModuleHandleW(L"User32"), MAKEINTRESOURCEA(2707));
		if (GetWindowDPI)
		{
			return GetWindowDPI(hwnd);
		}
		HDC hdc = GetDC(nullptr);
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(0, hdc);
		return (UINT)dpi;
	};

	// From UIRibbon.dll
	static auto MsoScaleForWindowDPI = [](HWND hwnd)
	{
		return fmaxf(1.0, (float)GetWindowDPI(hwnd) / 96.f);
	};

	float scale = MsoScaleForWindowDPI(hWnd);
	if (scale != 1.f) {
		if (!normal) {
			float offset = round(1.5f * scale);
			float height = round((float)src * scale);
			if (offsets)
				height -= offset;
			else if (scale == 2.f)
				height += 2;
			else if(scale < 1.7f)
				height -= 1;

			return (UINT)height;
		}
		else
		{
			return (UINT)round(scale * (float)src);
		}
	}
	return src;
}

bool EnablePriviledge(LPCTSTR lpSystemName)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp = { 1 };
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (LookupPrivilegeValue(nullptr, lpSystemName, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)nullptr, nullptr);
			if (GetLastError() != ERROR_SUCCESS)
			{
				CloseHandle(hToken);
				return false;
			}
		}
		CloseHandle(hToken);
	}
	return true;
}

std::wstring RegGetSZ(HKEY hKey, LPCWSTR SubKey, LPCWSTR KeyName)
{
	std::wstring ret;
	HKEY tkey = 0;

	LSTATUS status = RegOpenKeyExW(hKey, SubKey, 0, KEY_READ, &tkey);

	//以备份还原级别权限再重试
	if (status != ERROR_SUCCESS)
	{
		EnablePriviledge(SE_BACKUP_NAME);
		EnablePriviledge(SE_RESTORE_NAME);
		DWORD dwDisposition;
		status = RegCreateKeyExW(hKey,
			SubKey,
			0,
			NULL,
			REG_OPTION_BACKUP_RESTORE,
			KEY_ALL_ACCESS | KEY_WOW64_64KEY,
			NULL,
			&tkey,
			&dwDisposition);
	}

	if (status == ERROR_SUCCESS)
	{
		wchar_t dwValue[MAX_PATH];
		DWORD dwSzType = REG_SZ;
		DWORD dwSize = sizeof(dwValue);
		if (RegQueryValueExW(tkey, KeyName, 0, &dwSzType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS)
		{
			ret = L"";
		}
		else
			ret = dwValue;
	}
	if (tkey)
		RegCloseKey(tkey);
	return ret;
}

HRESULT WINAPI GetDpiForMonitor_(
	_In_ HMONITOR hmonitor,
	_In_ MONITOR_DPI_TYPE dpiType,
	_Out_ UINT* dpiX,
	_Out_ UINT* dpiY)
{
	HINSTANCE hInstWinSta = LoadLibraryW(L"SHCore.dll");
	if (hInstWinSta == nullptr) return E_NOINTERFACE;

	typedef HRESULT(WINAPI* PFN_GDFM)(
		HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

	PFN_GDFM pGetDpiForMonitor =
		(PFN_GDFM)GetProcAddress(hInstWinSta, "GetDpiForMonitor");
	if (pGetDpiForMonitor == nullptr) return E_NOINTERFACE;

	return pGetDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
}


void RefreshWin10BlurFrame(bool blurType)
{
	if (g_sysBuildNumber < 22000) //Windows 10
	{
		std::wstring width = RegGetSZ(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\WindowMetrics", L"PaddedBorderWidth");
		int updateWidth = 0;
		if (!blurType && width == L"0")
			updateWidth = 1;
		else if (blurType && width != L"0")
			updateWidth = 2;
		if (updateWidth != 0)
		{
			//DPI
			HWND hWnd = GetDesktopWindow();
			HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			UINT dpiX = 96;
			UINT dpiY = 96;
			GetDpiForMonitor_(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

			auto setreg = [dpiX](std::wstring value)
			{
				HKEY hKey = nullptr;
				LSTATUS ret = RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\WindowMetrics", 0, KEY_WRITE, &hKey);
				if (ret == ERROR_SUCCESS) {
					RegSetValueExW(hKey, L"PaddedBorderWidth", NULL, REG_SZ, (LPBYTE)value.c_str(), sizeof(wchar_t) * value.length());
					RegCloseKey(hKey);

					NONCLIENTMETRICS metrics = { 0 };
					metrics.cbSize = sizeof(metrics);
					SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &metrics, 0, dpiX);
					int size = _wtoi(value.c_str());
					metrics.iPaddedBorderWidth = size == 0 ? 0 : (-size) / 15;

					auto proc = [](LPVOID param) -> DWORD
					{
						SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), param, 0);
						return 0;
					};

					DWORD threadID = 0;
					auto threads = CreateThread(nullptr, 0, proc, &metrics, 0, &threadID);
					if (threads)
					{
						if (WaitForSingleObject(threads, 2000) != WAIT_OBJECT_0)
						{
							TerminateThread(threads, -1);
						}
						CloseHandle(threads);
					}

					//DWORD dwInfo = BSM_APPLICATIONS;
					//BroadcastSystemMessageW(BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE | BSF_FORCEIFHUNG, &dwInfo, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 0);
				}
			};
			if (updateWidth == 1)
				setreg(L"-60");
			else if (updateWidth == 2)
				setreg(L"0");
		}
	}
}

//Exported entry 126. uxtheme.dll
HRESULT DrawTextWithGlow
(
	HDC hdcMem,
	LPCWSTR pszText,
	UINT cch,
	const RECT* prc,
	DWORD dwFlags,
	COLORREF crText,
	COLORREF crGlow,
	UINT nGlowRadius,
	UINT nGlowIntensity,
	BOOL fPreMultiply,
	DTT_CALLBACK_PROC pfnDrawTextCallback,
	LPARAM lParam
)
{
	typedef HRESULT(WINAPI* function)(HDC, LPCWSTR, UINT, const RECT*, DWORD,
		COLORREF, COLORREF, UINT, UINT, BOOL, DTT_CALLBACK_PROC, LPARAM);
	static HMODULE hModuele = GetModuleHandleW(L"uxtheme.dll");
	if (hModuele)
	{
		static function pfun = (function)GetProcAddress(hModuele, MAKEINTRESOURCEA(126));
		return pfun(hdcMem, pszText, cch, prc, dwFlags, crText, crGlow, nGlowRadius,
			nGlowIntensity, fPreMultiply, pfnDrawTextCallback, lParam);
	}
	return E_NOTIMPL;
}

HRESULT DwmUpdateAccentBlurRect(HWND hWnd, RECT* prc)
{
	typedef HRESULT(WINAPI* function)(HWND, RECT*);
	static HMODULE hModule = GetModuleHandleW(L"dwmapi.dll");
	if(hModule)
	{
		static function pfun = (function)GetProcAddress(hModule, MAKEINTRESOURCEA(159));
		return pfun(hWnd, prc);
	}
	return E_NOTIMPL;
}

bool SysIsLightMode()
{
	DWORD value = 0;
	DWORD size = sizeof(value);
	auto state = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppsUseLightTheme",
		RRF_RT_REG_DWORD,
		nullptr,
		&value,
		&size);
	if (state == ERROR_SUCCESS)
		return value == 1;
	return true;
}
