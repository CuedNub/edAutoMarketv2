#include "config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

ConfigManager& ConfigManager::Instance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() : m_defaultSale(500), m_defaultBuy(0), m_weaponCount(0), m_tradeFrequency(100) {
    m_toggleMenu    = { 0x4D, false, false };
    m_saveConfig    = { 0x53, true,  true  };
    m_loadSnapshot  = { 0x4C, true,  true  };
    m_resetAll      = { 0x52, true,  false };
    m_reloadConfig  = { 0x4C, true,  false };
    InitDefaultItems();
}

void ConfigManager::InitDefaultItems() {
    m_items.clear();
    m_items.push_back({"bows",          L"Bows",          m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"crossbows",     L"Crossbows",     m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"leather armor", L"Leather Armor", m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"maces",         L"Maces",         m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"metal armor",   L"Metal Armor",   m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"pikes",         L"Pikes",         m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"spears",        L"Spears",        m_defaultSale, m_defaultBuy, true});
    m_items.push_back({"swords",        L"Swords",        m_defaultSale, m_defaultBuy, true});
    m_weaponCount = (int)m_items.size();
    m_items.push_back({"ale",    L"Ale (Beer)",     m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"bread",  L"Bread",          m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"cheese", L"Cheese",         m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"flour",  L"Flour",          m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"fruit",  L"Fruit (Apples)", m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"hops",   L"Hops",           m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"iron",   L"Iron",           m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"meat",   L"Meat",           m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"pitch",  L"Pitch",          m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"stone",  L"Stone",          m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"wheat",  L"Wheat",          m_defaultSale, m_defaultBuy, false});
    m_items.push_back({"wood",   L"Wood",           m_defaultSale, m_defaultBuy, false});
}

int ConfigManager::ReadInt(const wchar_t* section, const wchar_t* key, int defaultVal, const std::wstring& path) {
    return GetPrivateProfileIntW(section, key, defaultVal, path.c_str());
}

std::wstring ConfigManager::ReadString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultVal, const std::wstring& path) {
    wchar_t buf[256];
    GetPrivateProfileStringW(section, key, defaultVal, buf, 256, path.c_str());
    return std::wstring(buf);
}

COLORREF ConfigManager::ReadColor(const wchar_t* section, const wchar_t* key, COLORREF defaultColor, const std::wstring& path) {
    std::wstring s = ReadString(section, key, L"", path);
    if (s.empty()) return defaultColor;
    int r = 0, g = 0, b = 0;
    if (swscanf(s.c_str(), L"%d,%d,%d", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
    return defaultColor;
}

void ConfigManager::WriteColor(const wchar_t* section, const wchar_t* key, COLORREF color, const std::wstring& path) {
    std::wstring s = std::to_wstring(GetRValue(color)) + L"," + std::to_wstring(GetGValue(color)) + L"," + std::to_wstring(GetBValue(color));
    WritePrivateProfileStringW(section, key, s.c_str(), path.c_str());
}

void ConfigManager::LoadItemThresholds(const std::wstring& path) {
    const wchar_t* weaponKeys[] = {L"Bows", L"Crossbows", L"LeatherArmor", L"Maces", L"MetalArmor", L"Pikes", L"Spears", L"Swords"};
    for (int i = 0; i < m_weaponCount; i++) {
        std::wstring val = ReadString(L"Weapons", weaponKeys[i], L"", path);
        if (!val.empty()) {
            swscanf(val.c_str(), L"%d,%d", &m_items[i].saleThreshold, &m_items[i].buyThreshold);
        }
    }
    const wchar_t* resKeys[] = {L"Ale", L"Bread", L"Cheese", L"Flour", L"Fruit", L"Hops", L"Iron", L"Meat", L"Pitch", L"Stone", L"Wheat", L"Wood"};
    int resCount = (int)m_items.size() - m_weaponCount;
    for (int i = 0; i < resCount; i++) {
        std::wstring val = ReadString(L"Resources", resKeys[i], L"", path);
        if (!val.empty()) {
            swscanf(val.c_str(), L"%d,%d", &m_items[m_weaponCount+i].saleThreshold, &m_items[m_weaponCount+i].buyThreshold);
        }
    }
}

void ConfigManager::SaveItemThresholds(const std::wstring& path) {
    const wchar_t* weaponKeys[] = {L"Bows", L"Crossbows", L"LeatherArmor", L"Maces", L"MetalArmor", L"Pikes", L"Spears", L"Swords"};
    for (int i = 0; i < m_weaponCount; i++) {
        std::wstring val = std::to_wstring(m_items[i].saleThreshold) + L"," + std::to_wstring(m_items[i].buyThreshold);
        WritePrivateProfileStringW(L"Weapons", weaponKeys[i], val.c_str(), path.c_str());
    }
    const wchar_t* resKeys[] = {L"Ale", L"Bread", L"Cheese", L"Flour", L"Fruit", L"Hops", L"Iron", L"Meat", L"Pitch", L"Stone", L"Wheat", L"Wood"};
    int resCount = (int)m_items.size() - m_weaponCount;
    for (int i = 0; i < resCount; i++) {
        std::wstring val = std::to_wstring(m_items[m_weaponCount+i].saleThreshold) + L"," + std::to_wstring(m_items[m_weaponCount+i].buyThreshold);
        WritePrivateProfileStringW(L"Resources", resKeys[i], val.c_str(), path.c_str());
    }
}

// Helper: konversi wstring ke string (UTF-8) untuk std::ofstream
static std::string WtoA(const std::wstring& ws) {
    if (ws.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
    std::string s(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), &s[0], len, nullptr, nullptr);
    return s;
}

void ConfigManager::Load(const std::wstring& iniPath) {
    m_iniPath = iniPath;
    wchar_t dir[MAX_PATH];
    lstrcpynW(dir, iniPath.c_str(), MAX_PATH);
    if (wchar_t* lastSlash = wcsrchr(dir, L'\\')) *(lastSlash + 1) = L'\0';
    m_savePath = std::wstring(dir) + L"ddrawsave.ini";

    // Hotkeys
    m_toggleMenu.vkCode = ReadInt(L"Hotkeys", L"ToggleMenu", 0x4D, m_iniPath);
    m_toggleMenu.ctrl = ReadInt(L"Hotkeys", L"ToggleMenu_Ctrl", 0, m_iniPath);
    m_toggleMenu.shift = ReadInt(L"Hotkeys", L"ToggleMenu_Shift", 0, m_iniPath);

    m_saveConfig.vkCode = ReadInt(L"Hotkeys", L"SaveConfig", 0x53, m_iniPath);
    m_saveConfig.ctrl = ReadInt(L"Hotkeys", L"SaveConfig_Ctrl", 1, m_iniPath);
    m_saveConfig.shift = ReadInt(L"Hotkeys", L"SaveConfig_Shift", 1, m_iniPath);

    m_loadSnapshot.vkCode = ReadInt(L"Hotkeys", L"LoadSnapshot", 0x4C, m_iniPath);
    m_loadSnapshot.ctrl = ReadInt(L"Hotkeys", L"LoadSnapshot_Ctrl", 1, m_iniPath);
    m_loadSnapshot.shift = ReadInt(L"Hotkeys", L"LoadSnapshot_Shift", 1, m_iniPath);

    m_resetAll.vkCode = ReadInt(L"Hotkeys", L"ResetAll", 0x52, m_iniPath);
    m_resetAll.ctrl = ReadInt(L"Hotkeys", L"ResetAll_Ctrl", 1, m_iniPath);
    m_resetAll.shift = ReadInt(L"Hotkeys", L"ResetAll_Shift", 0, m_iniPath);

    m_reloadConfig.vkCode = ReadInt(L"Hotkeys", L"ReloadConfig", 0x4C, m_iniPath);
    m_reloadConfig.ctrl = ReadInt(L"Hotkeys", L"ReloadConfig_Ctrl", 1, m_iniPath);
    m_reloadConfig.shift = ReadInt(L"Hotkeys", L"ReloadConfig_Shift", 0, m_iniPath);

    // Advanced
    m_tradeFrequency = ReadInt(L"Advanced", L"TradeFrequencyMs", 100, m_iniPath);

    // UI
    m_ui.menuWidth = ReadInt(L"UI", L"MenuWidth", 400, m_iniPath);
    m_ui.rowHeight = ReadInt(L"UI", L"RowHeight", 22, m_iniPath);
    m_ui.offsetX = ReadInt(L"UI", L"OffsetX", 0, m_iniPath);
    m_ui.offsetY = ReadInt(L"UI", L"OffsetY", 0, m_iniPath);
    m_ui.fontName = ReadString(L"UI", L"FontName", L"Consolas", m_iniPath);
    m_ui.fontSize = ReadInt(L"UI", L"FontSize", 16, m_iniPath);
    m_ui.titleSize = ReadInt(L"UI", L"TitleSize", 20, m_iniPath);

    // === Stronghold Crusader HD Color Scheme ===
    m_ui.bgColor     = ReadColor(L"UI", L"BgColor", RGB(58, 40, 22), m_iniPath);
    m_ui.headerColor = ReadColor(L"UI", L"HeaderColor", RGB(92, 62, 30), m_iniPath);
    m_ui.rowColor    = ReadColor(L"UI", L"RowColor", RGB(72, 50, 28), m_iniPath);
    m_ui.selColor    = ReadColor(L"UI", L"SelColor", RGB(140, 95, 40), m_iniPath);
    m_ui.editColor   = ReadColor(L"UI", L"EditColor", RGB(170, 110, 35), m_iniPath);
    m_ui.catColor    = ReadColor(L"UI", L"CatColor", RGB(105, 72, 35), m_iniPath);

    m_ui.textColor     = ReadColor(L"UI", L"TextColor", RGB(245, 220, 155), m_iniPath);
    m_ui.selTextColor  = ReadColor(L"UI", L"HighlightTextColor", RGB(255, 245, 200), m_iniPath);
    m_ui.editTextColor = ReadColor(L"UI", L"EditTextColor", RGB(255, 255, 120), m_iniPath);
    m_ui.titleColor    = ReadColor(L"UI", L"TitleColor", RGB(255, 200, 80), m_iniPath);

    m_defaultSale = ReadInt(L"Defaults", L"DefaultSale", 500, m_iniPath);
    m_defaultBuy  = ReadInt(L"Defaults", L"DefaultBuy", 0, m_iniPath);

    // Init semua item dengan nilai default dari [Defaults]
    InitDefaultItems();

    // TIDAK membaca threshold dari ddraw.ini lagi.
    // Hanya baca dari ddrawsave.ini (jika ada) sebagai preset.
    DWORD attr = GetFileAttributesW(m_savePath.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        LoadItemThresholds(m_savePath);
    }
}

void ConfigManager::Save(const std::wstring& iniPath) {
    if (iniPath.empty()) return;

    std::string path = WtoA(iniPath);
    std::ofstream f(path);
    if (!f.is_open()) return;

    f << "; ============================================\n";
    f << "; edAutoMarket v2.0 - Configuration File\n";
    f << "; ============================================\n";
    f << "; File ini HANYA untuk konfigurasi (hotkey, UI, warna).\n";
    f << "; Nilai jual/beli per item disimpan di ddrawsave.ini\n";
    f << "; Tekan CTRL+L di game untuk reload tanpa restart.\n";
    f << "; ============================================\n\n";

    // --- HOTKEYS ---
    f << "[Hotkeys]\n";
    f << "; Format: Virtual-Key Code desimal\n";
    f << "; Referensi: 77=M  76=L  82=R  83=S  65-90=A-Z  112-123=F1-F12\n";
    f << "; _Ctrl dan _Shift: 1 = wajib ditekan, 0 = tidak\n\n";

    f << "; Buka/tutup menu Auto Market (default: M)\n";
    f << "ToggleMenu=" << m_toggleMenu.vkCode << "\n";
    f << "ToggleMenu_Ctrl=" << (m_toggleMenu.ctrl ? 1 : 0) << "\n";
    f << "ToggleMenu_Shift=" << (m_toggleMenu.shift ? 1 : 0) << "\n\n";

    f << "; Simpan konfigurasi + snapshot threshold (default: CTRL+SHIFT+S)\n";
    f << "SaveConfig=" << m_saveConfig.vkCode << "\n";
    f << "SaveConfig_Ctrl=" << (m_saveConfig.ctrl ? 1 : 0) << "\n";
    f << "SaveConfig_Shift=" << (m_saveConfig.shift ? 1 : 0) << "\n\n";

    f << "; Load snapshot threshold dari ddrawsave.ini (default: CTRL+SHIFT+L)\n";
    f << "LoadSnapshot=" << m_loadSnapshot.vkCode << "\n";
    f << "LoadSnapshot_Ctrl=" << (m_loadSnapshot.ctrl ? 1 : 0) << "\n";
    f << "LoadSnapshot_Shift=" << (m_loadSnapshot.shift ? 1 : 0) << "\n\n";

    f << "; Reset semua threshold ke default, tekan 2x untuk konfirmasi (default: CTRL+R)\n";
    f << "ResetAll=" << m_resetAll.vkCode << "\n";
    f << "ResetAll_Ctrl=" << (m_resetAll.ctrl ? 1 : 0) << "\n";
    f << "ResetAll_Shift=" << (m_resetAll.shift ? 1 : 0) << "\n\n";

    f << "; Reload file ddraw.ini tanpa restart game (default: CTRL+L)\n";
    f << "ReloadConfig=" << m_reloadConfig.vkCode << "\n";
    f << "ReloadConfig_Ctrl=" << (m_reloadConfig.ctrl ? 1 : 0) << "\n";
    f << "ReloadConfig_Shift=" << (m_reloadConfig.shift ? 1 : 0) << "\n\n";

    // --- ADVANCED ---
    f << "[Advanced]\n";
    f << "; Interval pengecekan trade dalam milidetik (default: 100)\n";
    f << "; Semakin kecil = semakin cepat trade, tapi lebih berat di CPU\n";
    f << "TradeFrequencyMs=" << m_tradeFrequency << "\n\n";

    // --- UI ---
    f << "[UI]\n";
    f << "; Lebar menu dalam pixel (default: 400)\n";
    f << "MenuWidth=" << m_ui.menuWidth << "\n";
    f << "; Tinggi setiap baris item dalam pixel (default: 22)\n";
    f << "RowHeight=" << m_ui.rowHeight << "\n";
    f << "; Geser posisi menu horizontal (0 = tengah layar)\n";
    f << "OffsetX=" << m_ui.offsetX << "\n";
    f << "; Geser posisi menu vertikal (0 = tengah layar)\n";
    f << "OffsetY=" << m_ui.offsetY << "\n\n";

    f << "; Font yang digunakan (contoh: Consolas, Arial, Tahoma, Verdana)\n";
    f << "FontName=" << WtoA(m_ui.fontName) << "\n";
    f << "; Ukuran font item (default: 16)\n";
    f << "FontSize=" << m_ui.fontSize << "\n";
    f << "; Ukuran font judul menu (default: 20)\n";
    f << "TitleSize=" << m_ui.titleSize << "\n\n";

    f << "; --- Warna UI (format: R,G,B) ---\n";
    f << "; Tema default: Stronghold Crusader HD (coklat kayu & emas)\n\n";

    f << "; Warna latar belakang menu\n";
    f << "BgColor=" << (int)GetRValue(m_ui.bgColor) << "," << (int)GetGValue(m_ui.bgColor) << "," << (int)GetBValue(m_ui.bgColor) << "\n";
    f << "; Warna baris header (Item / Sell / Buy)\n";
    f << "HeaderColor=" << (int)GetRValue(m_ui.headerColor) << "," << (int)GetGValue(m_ui.headerColor) << "," << (int)GetBValue(m_ui.headerColor) << "\n";
    f << "; Warna baris item biasa\n";
    f << "RowColor=" << (int)GetRValue(m_ui.rowColor) << "," << (int)GetGValue(m_ui.rowColor) << "," << (int)GetBValue(m_ui.rowColor) << "\n";
    f << "; Warna baris item yang sedang dipilih (cursor)\n";
    f << "SelColor=" << (int)GetRValue(m_ui.selColor) << "," << (int)GetGValue(m_ui.selColor) << "," << (int)GetBValue(m_ui.selColor) << "\n";
    f << "; Warna cell yang sedang diedit (ketik angka)\n";
    f << "EditColor=" << (int)GetRValue(m_ui.editColor) << "," << (int)GetGValue(m_ui.editColor) << "," << (int)GetBValue(m_ui.editColor) << "\n";
    f << "; Warna baris kategori (Weapons / Resources)\n";
    f << "CatColor=" << (int)GetRValue(m_ui.catColor) << "," << (int)GetGValue(m_ui.catColor) << "," << (int)GetBValue(m_ui.catColor) << "\n\n";

    f << "; --- Warna Teks (format: R,G,B) ---\n\n";
    f << "; Warna teks biasa\n";
    f << "TextColor=" << (int)GetRValue(m_ui.textColor) << "," << (int)GetGValue(m_ui.textColor) << "," << (int)GetBValue(m_ui.textColor) << "\n";
    f << "; Warna teks pada baris yang dipilih\n";
    f << "HighlightTextColor=" << (int)GetRValue(m_ui.selTextColor) << "," << (int)GetGValue(m_ui.selTextColor) << "," << (int)GetBValue(m_ui.selTextColor) << "\n";
    f << "; Warna teks angka saat sedang diedit\n";
    f << "EditTextColor=" << (int)GetRValue(m_ui.editTextColor) << "," << (int)GetGValue(m_ui.editTextColor) << "," << (int)GetBValue(m_ui.editTextColor) << "\n";
    f << "; Warna judul menu\n";
    f << "TitleColor=" << (int)GetRValue(m_ui.titleColor) << "," << (int)GetGValue(m_ui.titleColor) << "," << (int)GetBValue(m_ui.titleColor) << "\n\n";

    // --- DEFAULTS ---
    f << "[Defaults]\n";
    f << "; Nilai awal Sell threshold untuk semua item saat game dimulai\n";
    f << "; (jika tidak ada ddrawsave.ini)\n";
    f << "DefaultSale=" << m_defaultSale << "\n";
    f << "; Nilai awal Buy threshold untuk semua item saat game dimulai\n";
    f << "; 0 = tidak auto-buy\n";
    f << "DefaultBuy=" << m_defaultBuy << "\n";

    f.close();
}

void ConfigManager::SaveSnapshot() {
    if (!m_savePath.empty()) SaveItemThresholds(m_savePath);
}
void ConfigManager::LoadSnapshot() {
    if (!m_savePath.empty()) LoadItemThresholds(m_savePath);
}
void ConfigManager::ResetAll() {
    // Hanya reset di RAM, TIDAK menyentuh ddrawsave.ini
    for (auto& item : m_items) { item.saleThreshold = m_defaultSale; item.buyThreshold = m_defaultBuy; }
}
void ConfigManager::SetItemSale(int index, int value) { if (index >= 0 && index < (int)m_items.size()) m_items[index].saleThreshold = value; }
void ConfigManager::SetItemBuy(int index, int value) { if (index >= 0 && index < (int)m_items.size()) m_items[index].buyThreshold = value; }
bool ConfigManager::CheckHotkey(const HotkeyConfig& hk) {
    bool ctrlOk = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? hk.ctrl : !hk.ctrl;
    bool shiftOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? hk.shift : !hk.shift;
    return (GetAsyncKeyState(hk.vkCode) & 0x8000) && ctrlOk && shiftOk;
}
