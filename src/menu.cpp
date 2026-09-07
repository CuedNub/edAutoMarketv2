#include <vector>
#include <string>
#include <chrono>
#include <windowsx.h>
#include "Menu.hpp"
#include "config.hpp"

namespace Menu {
    HBRUSH hBgBrush, hHeaderBrush, hRowBrush, hSelBrush, hEditBrush, hCatBrush, hNotifBrush;
    HFONT hTitleFont, hFont, hSmallFont;
    HDC hdc;
    HWND g_hwnd;

    const int MENU_W = 400;
    const int ROW_H = 22;
    const int TITLE_H = 30;
    const int HEADER_H = 24;
    const int CAT_H = 22;
    const int FOOTER_H = 24;
    const int COL1_W = 175;
    const int COL2_W = 105;

    int offsetX, offsetY;
    int screenW, screenH;
    RECT menuCanvas;

    bool isMenuActive;
    int selectedRow;
    int selectedCol;
    bool isEditing;
    std::wstring editBuffer;

    std::wstring notifText;
    std::chrono::time_point<std::chrono::high_resolution_clock> notifEnd;
    bool hasNotif;
    bool pendingClose;

    bool resetConfirm;
    std::chrono::time_point<std::chrono::high_resolution_clock> resetTime;

    int GetMenuHeight() {
        auto& cfg = ConfigManager::Instance();
        int wc = cfg.GetWeaponCount();
        int rc = (int)cfg.GetItems().size() - wc;
        return TITLE_H + HEADER_H + CAT_H + wc * ROW_H + CAT_H + rc * ROW_H + FOOTER_H;
    }

    void Init(HWND hwnd) {
        hBgBrush     = CreateSolidBrush(RGB(35, 30, 25));
        hHeaderBrush = CreateSolidBrush(RGB(55, 45, 35));
        hRowBrush    = CreateSolidBrush(RGB(45, 38, 30));
        hSelBrush    = CreateSolidBrush(RGB(75, 60, 40));
        hEditBrush   = CreateSolidBrush(RGB(90, 80, 40));
        hCatBrush    = CreateSolidBrush(RGB(60, 50, 35));
        hNotifBrush  = CreateSolidBrush(RGB(40, 80, 40));

        hTitleFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, FIXED_PITCH, TEXT("Consolas"));
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, FIXED_PITCH, TEXT("Consolas"));
        hSmallFont = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, FIXED_PITCH, TEXT("Consolas"));

        g_hwnd = hwnd;
        isMenuActive = false;
        selectedRow = 0;
        selectedCol = 0;
        isEditing = false;
        hasNotif = false;
        pendingClose = false;
        resetConfirm = false;
    }

    void Uninit() {
        DeleteObject(hBgBrush);     DeleteObject(hHeaderBrush);
        DeleteObject(hRowBrush);    DeleteObject(hSelBrush);
        DeleteObject(hEditBrush);   DeleteObject(hCatBrush);
        DeleteObject(hNotifBrush);  DeleteObject(hTitleFont);
        DeleteObject(hFont);        DeleteObject(hSmallFont);
    }

    void FR(RECT r, HBRUSH b) {
        OffsetRect(&r, offsetX, offsetY);
        FillRect(hdc, &r, b);
    }

    void DT(RECT r, HFONT f, const wchar_t* t, int fmt, COLORREF c) {
        OffsetRect(&r, offsetX, offsetY);
        HFONT old = (HFONT)SelectObject(hdc, f);
        SetTextColor(hdc, c);
        SetBkMode(hdc, TRANSPARENT);
        DrawTextW(hdc, t, -1, &r, fmt | DT_NOPREFIX);
        SelectObject(hdc, old);
    }

    void ShowNotif(const std::wstring& text, int ms) {
        notifText = text;
        notifEnd = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(ms);
        hasNotif = true;
    }

    void ProcessNotif() {
        if (!hasNotif) return;
        if (std::chrono::high_resolution_clock::now() >= notifEnd) {
            hasNotif = false;
            if (pendingClose) {
                isMenuActive = false;
                pendingClose = false;
                isEditing = false;
            }
        }
    }

    void StartEdit() {
        auto& items = ConfigManager::Instance().GetItems();
        if (selectedRow < 0 || selectedRow >= (int)items.size()) return;
        isEditing = true;
        editBuffer = L"";
    }

    void ApplyEdit() {
        if (!isEditing) return;
        auto& cfg = ConfigManager::Instance();
        int val = editBuffer.empty() ? 0 : std::stoi(editBuffer);
        if (val < 0) val = 0;
        if (val > 9999) val = 9999;
        if (selectedCol == 0) cfg.SetItemSale(selectedRow, val);
        else cfg.SetItemBuy(selectedRow, val);
        isEditing = false;
        editBuffer = L"";
    }

    void CancelEdit() {
        isEditing = false;
        editBuffer = L"";
    }

    LRESULT ImplWin32_WndProcHandler(WNDPROC oWndProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_KEYDOWN) {
            if (lParam & 0x40000000)
                return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

            auto& cfg = ConfigManager::Instance();

            if (ConfigManager::CheckHotkey(cfg.GetToggleMenu())) {
                if (isEditing) CancelEdit();
                isMenuActive = !isMenuActive;
                return 0;
            }

            if (ConfigManager::CheckHotkey(cfg.GetReloadConfig())) {
                cfg.Load(cfg.GetIniPath());
                if (isMenuActive) ShowNotif(L"Config Reloaded!", 1200);
                return 0;
            }

            if (ConfigManager::CheckHotkey(cfg.GetLoadSnapshot())) {
                cfg.LoadSnapshot();
                if (isMenuActive) ShowNotif(L"Loaded!", 1200);
                return 0;
            }

            if (!isMenuActive)
                return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

            if (ConfigManager::CheckHotkey(cfg.GetSaveConfig())) {
                ApplyEdit();
                cfg.SaveSnapshot();
                ShowNotif(L"Saved!", 1500);
                pendingClose = true;
                return 0;
            }

            if (ConfigManager::CheckHotkey(cfg.GetResetAll())) {
                auto now = std::chrono::high_resolution_clock::now();
                if (resetConfirm &&
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - resetTime).count() < 2000) {
                    cfg.ResetAll();
                    resetConfirm = false;
                    ShowNotif(L"Reset!", 1200);
                } else {
                    resetConfirm = true;
                    resetTime = now;
                    ShowNotif(L"Press CTRL+R again to confirm", 2000);
                }
                return 0;
            }

            int total = (int)cfg.GetItems().size();

            if (isEditing) {
                if (wParam >= '0' && wParam <= '9') {
                    if (editBuffer.length() < 4) editBuffer += (wchar_t)wParam;
                    return 0;
                }
                if (wParam == VK_BACK) {
                    if (!editBuffer.empty()) editBuffer.pop_back();
                    return 0;
                }
                if (wParam == VK_DELETE) { editBuffer.clear(); return 0; }
                if (wParam == VK_RETURN) { ApplyEdit(); return 0; }
                if (wParam == VK_ESCAPE) { CancelEdit(); return 0; }
                if (wParam == VK_TAB)    { ApplyEdit(); selectedCol = 1 - selectedCol; return 0; }
            } else {
                if (wParam == VK_UP)   { if (selectedRow > 0) selectedRow--; return 0; }
                if (wParam == VK_DOWN) { if (selectedRow < total - 1) selectedRow++; return 0; }
                if (wParam == VK_TAB)  { selectedCol = 1 - selectedCol; return 0; }
                if (wParam >= '0' && wParam <= '9') {
                    StartEdit();
                    editBuffer += (wchar_t)wParam;
                    return 0;
                }
                if (wParam == VK_ESCAPE) { isMenuActive = false; return 0; }
            }
        }
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    void DrawRow(int idx, int cy, const std::wstring& displayName, int sell, int buy) {
        bool sel = (idx == selectedRow);
        FR({0, cy, MENU_W, cy + ROW_H}, sel ? hSelBrush : hRowBrush);
        std::wstring prefix = sel ? L"> " : L"  ";
        DT({5, cy, COL1_W, cy + ROW_H}, hFont, (prefix + displayName).c_str(),
           DT_LEFT | DT_VCENTER | DT_SINGLELINE,
           sel ? RGB(255, 255, 200) : RGB(220, 200, 160));
        for (int c = 0; c < 2; c++) {
            int cx = (c == 0) ? COL1_W : COL1_W + COL2_W;
            int cw = (c == 0) ? COL2_W : MENU_W - COL1_W - COL2_W;
            bool isCell = sel && (selectedCol == c);
            bool isEdit = isCell && isEditing;
            if (isCell)
                FR({cx, cy, cx + cw, cy + ROW_H}, isEdit ? hEditBrush : hSelBrush);
            std::wstring valStr;
            if (isEdit) valStr = L"[" + editBuffer + L"_]";
            else valStr = std::to_wstring(c == 0 ? sell : buy);
            DT({cx, cy, cx + cw, cy + ROW_H}, hFont, valStr.c_str(),
               DT_CENTER | DT_VCENTER | DT_SINGLELINE,
               isCell ? RGB(255, 255, 100) : RGB(200, 180, 140));
        }
    }

    void Draw() {
        ProcessNotif();
        if (!isMenuActive && !hasNotif) return;

        auto& cfg = ConfigManager::Instance();
        const auto& items = cfg.GetItems();
        int wc = cfg.GetWeaponCount();
        int menuH = GetMenuHeight();

        RECT wr;
        GetClientRect(g_hwnd, &wr);
        screenW = wr.right - wr.left;
        screenH = wr.bottom - wr.top;
        offsetX = screenW / 2 - MENU_W / 2;
        offsetY = screenH / 2 - menuH / 2;
        menuCanvas = {0, 0, MENU_W, menuH};

        FR(menuCanvas, hBgBrush);
        int cy = 0;

        DT({0, cy, MENU_W, cy + TITLE_H}, hTitleFont, L"edAutoMarket v2.0",
           DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(255, 220, 130));
        cy += TITLE_H;

        FR({0, cy, MENU_W, cy + HEADER_H}, hHeaderBrush);
        DT({5, cy, COL1_W, cy + HEADER_H}, hFont, L"Item",
           DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(180, 170, 150));
        DT({COL1_W, cy, COL1_W + COL2_W, cy + HEADER_H}, hFont, L"Sell >",
           DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(180, 170, 150));
        DT({COL1_W + COL2_W, cy, MENU_W - 5, cy + HEADER_H}, hFont, L"Buy <",
           DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(180, 170, 150));
        cy += HEADER_H;

        FR({0, cy, MENU_W, cy + CAT_H}, hCatBrush);
        DT({5, cy, MENU_W, cy + CAT_H}, hFont, L"-- Weapons --",
           DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(255, 150, 100));
        cy += CAT_H;

        for (int i = 0; i < wc && i < (int)items.size(); i++) {
            DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold);
            cy += ROW_H;
        }

        FR({0, cy, MENU_W, cy + CAT_H}, hCatBrush);
        DT({5, cy, MENU_W, cy + CAT_H}, hFont, L"-- Resources --",
           DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(100, 200, 100));
        cy += CAT_H;

        for (int i = wc; i < (int)items.size(); i++) {
            DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold);
            cy += ROW_H;
        }

        FR({0, cy, MENU_W, cy + FOOTER_H}, hHeaderBrush);
        DT({5, cy, MENU_W, cy + FOOTER_H}, hSmallFont, L"CTRL+R = Reset All",
           DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(150, 140, 120));

        if (hasNotif) {
            int mh = GetMenuHeight();
            RECT nr = {MENU_W / 4, mh / 2 - 15, MENU_W * 3 / 4, mh / 2 + 15};
            FR(nr, hNotifBrush);
            DT(nr, hFont, notifText.c_str(),
               DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(200, 255, 200));
        }
    }

    void SetContext(HDC newHdc) { hdc = newHdc; }
    bool IsOpenMenu() { return isMenuActive; }
    bool IsVisible() { return isMenuActive || hasNotif; }
    RECT GetClipRect() {
        RECT r = menuCanvas;
        OffsetRect(&r, offsetX, offsetY);
        return r;
    }
}
