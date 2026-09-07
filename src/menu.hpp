#ifndef MENU_HPP
#define MENU_HPP

#include <Windows.h>
#include <string>

namespace Menu {
    void Init(HWND hwnd);
    void Uninit();
    LRESULT ImplWin32_WndProcHandler(WNDPROC oWndProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void Draw();
    void SetContext(HDC newHdc);
    bool IsOpenMenu();
    bool IsVisible();
    RECT GetClipRect();
}

#endif
