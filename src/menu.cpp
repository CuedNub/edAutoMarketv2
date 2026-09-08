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

    int offsetX, offsetY, screenW, screenH;
    RECT menuCanvas;

    bool isMenuActive, isEditing, hasNotif, pendingClose, resetConfirm;
    int selectedRow, selectedCol;
    std::wstring editBuffer, notifText;
    std::chrono::time_point<std::chrono::high_resolution_clock> notifEnd, resetTime;

    void ReloadUI() {
        auto& ui = ConfigManager::Instance().GetUI();

        if (hBgBrush) {
            DeleteObject(hBgBrush); DeleteObject(hHeaderBrush); DeleteObject(hRowBrush);
            DeleteObject(hSelBrush); DeleteObject(hEditBrush); DeleteObject(hCatBrush);
            DeleteObject(hNotifBrush); DeleteObject(hTitleFont); DeleteObject(hFont); DeleteObject(hSmallFont);
        }

        hBgBrush     = CreateSolidBrush(ui.bgColor);
        hHeaderBrush = CreateSolidBrush(ui.headerColor);
        hRowBrush    = CreateSolidBrush(ui.rowColor);
        hSelBrush    = CreateSolidBrush(ui.selColor);
        hEditBrush   = CreateSolidBrush(ui.editColor);
        hCatBrush    = CreateSolidBrush(ui.catColor);
        hNotifBrush  = CreateSolidBrush(RGB(40, 80, 40));

        hTitleFont = CreateFontW(ui.titleSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
        hFont = CreateFontW(ui.fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
        hSmallFont = CreateFontW(ui.fontSize - 3, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
    }

    int GetMenuHeight() {
        auto& cfg = ConfigManager::Instance();
        auto& ui = cfg.GetUI();
        int wc = cfg.GetWeaponCount();
        int rc = (int)cfg.GetItems().size() - wc;
        return (ui.titleSize + 10) + (ui.fontSize + 8) + (ui.fontSize + 6) + wc * ui.rowHeight + (ui.fontSize + 6) + rc * ui.rowHeight + (ui.fontSize + 8);
    }

    void Init(HWND hwnd) {
        g_hwnd = hwnd;
        isMenuActive = isEditing = hasNotif = pendingClose = resetConfirm = false;
        selectedRow = selectedCol = 0;
        ReloadUI();
    }

    void Uninit() {}

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
            if (pendingClose) { isMenuActive = isEditing = pendingClose = false; }
        }
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

    LRESULT ImplWin32_WndProcHandler(WNDPROC oWndProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_KEYDOWN) {
            if (lParam & 0x40000000) return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
            auto& cfg = ConfigManager::Instance();

            if (ConfigManager::CheckHotkey(cfg.GetToggleMenu())) {
                if (isEditing) { isEditing = false; editBuffer = L""; }
                isMenuActive = !isMenuActive;
                return 0;
            }
            if (ConfigManager::CheckHotkey(cfg.GetReloadConfig())) {
                cfg.Load(cfg.GetIniPath());
                ReloadUI(); // Terapkan font dan warna baru
                if (isMenuActive) ShowNotif(L"Config Reloaded!", 1200);
                return 0;
            }
            if (ConfigManager::CheckHotkey(cfg.GetLoadSnapshot())) {
                cfg.LoadSnapshot();
                if (isMenuActive) ShowNotif(L"Loaded!", 1200);
                return 0;
            }
            if (!isMenuActive) return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

            if (ConfigManager::CheckHotkey(cfg.GetSaveConfig())) {
                ApplyEdit();
                cfg.Save(cfg.GetIniPath()); // Simpan warna & layout ke .ini jika ada perubahan baru
                cfg.SaveSnapshot();
                ShowNotif(L"Saved!", 1500);
                pendingClose = true;
                return 0;
            }

            int total = (int)cfg.GetItems().size();
            if (isEditing) {
                if (wParam >= '0' && wParam <= '9') { if (editBuffer.length() < 4) editBuffer += (wchar_t)wParam; return 0; }
                if (wParam == VK_BACK) { if (!editBuffer.empty()) editBuffer.pop_back(); return 0; }
                if (wParam == VK_DELETE) { editBuffer.clear(); return 0; }
                if (wParam == VK_RETURN) { ApplyEdit(); return 0; }
                if (wParam == VK_ESCAPE) { isEditing = false; return 0; }
                if (wParam == VK_TAB)    { ApplyEdit(); selectedCol = 1 - selectedCol; return 0; }
            } else {
                if (wParam == VK_UP)   { if (selectedRow > 0) selectedRow--; return 0; }
                if (wParam == VK_DOWN) { if (selectedRow < total - 1) selectedRow++; return 0; }
                if (wParam == VK_TAB)  { selectedCol = 1 - selectedCol; return 0; }
                if (wParam >= '0' && wParam <= '9') {
                    if (selectedRow >= 0 && selectedRow < total) { isEditing = true; editBuffer = (wchar_t)wParam; }
                    return 0;
                }
                if (wParam == VK_ESCAPE) { isMenuActive = false; return 0; }
            }
        }
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    void DrawRow(int idx, int cy, const std::wstring& name, int sell, int buy, const UIConfig& ui) {
        bool sel = (idx == selectedRow);
        FR({0, cy, ui.menuWidth, cy + ui.rowHeight}, sel ? hSelBrush : hRowBrush);

        int col1w = (int)(ui.menuWidth * 0.45);
        int col2w = (int)(ui.menuWidth * 0.25);

        std::wstring prefix = sel ? L"> " : L"  ";
        DT({5, cy, col1w, cy + ui.rowHeight}, hFont, (prefix + name).c_str(),
           DT_LEFT | DT_VCENTER | DT_SINGLELINE, sel ? ui.selTextColor : ui.textColor);

        for (int c = 0; c < 2; c++) {
            int cx = (c == 0) ? col1w : col1w + col2w;
            int cw = (c == 0) ? col2w : ui.menuWidth - col1w - col2w;
            bool isCell = sel && (selectedCol == c);
            bool isEdit = isCell && isEditing;

            if (isCell) FR({cx, cy, cx + cw, cy + ui.rowHeight}, isEdit ? hEditBrush : hSelBrush);

            std::wstring valStr = isEdit ? L"[" + editBuffer + L"_]" : std::to_wstring(c == 0 ? sell : buy);
            DT({cx, cy, cx + cw, cy + ui.rowHeight}, hFont, valStr.c_str(),
               DT_CENTER | DT_VCENTER | DT_SINGLELINE, isCell ? ui.editTextColor : ui.textColor);
        }
    }

    void Draw() {
        ProcessNotif();
        if (!isMenuActive && !hasNotif) return;

        auto& cfg = ConfigManager::Instance();
        auto& ui = cfg.GetUI();
        const auto& items = cfg.GetItems();
        int wc = cfg.GetWeaponCount();

        int titleH = ui.titleSize + 10;
        int headerH = ui.fontSize + 8;
        int catH = ui.fontSize + 6;
        int menuH = GetMenuHeight();

        RECT wr; GetClientRect(g_hwnd, &wr);
        screenW = wr.right - wr.left; screenH = wr.bottom - wr.top;
        offsetX = (screenW / 2 - ui.menuWidth / 2) + ui.offsetX;
        offsetY = (screenH / 2 - menuH / 2) + ui.offsetY;
        menuCanvas = {0, 0, ui.menuWidth, menuH};

        FR(menuCanvas, hBgBrush);
        int cy = 0;

        DT({0, cy, ui.menuWidth, cy + titleH}, hTitleFont, L"edAutoMarket v2.0", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
        cy += titleH;

        int col1w = (int)(ui.menuWidth * 0.45), col2w = (int)(ui.menuWidth * 0.25);
        FR({0, cy, ui.menuWidth, cy + headerH}, hHeaderBrush);
        DT({5, cy, col1w, cy + headerH}, hFont, L"Item", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);
        DT({col1w, cy, col1w + col2w, cy + headerH}, hFont, L"Sell >", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.textColor);
        DT({col1w + col2w, cy, ui.menuWidth - 5, cy + headerH}, hFont, L"Buy <", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.textColor);
        cy += headerH;

        FR({0, cy, ui.menuWidth, cy + catH}, hCatBrush);
        DT({5, cy, ui.menuWidth, cy + catH}, hFont, L"-- Weapons --", DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(255, 150, 100));
        cy += catH;

        for (int i = 0; i < wc; i++) { DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold, ui); cy += ui.rowHeight; }

        FR({0, cy, ui.menuWidth, cy + catH}, hCatBrush);
        DT({5, cy, ui.menuWidth, cy + catH}, hFont, L"-- Resources --", DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(100, 200, 100));
        cy += catH;

        for (int i = wc; i < (int)items.size(); i++) { DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold, ui); cy += ui.rowHeight; }

        FR({0, cy, ui.menuWidth, cy + headerH}, hHeaderBrush);
        DT({5, cy, ui.menuWidth, cy + headerH}, hSmallFont, L"CTRL+R = Reset All", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);

        if (hasNotif) {
            RECT nr = {ui.menuWidth / 4, menuH / 2 - 15, ui.menuWidth * 3 / 4, menuH / 2 + 15};
            FR(nr, hNotifBrush);
            DT(nr, hFont, notifText.c_str(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(200, 255, 200));
        }
    }
    void SetContext(HDC newHdc) { hdc = newHdc; }
    bool IsVisible() { return isMenuActive || hasNotif; }
}
