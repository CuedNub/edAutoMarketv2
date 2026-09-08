extern "C" int __mingw_SEH_error_handler(void*, void*, void*, void*) { return 1; }
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>

#include <Windows.h>
#include <ddraw.h>

#include "MinHook.h"
#include "log.hpp"
#include "internal.hpp"
#include "menu.hpp"
#include "config.hpp"

static HMODULE GetRealDDraw() {
    static HMODULE hReal = []() {
        wchar_t path[MAX_PATH];
        GetSystemDirectoryW(path, MAX_PATH);
        std::wstring ddrawPath = std::wstring(path) + L"\\ddraw.dll";
        return LoadLibraryW(ddrawPath.c_str());
    }();
    return hReal;
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawCreate(GUID* lpGUID, LPDIRECTDRAW* lplpDD, IUnknown* pUnkOuter) {
    typedef HRESULT (WINAPI *Fn)(GUID*, LPDIRECTDRAW*, IUnknown*);
    static Fn realFn = (Fn)GetProcAddress(GetRealDDraw(), "DirectDrawCreate");
    return realFn(lpGUID, lplpDD, pUnkOuter);
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawCreateEx(GUID* lpGUID, LPVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter) {
    typedef HRESULT (WINAPI *Fn)(GUID*, LPVOID*, REFIID, IUnknown*);
    static Fn realFn = (Fn)GetProcAddress(GetRealDDraw(), "DirectDrawCreateEx");
    return realFn(lpGUID, lplpDD, iid, pUnkOuter);
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext) {
    typedef HRESULT (WINAPI *Fn)(LPDDENUMCALLBACKA, LPVOID);
    static Fn realFn = (Fn)GetProcAddress(GetRealDDraw(), "DirectDrawEnumerateA");
    return realFn(lpCallback, lpContext);
}

extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawEnumerateW(LPDDENUMCALLBACKW lpCallback, LPVOID lpContext) {
    typedef HRESULT (WINAPI *Fn)(LPDDENUMCALLBACKW, LPVOID);
    static Fn realFn = (Fn)GetProcAddress(GetRealDDraw(), "DirectDrawEnumerateW");
    return realFn(lpCallback, lpContext);
}

typedef HWND (WINAPI *CreateWindowExAFn)(
    DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef HWND (WINAPI *CreateWindowExWFn)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef HRESULT (WINAPI *DirectDrawCreateFn)(GUID*, IDirectDraw**, IUnknown*);
typedef HRESULT (WINAPI *CreateSurfaceFn)(IDirectDraw*, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE*, IUnknown*);
typedef HRESULT (WINAPI *BltFn)(IDirectDrawSurface*, LPCRECT, IDirectDrawSurface*, LPCRECT, DWORD, LPDDBLTFX);

CreateWindowExAFn pCreateWindowExA = nullptr, oCreateWindowExA = nullptr;
CreateWindowExWFn pCreateWindowExW = nullptr, oCreateWindowExW = nullptr;
DirectDrawCreateFn pDirectDrawCreate = nullptr, oDirectDrawCreate = nullptr;
CreateSurfaceFn pCreateSurface = nullptr, oCreateSurface = nullptr;
BltFn pBlt = nullptr, oBlt = nullptr;

HWND WINAPI MyCreateWindowExA(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
HWND WINAPI MyCreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
HRESULT WINAPI MyDirectDrawCreate(GUID*, IDirectDraw**, IUnknown*);
HRESULT STDMETHODCALLTYPE MyCreateSurface(IDirectDraw*, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE*, IUnknown*);
HRESULT WINAPI MyBlt(IDirectDrawSurface*, LPCRECT, IDirectDrawSurface*, LPCRECT, DWORD, LPDDBLTFX);

WNDPROC oWndProc = nullptr;
LRESULT __stdcall MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

GameInterface driver;
HWND g_hwnd = nullptr;

std::chrono::time_point<std::chrono::high_resolution_clock> lastTradeTime;

bool wasInGame = false;

bool ContainsWordIgnoreCase(const std::string& str, const std::string& word) {
    std::string ls = str, lw = word;
    std::transform(ls.begin(), ls.end(), ls.begin(), ::tolower);
    std::transform(lw.begin(), lw.end(), lw.begin(), ::tolower);
    return ls.find(lw) != std::string::npos;
}

void ExecuteTrade() {
    bool currentlyInGame = driver.isInGame();

    if (wasInGame && !currentlyInGame) {
        ConfigManager::Instance().ResetAll();
        wasInGame = false;
        return;
    }

    if (!currentlyInGame) {
        wasInGame = false;
        return;
    }

    wasInGame = true;

    auto now = std::chrono::high_resolution_clock::now();
    int freqMs = ConfigManager::Instance().GetTradeFrequency();
    if (freqMs <= 0) freqMs = 100;

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTradeTime).count() < freqMs)
        return;

    auto& cfg = ConfigManager::Instance();
    const auto& items = cfg.GetItems();
    for (const auto& item : items) {
        int count = driver.getNumberProducts(item.name);
        if (item.saleThreshold > 0 && count > item.saleThreshold)
            driver.sellProduct(item.name);
        if (item.buyThreshold > 0 && count < item.buyThreshold)
            driver.buyProduct(item.name);
    }
    lastTradeTime = std::chrono::high_resolution_clock::now();
}

HRESULT WINAPI MyDirectDrawCreate(GUID* lpGUID, IDirectDraw** lplpDD, IUnknown* pUnkOuter) {
    HRESULT hr = oDirectDrawCreate(lpGUID, lplpDD, pUnkOuter);
    if (!SUCCEEDED(hr) || !lplpDD || !*lplpDD) return hr;

    void** vTable = *reinterpret_cast<void***>(*lplpDD);
    pCreateSurface = (CreateSurfaceFn)vTable[6];

    MH_CreateHook((LPVOID)pCreateSurface, (LPVOID)&MyCreateSurface, reinterpret_cast<void**>(&oCreateSurface));
    MH_EnableHook((LPVOID)pCreateSurface);

    MH_DisableHook((LPVOID)pDirectDrawCreate);
    MH_RemoveHook((LPVOID)pDirectDrawCreate);
    return hr;
}

HRESULT STDMETHODCALLTYPE MyCreateSurface(IDirectDraw* pDD, LPDDSURFACEDESC lpDesc, LPDIRECTDRAWSURFACE* lplpSurf, IUnknown* pUnk) {
    HRESULT hr = oCreateSurface(pDD, lpDesc, lplpSurf, pUnk);
    if (!SUCCEEDED(hr) || !lplpSurf || !*lplpSurf) return hr;
    void** vTable = *reinterpret_cast<void***>(*lplpSurf);
    pBlt = (BltFn)vTable[5];
    if (MH_CreateHook((LPVOID)pBlt, (LPVOID)&MyBlt, reinterpret_cast<void**>(&oBlt)) != MH_OK) return hr;
    if (MH_EnableHook((LPVOID)pBlt) != MH_OK) { MH_RemoveHook((LPVOID)pBlt); return hr; }
    MH_DisableHook((LPVOID)pCreateSurface);
    MH_RemoveHook((LPVOID)pCreateSurface);
    return hr;
}

HRESULT WINAPI MyBlt(IDirectDrawSurface* pDest, LPCRECT lpDestRect, IDirectDrawSurface* pSrc, LPCRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx) {
    ExecuteTrade();

    // Gambar menu pada Backbuffer (pSrc) SEBELUM Blt dilakukan.
    if (Menu::IsVisible()) {
        IDirectDrawSurface* targetSurf = pSrc ? pSrc : pDest;
        if (targetSurf) {
            HDC hDC = nullptr;
            if (SUCCEEDED(targetSurf->GetDC(&hDC)) && hDC) {
                Menu::SetContext(hDC);
                Menu::Draw();
                targetSurf->ReleaseDC(hDC);
            }
        }
    }

    return oBlt(pDest, lpDestRect, pSrc, lpSrcRect, dwFlags, lpDDBltFx);
}

bool MainInit(HWND hwnd) {
    g_hwnd = hwnd;
    wchar_t cwd[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, cwd);
    std::wstring iniPath = std::wstring(cwd) + L"\\automarket.ini";
    
    // Wajib dibaca sebelum Menu::Init
    ConfigManager::Instance().Load(iniPath);
    Menu::Init(g_hwnd);

    lastTradeTime = std::chrono::high_resolution_clock::now();
    wasInGame = false;
    oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MyWndProc);
    return oWndProc != nullptr;
}

HWND WINAPI MyCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = oCreateWindowExA(dwExStyle, lpClassName, lpWindowName,
                                 dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && !g_hwnd) {
        char title[256];
        GetWindowTextA(hwnd, title, sizeof(title));
        if (ContainsWordIgnoreCase(title, "Crusader")) {
            MainInit(hwnd);
            MH_DisableHook((LPVOID)pCreateWindowExW); MH_RemoveHook((LPVOID)pCreateWindowExW);
            MH_DisableHook((LPVOID)pCreateWindowExA); MH_RemoveHook((LPVOID)pCreateWindowExA);
        }
    }
    return hwnd;
}

HWND WINAPI MyCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = oCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
                                 dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && !g_hwnd) {
        wchar_t titleW[256];
        GetWindowTextW(hwnd, titleW, 256);
        char title[256];
        WideCharToMultiByte(CP_ACP, 0, titleW, -1, title, 256, NULL, NULL);
        if (ContainsWordIgnoreCase(title, "Crusader")) {
            MainInit(hwnd);
            MH_DisableHook((LPVOID)pCreateWindowExW); MH_RemoveHook((LPVOID)pCreateWindowExW);
            MH_DisableHook((LPVOID)pCreateWindowExA); MH_RemoveHook((LPVOID)pCreateWindowExA);
        }
    }
    return hwnd;
}

bool SetupHooks() {
    if (MH_Initialize() != MH_OK) return false;

    HMODULE hDdraw = GetRealDDraw();
    if (!hDdraw) return false;

    pDirectDrawCreate = (DirectDrawCreateFn)GetProcAddress(hDdraw, "DirectDrawCreate");
    if (!pDirectDrawCreate) return false;

    if (MH_CreateHook((LPVOID)pDirectDrawCreate, (LPVOID)&MyDirectDrawCreate, reinterpret_cast<void**>(&oDirectDrawCreate)) != MH_OK)
        return false;

    if (MH_EnableHook((LPVOID)pDirectDrawCreate) != MH_OK) return false;

    HMODULE hUser32 = GetModuleHandleA("user32.dll");
    if (!hUser32) return false;

    pCreateWindowExA = (CreateWindowExAFn)GetProcAddress(hUser32, "CreateWindowExA");
    if (pCreateWindowExA) {
        MH_CreateHook((LPVOID)pCreateWindowExA, (LPVOID)&MyCreateWindowExA, reinterpret_cast<void**>(&oCreateWindowExA));
        MH_EnableHook((LPVOID)pCreateWindowExA);
    }

    pCreateWindowExW = (CreateWindowExWFn)GetProcAddress(hUser32, "CreateWindowExW");
    if (pCreateWindowExW) {
        MH_CreateHook((LPVOID)pCreateWindowExW, (LPVOID)&MyCreateWindowExW, reinterpret_cast<void**>(&oCreateWindowExW));
        MH_EnableHook((LPVOID)pCreateWindowExW);
    }

    return true;
}

void CleanupHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

LRESULT __stdcall MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return Menu::ImplWin32_WndProcHandler(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: SetupHooks(); break;
    case DLL_PROCESS_DETACH: CleanupHooks(); break;
    }
    return TRUE;
}
