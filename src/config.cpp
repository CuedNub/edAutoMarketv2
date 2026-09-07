#include "config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

ConfigManager& ConfigManager::Instance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager()
    : m_defaultSale(500), m_defaultBuy(0), m_weaponCount(0) {
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

void ConfigManager::LoadItemThresholds(const std::wstring& path) {
    const wchar_t* weaponKeys[] = {
        L"Bows", L"Crossbows", L"LeatherArmor", L"Maces", L"MetalArmor", L"Pikes", L"Spears", L"Swords"
    };
    for (int i = 0; i < m_weaponCount; i++) {
        std::wstring val = ReadString(L"Weapons", weaponKeys[i], L"", path);
        if (!val.empty()) {
            int sale = m_defaultSale, buy = m_defaultBuy;
            swscanf(val.c_str(), L"%d,%d", &sale, &buy);
            m_items[i].saleThreshold = sale;
            m_items[i].buyThreshold = buy;
        }
    }
    const wchar_t* resKeys[] = {
        L"Ale", L"Bread", L"Cheese", L"Flour", L"Fruit", L"Hops",
        L"Iron", L"Meat", L"Pitch", L"Stone", L"Wheat", L"Wood"
    };
    int resCount = (int)m_items.size() - m_weaponCount;
    for (int i = 0; i < resCount; i++) {
        std::wstring val = ReadString(L"Resources", resKeys[i], L"", path);
        if (!val.empty()) {
            int sale = m_defaultSale, buy = m_defaultBuy;
            swscanf(val.c_str(), L"%d,%d", &sale, &buy);
            m_items[m_weaponCount + i].saleThreshold = sale;
            m_items[m_weaponCount + i].buyThreshold = buy;
        }
    }
}

void ConfigManager::SaveItemThresholds(const std::wstring& path) {
    const wchar_t* weaponKeys[] = {
        L"Bows", L"Crossbows", L"LeatherArmor", L"Maces", L"MetalArmor", L"Pikes", L"Spears", L"Swords"
    };
    for (int i = 0; i < m_weaponCount; i++) {
        std::wstring val = std::to_wstring(m_items[i].saleThreshold) + L"," + std::to_wstring(m_items[i].buyThreshold);
        WritePrivateProfileStringW(L"Weapons", weaponKeys[i], val.c_str(), path.c_str());
    }
    const wchar_t* resKeys[] = {
        L"Ale", L"Bread", L"Cheese", L"Flour", L"Fruit", L"Hops",
        L"Iron", L"Meat", L"Pitch", L"Stone", L"Wheat", L"Wood"
    };
    int resCount = (int)m_items.size() - m_weaponCount;
    for (int i = 0; i < resCount; i++) {
        std::wstring val = std::to_wstring(m_items[m_weaponCount + i].saleThreshold) + L"," + std::to_wstring(m_items[m_weaponCount + i].buyThreshold);
        WritePrivateProfileStringW(L"Resources", resKeys[i], val.c_str(), path.c_str());
    }
}

void ConfigManager::Load(const std::wstring& iniPath) {
    m_iniPath = iniPath;

    wchar_t dir[MAX_PATH];
    lstrcpynW(dir, iniPath.c_str(), MAX_PATH);
    wchar_t* lastSlash = wcsrchr(dir, L'\\');
    if (lastSlash) *(lastSlash + 1) = L'\0';
    m_savePath = std::wstring(dir) + L"automarketsave.ini";

    m_toggleMenu.vkCode  = ReadInt(L"Hotkeys", L"ToggleMenu", 0x4D, m_iniPath);
    m_toggleMenu.ctrl    = ReadInt(L"Hotkeys", L"ToggleMenu_Ctrl", 0, m_iniPath) != 0;
    m_toggleMenu.shift   = ReadInt(L"Hotkeys", L"ToggleMenu_Shift", 0, m_iniPath) != 0;

    m_saveConfig.vkCode  = ReadInt(L"Hotkeys", L"SaveConfig", 0x53, m_iniPath);
    m_saveConfig.ctrl    = ReadInt(L"Hotkeys", L"SaveConfig_Ctrl", 1, m_iniPath) != 0;
    m_saveConfig.shift   = ReadInt(L"Hotkeys", L"SaveConfig_Shift", 1, m_iniPath) != 0;

    m_loadSnapshot.vkCode  = ReadInt(L"Hotkeys", L"LoadSnapshot", 0x4C, m_iniPath);
    m_loadSnapshot.ctrl    = ReadInt(L"Hotkeys", L"LoadSnapshot_Ctrl", 1, m_iniPath) != 0;
    m_loadSnapshot.shift   = ReadInt(L"Hotkeys", L"LoadSnapshot_Shift", 1, m_iniPath) != 0;

    m_resetAll.vkCode    = ReadInt(L"Hotkeys", L"ResetAll", 0x52, m_iniPath);
    m_resetAll.ctrl      = ReadInt(L"Hotkeys", L"ResetAll_Ctrl", 1, m_iniPath) != 0;
    m_resetAll.shift     = ReadInt(L"Hotkeys", L"ResetAll_Shift", 0, m_iniPath) != 0;

    m_reloadConfig.vkCode  = ReadInt(L"Hotkeys", L"ReloadConfig", 0x4C, m_iniPath);
    m_reloadConfig.ctrl    = ReadInt(L"Hotkeys", L"ReloadConfig_Ctrl", 1, m_iniPath) != 0;
    m_reloadConfig.shift   = ReadInt(L"Hotkeys", L"ReloadConfig_Shift", 0, m_iniPath) != 0;

    m_defaultSale = ReadInt(L"Defaults", L"DefaultSale", 500, m_iniPath);
    m_defaultBuy  = ReadInt(L"Defaults", L"DefaultBuy", 0, m_iniPath);

    InitDefaultItems();
    LoadItemThresholds(m_iniPath);

    DWORD attr = GetFileAttributesW(m_savePath.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        LoadItemThresholds(m_savePath);
    }
}

void ConfigManager::Save(const std::wstring& iniPath) {
    if (iniPath.empty()) return;

    WritePrivateProfileStringW(L"Hotkeys", L"ToggleMenu", std::to_wstring(m_toggleMenu.vkCode).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ToggleMenu_Ctrl", m_toggleMenu.ctrl ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ToggleMenu_Shift", m_toggleMenu.shift ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"SaveConfig", std::to_wstring(m_saveConfig.vkCode).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"SaveConfig_Ctrl", m_saveConfig.ctrl ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"SaveConfig_Shift", m_saveConfig.shift ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"LoadSnapshot", std::to_wstring(m_loadSnapshot.vkCode).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"LoadSnapshot_Ctrl", m_loadSnapshot.ctrl ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"LoadSnapshot_Shift", m_loadSnapshot.shift ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ResetAll", std::to_wstring(m_resetAll.vkCode).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ResetAll_Ctrl", m_resetAll.ctrl ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ResetAll_Shift", m_resetAll.shift ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ReloadConfig", std::to_wstring(m_reloadConfig.vkCode).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ReloadConfig_Ctrl", m_reloadConfig.ctrl ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Hotkeys", L"ReloadConfig_Shift", m_reloadConfig.shift ? L"1" : L"0", iniPath.c_str());
    WritePrivateProfileStringW(L"Defaults", L"DefaultSale", std::to_wstring(m_defaultSale).c_str(), iniPath.c_str());
    WritePrivateProfileStringW(L"Defaults", L"DefaultBuy", std::to_wstring(m_defaultBuy).c_str(), iniPath.c_str());

    SaveItemThresholds(iniPath);
}

void ConfigManager::SaveSnapshot() {
    if (m_savePath.empty()) return;
    SaveItemThresholds(m_savePath);
}

void ConfigManager::LoadSnapshot() {
    if (m_savePath.empty()) return;
    DWORD attr = GetFileAttributesW(m_savePath.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        LoadItemThresholds(m_savePath);
    }
}

void ConfigManager::ResetAll() {
    for (auto& item : m_items) {
        item.saleThreshold = m_defaultSale;
        item.buyThreshold = m_defaultBuy;
    }
}

void ConfigManager::SetItemSale(int index, int value) {
    if (index >= 0 && index < (int)m_items.size())
        m_items[index].saleThreshold = value;
}

void ConfigManager::SetItemBuy(int index, int value) {
    if (index >= 0 && index < (int)m_items.size())
        m_items[index].buyThreshold = value;
}

bool ConfigManager::CheckHotkey(const HotkeyConfig& hk) {
    bool ctrlOk  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? hk.ctrl : !hk.ctrl;
    bool shiftOk = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? hk.shift : !hk.shift;
    bool keyOk   = (GetAsyncKeyState(hk.vkCode) & 0x8000) != 0;
    return keyOk && ctrlOk && shiftOk;
}
