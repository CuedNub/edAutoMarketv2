#include <vector>
#include <string>
#include <chrono>
#include <windowsx.h>
#include "Menu.hpp"
#include "config.hpp"

namespace Menu {
    enum PopupType {
        POPUP_NONE = 0,
        POPUP_SAVE = 1,       // Save preset menu (Update/Add/Delete/Cancel)
        POPUP_SAVE_ADD = 2,   // Input nama preset baru
        POPUP_SAVE_CONFIRM_DEL = 3, // Konfirmasi hapus
        POPUP_LOAD = 4,       // List preset untuk dipilih
        POPUP_HELP = 5        // Menu bantuan hotkey
    };

    HBRUSH hBgBrush, hHeaderBrush, hRowBrush, hSelBrush, hEditBrush, hCatBrush, hNotifBrush, hPopupBrush;
    HFONT hTitleFont, hFont, hSmallFont;
    HDC hdc;
    HWND g_hwnd;

    int offsetX, offsetY, screenW, screenH;
    RECT menuCanvas;

    bool isMenuActive, isEditing, hasNotif, pendingClose, resetConfirm;
    int selectedRow, selectedCol;
    std::wstring editBuffer, notifText;
    std::chrono::time_point<std::chrono::high_resolution_clock> notifEnd, resetTime;

    PopupType activePopup = POPUP_NONE;
    int popupSelectedIdx = 0;
    std::wstring popupInputBuffer;

    wchar_t lastJumpChar = 0;
    int lastJumpIndex = -1;

    void ReloadUI() {
        auto& ui = ConfigManager::Instance().GetUI();

        if (hBgBrush) {
            DeleteObject(hBgBrush); DeleteObject(hHeaderBrush); DeleteObject(hRowBrush);
            DeleteObject(hSelBrush); DeleteObject(hEditBrush); DeleteObject(hCatBrush);
            DeleteObject(hNotifBrush); DeleteObject(hPopupBrush);
            DeleteObject(hTitleFont); DeleteObject(hFont); DeleteObject(hSmallFont);
        }

        hBgBrush     = CreateSolidBrush(ui.bgColor);
        hHeaderBrush = CreateSolidBrush(ui.headerColor);
        hRowBrush    = CreateSolidBrush(ui.rowColor);
        hSelBrush    = CreateSolidBrush(ui.selColor);
        hEditBrush   = CreateSolidBrush(ui.editColor);
        hCatBrush    = CreateSolidBrush(ui.catColor);
        hNotifBrush  = CreateSolidBrush(RGB(80, 55, 25));
        hPopupBrush  = CreateSolidBrush(RGB(40, 25, 12));

        hTitleFont = CreateFontW(ui.titleSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
        hFont = CreateFontW(ui.fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
        hSmallFont = CreateFontW(ui.fontSize - 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, ui.fontName.c_str());
    }

    int GetMenuHeight() {
        auto& cfg = ConfigManager::Instance();
        auto& ui = cfg.GetUI();
        int wc = cfg.GetWeaponCount();
        int rc = (int)cfg.GetItems().size() - wc;
        return (ui.titleSize + 10) + (ui.fontSize + 8) + (ui.fontSize + 6) + wc * ui.rowHeight + (ui.fontSize + 6) + rc * ui.rowHeight + ui.rowHeight;
    }

    void Init(HWND hwnd) {
        g_hwnd = hwnd;
        isMenuActive = isEditing = hasNotif = pendingClose = resetConfirm = false;
        selectedRow = 0; selectedCol = 0;
        activePopup = POPUP_NONE;
        popupSelectedIdx = 0;
        lastJumpChar = 0;
        lastJumpIndex = -1;
        ReloadUI();
    }

    void Uninit() {}

    bool IsOpenMenu() { return isMenuActive; }

    RECT GetClipRect() { 
        RECT r = menuCanvas;
        OffsetRect(&r, offsetX, offsetY);
        // Perluas untuk mencakup popup
        r.left -= 100; r.right += 100; r.top -= 50; r.bottom += 200;
        return r; 
    }

    void FR(RECT r, HBRUSH b) {
        OffsetRect(&r, offsetX, offsetY);
        FillRect(hdc, &r, b);
    }

    void FR_Abs(RECT r, HBRUSH b) {
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

    void DT_Abs(RECT r, HFONT f, const wchar_t* t, int fmt, COLORREF c) {
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

    void JumpToLetter(wchar_t ch) {
        auto& cfg = ConfigManager::Instance();
        const auto& items = cfg.GetItems();
        int total = (int)items.size();
        if (total == 0) return;

        wchar_t upperCh = towupper(ch);

        std::vector<int> matches;
        for (int i = 0; i < total; i++) {
            if (!items[i].displayName.empty()) {
                wchar_t firstCh = towupper(items[i].displayName[0]);
                if (firstCh == upperCh) matches.push_back(i);
            }
        }
        if (matches.empty()) return;

        int targetIdx;
        if (lastJumpChar == upperCh && lastJumpIndex >= 0) {
            int nextPos = 0;
            for (size_t k = 0; k < matches.size(); k++) {
                if (matches[k] == lastJumpIndex) {
                    nextPos = (k + 1) % matches.size();
                    break;
                }
            }
            targetIdx = matches[nextPos];
        } else {
            targetIdx = matches[0];
        }

        selectedRow = targetIdx;
        lastJumpChar = upperCh;
        lastJumpIndex = targetIdx;
    }

    void OpenSavePopup() {
        activePopup = POPUP_SAVE;
        popupSelectedIdx = 0;
        popupInputBuffer = L"";
    }
    void OpenLoadPopup() {
        activePopup = POPUP_LOAD;
        popupSelectedIdx = ConfigManager::Instance().GetActivePresetIndex();
    }
    void OpenHelpPopup() {
        activePopup = POPUP_HELP;
        popupSelectedIdx = 0;
    }
    void ClosePopup() {
        activePopup = POPUP_NONE;
        popupInputBuffer = L"";
    }

    LRESULT HandlePopupKey(WPARAM wParam, bool isRepeat) {
        auto& cfg = ConfigManager::Instance();

        // ESC = tutup semua popup (kembali ke menu utama, tidak menutup menu)
        if (!isRepeat && wParam == VK_ESCAPE) {
            if (activePopup == POPUP_SAVE_ADD || activePopup == POPUP_SAVE_CONFIRM_DEL) {
                activePopup = POPUP_SAVE;
                popupInputBuffer = L"";
                return 0;
            }
            ClosePopup();
            return 0;
        }

        if (activePopup == POPUP_SAVE) {
            if (!isRepeat && (wParam == 'U' || wParam == '1')) {
                cfg.SaveActivePreset();
                ShowNotif(L"Updated: " + cfg.GetActivePresetName(), 1200);
                ClosePopup();
                return 0;
            }
            if (!isRepeat && (wParam == 'A' || wParam == '2')) {
                activePopup = POPUP_SAVE_ADD;
                popupInputBuffer = L"";
                return 0;
            }
            if (!isRepeat && (wParam == 'D' || wParam == '3')) {
                if (cfg.GetPresetList().size() <= 1) {
                    ShowNotif(L"Cannot delete last preset!", 1500);
                    return 0;
                }
                activePopup = POPUP_SAVE_CONFIRM_DEL;
                return 0;
            }
            return 0;
        }

        if (activePopup == POPUP_SAVE_ADD) {
            if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9')) {
                if (popupInputBuffer.length() < 15) {
                    wchar_t ch = (wchar_t)wParam;
                    if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && ch >= 'A' && ch <= 'Z') {
                        ch = towlower(ch);
                    }
                    popupInputBuffer += ch;
                }
                return 0;
            }
            if (wParam == VK_SPACE) {
                if (popupInputBuffer.length() < 15) popupInputBuffer += L' ';
                return 0;
            }
            if (wParam == VK_BACK) {
                if (!popupInputBuffer.empty()) popupInputBuffer.pop_back();
                return 0;
            }
            if (!isRepeat && wParam == VK_RETURN) {
                if (!popupInputBuffer.empty()) {
                    cfg.AddPreset(popupInputBuffer);
                    ShowNotif(L"Added: " + popupInputBuffer, 1200);
                    ClosePopup();
                }
                return 0;
            }
            return 0;
        }

        if (activePopup == POPUP_SAVE_CONFIRM_DEL) {
            if (!isRepeat && (wParam == 'Y' || wParam == VK_RETURN)) {
                std::wstring deletedName = cfg.GetActivePresetName();
                cfg.DeleteActivePreset();
                ShowNotif(L"Deleted: " + deletedName, 1200);
                ClosePopup();
                return 0;
            }
            if (!isRepeat && wParam == 'N') {
                activePopup = POPUP_SAVE;
                return 0;
            }
            return 0;
        }

        if (activePopup == POPUP_LOAD) {
            const auto& presets = cfg.GetPresetList();
            int total = (int)presets.size();
            if (wParam == VK_UP)   { if (popupSelectedIdx > 0) popupSelectedIdx--; return 0; }
            if (wParam == VK_DOWN) { if (popupSelectedIdx < total - 1) popupSelectedIdx++; return 0; }
            if (wParam == VK_HOME) { popupSelectedIdx = 0; return 0; }
            if (wParam == VK_END)  { popupSelectedIdx = total - 1; return 0; }
            if (!isRepeat && wParam == VK_RETURN) {
                cfg.SetActivePreset(popupSelectedIdx);
                ShowNotif(L"Loaded: " + cfg.GetActivePresetName(), 1200);
                ClosePopup();
                return 0;
            }
            return 0;
        }

        if (activePopup == POPUP_HELP) {
            // Any key to close (except modifiers)
            if (!isRepeat && wParam != VK_CONTROL && wParam != VK_SHIFT && wParam != VK_MENU) {
                ClosePopup();
                return 0;
            }
            return 0;
        }

        return 0;
    }

    LRESULT ImplWin32_WndProcHandler(WNDPROC oWndProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        auto& cfg = ConfigManager::Instance();

        if (isMenuActive) {
            switch (uMsg) {
                case WM_KEYDOWN: case WM_KEYUP:
                case WM_SYSKEYDOWN: case WM_SYSKEYUP:
                case WM_CHAR: case WM_SYSCHAR: case WM_DEADCHAR:
                case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
                case WM_MBUTTONDOWN: case WM_MBUTTONUP:
                case WM_MOUSEWHEEL: case WM_MOUSEMOVE:
                    if (uMsg != WM_KEYDOWN && uMsg != WM_SYSKEYDOWN) return 0;
                    break;
            }
        }

        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
            bool isRepeat = (lParam & 0x40000000) != 0;

            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetToggleMenu())) {
                if (isEditing) { isEditing = false; editBuffer = L""; }
                if (activePopup != POPUP_NONE) ClosePopup();
                isMenuActive = !isMenuActive;
                lastJumpChar = 0; lastJumpIndex = -1;
                return 0;
            }
            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetReloadConfig())) {
                cfg.Load(cfg.GetIniPath());
                ReloadUI();
                if (isMenuActive) ShowNotif(L"Config Reloaded!", 1200);
                return 0;
            }
            
            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetLoadSnapshot())) {
                if (!isMenuActive) isMenuActive = true;
                OpenLoadPopup();
                return 0;
            }

            if (!isMenuActive) return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetSaveConfig())) {
                ApplyEdit();
                OpenSavePopup();
                return 0;
            }

            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetHelp())) {
                if (activePopup == POPUP_HELP) ClosePopup();
                else OpenHelpPopup();
                return 0;
            }

            // Jika popup aktif, semua input dialihkan ke popup handler
            if (activePopup != POPUP_NONE) {
                return HandlePopupKey(wParam, isRepeat);
            }

            if (!isRepeat && ConfigManager::CheckHotkey(cfg.GetResetAll())) {
                auto now = std::chrono::high_resolution_clock::now();
                if (resetConfirm &&
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - resetTime).count() < 2000) {
                    cfg.ResetAll();
                    resetConfirm = false;
                    ShowNotif(L"Reset!", 1200);
                } else {
                    resetConfirm = true;
                    resetTime = now;
                    ShowNotif(L"Press hotkey again to confirm reset", 2000);
                }
                return 0;
            }

            int total = (int)cfg.GetItems().size();

            if (isEditing) {
                if (wParam >= '0' && wParam <= '9') { if (editBuffer.length() < 4) editBuffer += (wchar_t)wParam; return 0; }
                if (wParam == VK_BACK)   { if (!editBuffer.empty()) editBuffer.pop_back(); return 0; }
                if (wParam == VK_DELETE) { editBuffer.clear(); return 0; }
                if (!isRepeat && wParam == VK_RETURN) { ApplyEdit(); return 0; }
                if (!isRepeat && wParam == VK_ESCAPE) { isEditing = false; editBuffer = L""; return 0; }
                if (!isRepeat && wParam == VK_LEFT)   { ApplyEdit(); selectedCol = 0; return 0; }
                if (!isRepeat && wParam == VK_RIGHT)  { ApplyEdit(); selectedCol = 1; return 0; }
                if (!isRepeat && wParam == VK_TAB)    { ApplyEdit(); selectedCol = 1 - selectedCol; return 0; }
                return 0;
            } else {
                if (wParam == VK_UP)   { if (selectedRow > 0) selectedRow--; lastJumpChar = 0; return 0; }
                if (wParam == VK_DOWN) { if (selectedRow < total - 1) selectedRow++; lastJumpChar = 0; return 0; }
                if (wParam == VK_HOME) { selectedRow = 0; lastJumpChar = 0; return 0; }
                if (wParam == VK_END)  { selectedRow = total - 1; lastJumpChar = 0; return 0; }
                if (wParam == VK_PRIOR){ selectedRow = std::max(0, selectedRow - 5); lastJumpChar = 0; return 0; }
                if (wParam == VK_NEXT) { selectedRow = std::min(total - 1, selectedRow + 5); lastJumpChar = 0; return 0; }

                if (!isRepeat && wParam == VK_LEFT)  { selectedCol = 0; return 0; }
                if (!isRepeat && wParam == VK_RIGHT) { selectedCol = 1; return 0; }
                if (!isRepeat && wParam == VK_TAB)   { selectedCol = 1 - selectedCol; return 0; }

                if (wParam >= '0' && wParam <= '9') {
                    if (selectedRow >= 0 && selectedRow < total) { isEditing = true; editBuffer = (wchar_t)wParam; }
                    return 0;
                }

                if (!isRepeat && wParam >= 'A' && wParam <= 'Z') {
                    JumpToLetter((wchar_t)wParam);
                    return 0;
                }

                if (!isRepeat && wParam == VK_ESCAPE) { isMenuActive = false; lastJumpChar = 0; return 0; }
                return 0;
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

    void DrawPopup() {
        auto& cfg = ConfigManager::Instance();
        auto& ui = cfg.GetUI();

        int pw = 340, ph = 200;
        if (activePopup == POPUP_LOAD) {
            int lines = (int)cfg.GetPresetList().size();
            ph = 60 + lines * ui.rowHeight + 40;
            if (ph < 200) ph = 200;
        } else if (activePopup == POPUP_HELP) {
            pw = 420; ph = 300;
        }

        int px = (screenW - pw) / 2;
        int py = (screenH - ph) / 2;
        RECT pr = {px, py, px + pw, py + ph};

        // Bayangan / border
        RECT border = {px - 2, py - 2, px + pw + 2, py + ph + 2};
        FR_Abs(border, hHeaderBrush);
        FR_Abs(pr, hPopupBrush);

        int cy = py + 10;

        if (activePopup == POPUP_SAVE) {
            DT_Abs({px, cy, px + pw, cy + ui.titleSize + 4}, hTitleFont, L"Save Preset", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
            cy += ui.titleSize + 15;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, (L"Active: " + cfg.GetActivePresetName()).c_str(), DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);
            cy += 30;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, L"[U] Update Active Preset", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += 26;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, L"[A] Add New Preset", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += 26;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, L"[D] Delete Active Preset", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += 26;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hSmallFont, L"[ESC] Cancel", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.selTextColor);
        }
        else if (activePopup == POPUP_SAVE_ADD) {
            DT_Abs({px, cy, px + pw, cy + ui.titleSize + 4}, hTitleFont, L"Add New Preset", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
            cy += ui.titleSize + 20;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, L"Enter preset name:", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);
            cy += 30;
            RECT ib = {px + 20, cy, px + pw - 20, cy + 30};
            FR_Abs(ib, hEditBrush);
            DT_Abs(ib, hFont, (L" " + popupInputBuffer + L"_").c_str(), DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.editTextColor);
            cy += 40;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hSmallFont, L"[ENTER] Save   [ESC] Back", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.selTextColor);
        }
        else if (activePopup == POPUP_SAVE_CONFIRM_DEL) {
            DT_Abs({px, cy, px + pw, cy + ui.titleSize + 4}, hTitleFont, L"Confirm Delete", DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(255, 100, 100));
            cy += ui.titleSize + 20;
            DT_Abs({px + 20, cy, px + pw - 20, cy + 24}, hFont, L"Delete preset:", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += 28;
            DT_Abs({px + 20, cy, px + pw - 20, cy + 24}, hFont, cfg.GetActivePresetName().c_str(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.editTextColor); cy += 40;
            DT_Abs({px + 20, cy, px + pw, cy + 24}, hFont, L"[Y] Yes   [N] No   [ESC] Cancel", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.selTextColor);
        }
        else if (activePopup == POPUP_LOAD) {
            DT_Abs({px, cy, px + pw, cy + ui.titleSize + 4}, hTitleFont, L"Load Preset", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
            cy += ui.titleSize + 15;
            const auto& presets = cfg.GetPresetList();
            for (size_t i = 0; i < presets.size(); i++) {
                bool sel = ((int)i == popupSelectedIdx);
                RECT rr = {px + 15, cy, px + pw - 15, cy + ui.rowHeight};
                if (sel) FR_Abs(rr, hSelBrush);
                std::wstring txt = (sel ? L"> " : L"  ") + presets[i];
                if ((int)i == cfg.GetActivePresetIndex()) txt += L"  (active)";
                DT_Abs({px + 25, cy, px + pw - 15, cy + ui.rowHeight}, hFont, txt.c_str(), DT_LEFT | DT_VCENTER | DT_SINGLELINE, sel ? ui.selTextColor : ui.textColor);
                cy += ui.rowHeight;
            }
            cy += 10;
            DT_Abs({px + 15, cy, px + pw, cy + 24}, hSmallFont, L"[UP/DN] Navigate  [ENTER] Load  [ESC] Cancel", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.selTextColor);
        }
        else if (activePopup == POPUP_HELP) {
            DT_Abs({px, cy, px + pw, cy + ui.titleSize + 4}, hTitleFont, L"Hotkey Help", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
            cy += ui.titleSize + 15;

            auto drawHelp = [&](const wchar_t* label, const std::wstring& hk) {
                DT_Abs({px + 20, cy, px + 180, cy + ui.rowHeight}, hFont, label, DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);
                DT_Abs({px + 180, cy, px + pw - 20, cy + ui.rowHeight}, hFont, hk.c_str(), DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.editTextColor);
                cy += ui.rowHeight;
            };

            drawHelp(L"Toggle Menu",   ConfigManager::HotkeyToString(cfg.GetToggleMenu()));
            drawHelp(L"Save Preset",   ConfigManager::HotkeyToString(cfg.GetSaveConfig()));
            drawHelp(L"Load Preset",   ConfigManager::HotkeyToString(cfg.GetLoadSnapshot()));
            drawHelp(L"Reset All",     ConfigManager::HotkeyToString(cfg.GetResetAll()));
            drawHelp(L"Reload Config", ConfigManager::HotkeyToString(cfg.GetReloadConfig()));
            drawHelp(L"Help",          ConfigManager::HotkeyToString(cfg.GetHelp()));
            cy += 10;
            DT_Abs({px + 20, cy, px + pw - 20, cy + ui.rowHeight}, hSmallFont, L"In Menu: UP/DN=nav, 0-9=edit, A-Z=jump", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += ui.rowHeight;
            DT_Abs({px + 20, cy, px + pw - 20, cy + ui.rowHeight}, hSmallFont, L"Edit .ini file to change hotkeys.", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor); cy += ui.rowHeight;
            DT_Abs({px + 20, cy, px + pw - 20, cy + ui.rowHeight}, hSmallFont, L"[Any key] Close", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.selTextColor);
        }
    }

    void Draw() {
        ProcessNotif();
        if (!isMenuActive && !hasNotif) return;

        auto& cfg = ConfigManager::Instance();
        auto& ui = cfg.GetUI();
        const auto& items = cfg.GetItems();
        int wc = cfg.GetWeaponCount();

        RECT wr; GetClientRect(g_hwnd, &wr);
        screenW = wr.right - wr.left; screenH = wr.bottom - wr.top;

        if (isMenuActive) {
            int titleH = ui.titleSize + 10;
            int headerH = ui.fontSize + 8;
            int catH = ui.fontSize + 6;
            int menuH = GetMenuHeight();

            offsetX = (screenW / 2 - ui.menuWidth / 2) + ui.offsetX;
            offsetY = (screenH / 2 - menuH / 2) + ui.offsetY;
            menuCanvas = {0, 0, ui.menuWidth, menuH};

            FR(menuCanvas, hBgBrush);
            int cy = 0;

            DT({0, cy, ui.menuWidth, cy + titleH}, hTitleFont, L"edAutoMarket v2.1", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);
            cy += titleH;

            int col1w = (int)(ui.menuWidth * 0.45), col2w = (int)(ui.menuWidth * 0.25);
            FR({0, cy, ui.menuWidth, cy + headerH}, hHeaderBrush);
            DT({5, cy, col1w, cy + headerH}, hFont, L"Item", DT_LEFT | DT_VCENTER | DT_SINGLELINE, ui.textColor);
            DT({col1w, cy, col1w + col2w, cy + headerH}, hFont, L"Sell >", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.textColor);
            DT({col1w + col2w, cy, ui.menuWidth - 5, cy + headerH}, hFont, L"Buy <", DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.textColor);
            cy += headerH;

            FR({0, cy, ui.menuWidth, cy + catH}, hCatBrush);
            DT({5, cy, ui.menuWidth, cy + catH}, hFont, L"-- Weapons --", DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(255, 180, 90));
            cy += catH;

            for (int i = 0; i < wc; i++) { DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold, ui); cy += ui.rowHeight; }

            FR({0, cy, ui.menuWidth, cy + catH}, hCatBrush);
            DT({5, cy, ui.menuWidth, cy + catH}, hFont, L"-- Resources --", DT_LEFT | DT_VCENTER | DT_SINGLELINE, RGB(230, 200, 120));
            cy += catH;

            for (int i = wc; i < (int)items.size(); i++) { DrawRow(i, cy, items[i].displayName, items[i].saleThreshold, items[i].buyThreshold, ui); cy += ui.rowHeight; }

            // Footer: hanya nama preset (tanpa keterangan hotkey)
            FR({0, cy, ui.menuWidth, cy + ui.rowHeight}, hHeaderBrush);
            std::wstring footerText = L"Preset: " + cfg.GetActivePresetName();
            DT({0, cy, ui.menuWidth, cy + ui.rowHeight}, hFont, footerText.c_str(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, ui.titleColor);

            // Popup di atas menu
            if (activePopup != POPUP_NONE) {
                DrawPopup();
            }
        }

        if (hasNotif) {
            int nw = 300, nh = 40;
            RECT nr = {(screenW - nw)/2, screenH - 120, (screenW + nw)/2, screenH - 120 + nh};
            FR_Abs(nr, hNotifBrush);
            DT_Abs(nr, hFont, notifText.c_str(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(255, 235, 180));
        }
    }
    void SetContext(HDC newHdc) { hdc = newHdc; }
    bool IsVisible() { return isMenuActive || hasNotif; }
}
