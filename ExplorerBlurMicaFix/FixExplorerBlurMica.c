/**
 * FixExplorerBlurMica.c - companion fix for ExplorerBlurMica 2.0.1 on Windows 11 24H2/25H2+
 *
 * Copyright (C) 2026 Valor#1337
 *
 * This file is part of a community fix for ExplorerBlurMica and is licensed
 * under the GNU Lesser General Public License v3.0, matching the upstream
 * project license. See <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * Root cause (verified by instrumentation on Windows 11 build 26200):
 *   ExplorerBlurMica applies the effect (DWMWA_SYSTEMBACKDROP_TYPE) only when
 *   the CabinetWClass window is CREATED. On minimize/restore the DWM backdrop
 *   is reset, and on Win11 24H2/25H2 the restored Explorer window sends no
 *   window messages (WM_SHOWWINDOW / WM_WINDOWPOSCHANGED / WM_SIZE ...) to the
 *   subclass, so ExplorerBlurMica's WndProc-based re-apply (UPDATE) never runs
 *   and the window stays fully transparent. See issue #278.
 *
 * Fix: a FolderExtension companion DLL in explorer.exe. A worker thread polls
 *   every 400 ms and re-applies DWMWA_SYSTEMBACKDROP_TYPE for every non-iconic
 *   CabinetWClass window (plus a WinEvent hook as an early trigger), using the
 *   same numeric mapping used by ExplorerBlurMica's accent path:
 *     effect=0 -> 3 (Blur)        effect=1 -> 4 (Acrylic/Tabbed)
 *     effect=2 -> 2 (Mica)        effect=3 -> 4 (Blur Clear)
 *     effect=4 -> 5 (best-effort; not covered by issue #278 testing)
 *   effect=1 is the configuration verified on Windows 11 build 26200.
 *
 * Build: see BuildAndRegister.ps1 (mingw-w64 g++).
 * Register without admin: HKCU\SOFTWARE\Classes (see BuildAndRegister.ps1).
 * Register with admin:    regsvr32 FixExplorerBlurMica.dll
 */

#include <windows.h>
#include <winreg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <shlwapi.h>

static wchar_t g_logPath[MAX_PATH + 32] = {0};

static void log_line(const wchar_t* fmt, ...)
{
    if (!g_logPath[0]) return;
    FILE* f = _wfopen(g_logPath, L"a, ccs=UTF-8");
    if (!f) return;
    va_list ap;
    va_start(ap, fmt);
    fwprintf(f, L"[%llu] ", (unsigned long long)GetTickCount64());
    vfwprintf(f, fmt, ap);
    fwprintf(f, L"\n");
    va_end(ap);
    fclose(f);
}

#define CLSID_FIX_STR L"{48E13E56-7F01-4B8A-9C32-5A0E3F0D2B71}"

// DWMWA_SYSTEMBACKDROP_TYPE (38) values
#define DWMSBT_NONE              0
#define DWMSBT_AUTO              1
#define DWMSBT_MAINWINDOW        2   // Mica
#define DWMSBT_TRANSIENTWINDOW   3   // Acrylic
#define DWMSBT_TABBEDWINDOW      4   // Tabbed/Acrylic
#define DWMSBT_MAINWINDOW_ALT    5   // Mica Alt
#define DWMSBT_BORDERLESS        6

static HMODULE g_hDll = NULL;
static HANDLE g_thread = NULL;
static volatile LONG g_running = 0;

typedef int(WINAPI* pfn_DwmSetWindowAttribute)(HWND, DWORD, const void*, DWORD);

static pfn_DwmSetWindowAttribute g_pDwmSetWindowAttribute = NULL;

/* ---------- config ---------- */
typedef struct {
    int effect;
    int clearAddress, clearBarBg, clearWinUIBg, showLine;
} FIX_CONFIG;

static FIX_CONFIG g_cfg = { 1, 1, 1, 1, 1 };

static int ini_get_int(const wchar_t* section, const wchar_t* key, int def, const wchar_t* path)
{
    wchar_t buf[64];
    DWORD r = GetPrivateProfileStringW(section, key, L"", buf, 64, path);
    if (r == 0) return def;
    return _wtoi(buf);
}

static void load_config(void)
{
    wchar_t path[MAX_PATH + 32];
    GetModuleFileNameW(g_hDll, path, MAX_PATH);
    wchar_t* slash = wcsrchr(path, L'\\');
    if (slash) wcscpy(slash + 1, L"config.ini");
    g_cfg.effect = ini_get_int(L"config", L"effect", 1, path);
}

/* ---------- effect application ---------- */
static int backdrop_for_effect(int effect)
{
    switch (effect) {
    case 0: return DWMSBT_TRANSIENTWINDOW;   // Blur -> acrylic-ish
    case 1: return DWMSBT_TABBEDWINDOW;      // Acrylic
    case 2: return DWMSBT_MAINWINDOW;        // Mica
    case 3: return DWMSBT_TABBEDWINDOW;      // Blur(Clear)
    case 4: return DWMSBT_MAINWINDOW_ALT;    // MicaAlt
    default: return DWMSBT_TABBEDWINDOW;
    }
}

static HRESULT apply_effect(HWND hwnd)
{
    if (!g_pDwmSetWindowAttribute || !IsWindow(hwnd)) return E_FAIL;
    int backdrop = backdrop_for_effect(g_cfg.effect);
    return (HRESULT)g_pDwmSetWindowAttribute(hwnd, 38, &backdrop, sizeof(backdrop));
}

/* ---------- watch logic ---------- */
#define MAX_WATCH 64
static struct { HWND hwnd; BOOL iconic; } s_watch[MAX_WATCH];
static int s_watchCount = 0;
static CRITICAL_SECTION g_cs;
static volatile LONG g_seen = 0;

static BOOL is_cabinet(HWND h)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return FALSE; // avoid cross-process GetClassNameW hangs
    wchar_t cls[128];
    return GetClassNameW(h, cls, 128) && wcscmp(cls, L"CabinetWClass") == 0;
}

// returns TRUE when hwnd just transitioned iconic->normal
static BOOL check_transition(HWND h)
{
    BOOL iconic = IsIconic(h);
    for (int i = 0; i < s_watchCount; i++) {
        if (s_watch[i].hwnd == h) {
            BOOL was = s_watch[i].iconic;
            s_watch[i].iconic = iconic;
            return was && !iconic;
        }
    }
    if (s_watchCount < MAX_WATCH) {
        s_watch[s_watchCount].hwnd = h;
        s_watch[s_watchCount].iconic = iconic;
        s_watchCount++;
    }
    return FALSE;
}

static BOOL CALLBACK enum_check(HWND h, LPARAM)
{
    if (!is_cabinet(h)) return TRUE;
    InterlockedIncrement(&g_seen);
    EnterCriticalSection(&g_cs);
    BOOL iconic = IsIconic(h);
    BOOL restored = check_transition(h);
    if (!iconic) {
        // Robust: re-apply backdrop on every poll for every non-iconic cabinet.
        // Idempotent (same attribute value) and covers restore even if the
        // iconic->normal transition was never observed (missed poll window).
        HRESULT hr = apply_effect(h);
        static DWORD lastLog = 0;
        DWORD now = GetTickCount();
        if (restored || (now - lastLog) > 5000) {
            log_line(L"apply 0x%p iconic=0 restored=%d hr=0x%08x effect=%d backdrop=%d",
                     (void*)h, restored, hr, g_cfg.effect, backdrop_for_effect(g_cfg.effect));
            lastLog = now;
        }
    }
    LeaveCriticalSection(&g_cs);
    return TRUE;
}

// WinEvent hook as an additional early trigger
static void CALLBACK win_event_proc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                    LONG idObject, LONG idChild, DWORD eventThread, DWORD eventTime)
{
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (!hwnd || !IsWindow(hwnd) || !is_cabinet(hwnd)) return;
    if (event == EVENT_SYSTEM_MINIMIZEEND || event == EVENT_SYSTEM_FOREGROUND) {
        EnterCriticalSection(&g_cs);
        if (check_transition(hwnd)) {
            log_line(L"event 0x%x restore 0x%p -> reapply", (unsigned)event, (void*)hwnd);
            for (int i = 0; i < 4; i++) {
                apply_effect(hwnd);
                Sleep(120);
            }
        }
        LeaveCriticalSection(&g_cs);
    } else if (event == EVENT_SYSTEM_MINIMIZESTART) {
        EnterCriticalSection(&g_cs);
        if (check_transition(hwnd)) log_line(L"event minimize 0x%p", (void*)hwnd);
        LeaveCriticalSection(&g_cs);
    }
}

static HWINEVENTHOOK s_hook = NULL;

static DWORD WINAPI worker_thread(LPVOID)
{
    GetModuleFileNameW(g_hDll, g_logPath, MAX_PATH);
    wchar_t* slash = wcsrchr(g_logPath, L'\\');
    if (slash) wcscpy(slash + 1, L"fix_debug.log");
    log_line(L"=== worker_thread start pid=%lu ===", (unsigned long)GetCurrentProcessId());

    load_config();
    log_line(L"config effect=%d", g_cfg.effect);

    HMODULE hDwm = GetModuleHandleW(L"dwmapi.dll");
    if (!hDwm) hDwm = LoadLibraryW(L"dwmapi.dll");
    if (hDwm) {
        g_pDwmSetWindowAttribute = (pfn_DwmSetWindowAttribute)GetProcAddress(
            hDwm, "DwmSetWindowAttribute");
    }
    log_line(L"dwmapi=0x%p pfn=0x%p", (void*)hDwm, (void*)g_pDwmSetWindowAttribute);
    if (!g_pDwmSetWindowAttribute) return 0;

    // apply to existing windows (covers windows already open)
    EnumWindows([](HWND h, LPARAM) -> BOOL {
        if (is_cabinet(h)) {
            HRESULT hr = apply_effect(h);
            log_line(L"startup apply 0x%p hr=0x%08x", (void*)h, hr);
        }
        return TRUE;
    }, 0);

    // event-driven early trigger (best effort)
    s_hook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
                             NULL, win_event_proc, 0, 0,
                             WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    log_line(L"winevent hook=0x%p", (void*)s_hook);

    // reliable poll: re-apply every 400ms for every non-iconic cabinet
    static DWORD lastBeat = 0;
    while (g_running) {
        Sleep(400);
        InterlockedExchange(&g_seen, 0);
        EnumWindows(enum_check, 0);
        DWORD now = GetTickCount();
        if (now - lastBeat > 5000) {
            log_line(L"heartbeat seen=%d", (int)g_seen);
            lastBeat = now;
        }
    }
    log_line(L"worker_thread exit");
    return 0;
}

/* ---------- COM shell (FolderExtension) ---------- */
#include <objbase.h>

struct CFImpl : public IClassFactory {
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = NULL;
        if (riid == IID_IUnknown || riid == IID_IClassFactory) { *ppv = this; return S_OK; }
        return E_NOINTERFACE;
    }
    virtual ULONG STDMETHODCALLTYPE AddRef() override { return 2; }
    virtual ULONG STDMETHODCALLTYPE Release() override { return 1; }
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) override {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        return QueryInterface(riid, ppv);
    }
    virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override { return S_OK; }
};
static CFImpl g_cf;

extern "C" {
__declspec(dllexport) HRESULT WINAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (riid == IID_IUnknown || riid == IID_IClassFactory) {
        *ppv = &g_cf;
        return S_OK;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

__declspec(dllexport) HRESULT WINAPI DllCanUnloadNow(void) { return S_FALSE; }

static LONG WINAPI RegKey(const wchar_t* path, const wchar_t* name, const wchar_t* value)
{
    HKEY hk = NULL;
    LONG r = RegCreateKeyExW(HKEY_CLASSES_ROOT, path, 0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL);
    if (r != ERROR_SUCCESS) return r;
    r = RegSetValueExW(hk, name, 0, REG_SZ, (const BYTE*)value, (DWORD)((wcslen(value) + 1) * 2));
    RegCloseKey(hk);
    return r;
}

__declspec(dllexport) HRESULT WINAPI DllRegisterServer(void)
{
    wchar_t dll[MAX_PATH];
    GetModuleFileNameW(g_hDll, dll, MAX_PATH);
    wchar_t key[512], sub[600];
    wsprintfW(key, L"CLSID\\%s", CLSID_FIX_STR);
    RegKey(key, NULL, L"FixExplorerBlurMica");
    wsprintfW(sub, L"CLSID\\%s\\InprocServer32", CLSID_FIX_STR);
    RegKey(sub, NULL, dll);
    RegKey(sub, L"ThreadingModel", L"Apartment");
    wsprintfW(key, L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\%s", CLSID_FIX_STR);
    HKEY hk = NULL;
    LONG r = RegCreateKeyExW(HKEY_LOCAL_MACHINE, key, 0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL);
    if (r == ERROR_SUCCESS) {
        DWORD mask = 255;
        RegSetValueExW(hk, L"DriveMask", 0, REG_DWORD, (const BYTE*)&mask, sizeof(mask));
        RegCloseKey(hk);
    }
    return r == ERROR_SUCCESS ? S_OK : HRESULT_FROM_WIN32(r);
}

__declspec(dllexport) HRESULT WINAPI DllUnregisterServer(void)
{
    wchar_t key[512];
    wsprintfW(key, L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\%s", CLSID_FIX_STR);
    RegDeleteKeyW(HKEY_LOCAL_MACHINE, key);
    wsprintfW(key, L"CLSID\\%s", CLSID_FIX_STR);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, key);
    return S_OK;
}
} // extern "C"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) {
        g_hDll = hModule;
        InitializeCriticalSection(&g_cs);
        if (InterlockedCompareExchange(&g_running, 1, 0) == 0) {
            g_thread = CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
        }
        DisableThreadLibraryCalls(hModule);
    } else if (reason == DLL_PROCESS_DETACH) {
        InterlockedExchange(&g_running, 0);
        if (s_hook) UnhookWinEvent(s_hook);
        if (g_thread) { WaitForSingleObject(g_thread, 2000); CloseHandle(g_thread); }
        DeleteCriticalSection(&g_cs);
    }
    return TRUE;
}
