#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct HotkeyConfig {
    UINT vkCode;
    bool ctrl;
    bool shift;
};

struct ItemConfig {
    std::string name;
    std::wstring displayName;
    int saleThreshold;
    int buyThreshold;
    bool isWeapon;
};

class ConfigManager {
public:
    static ConfigManager& Instance();

    void Load(const std::wstring& iniPath);
    void Save(const std::wstring& iniPath);
    void ResetAll();

    void SaveSnapshot();
    void LoadSnapshot();

    HotkeyConfig GetToggleMenu() const { return m_toggleMenu; }
    HotkeyConfig GetSaveConfig() const { return m_saveConfig; }
    HotkeyConfig GetLoadSnapshot() const { return m_loadSnapshot; }
    HotkeyConfig GetResetAll() const { return m_resetAll; }
    HotkeyConfig GetReloadConfig() const { return m_reloadConfig; }

    const std::vector<ItemConfig>& GetItems() const { return m_items; }
    void SetItemSale(int index, int value);
    void SetItemBuy(int index, int value);

    int GetDefaultSale() const { return m_defaultSale; }
    int GetDefaultBuy() const { return m_defaultBuy; }

    static bool CheckHotkey(const HotkeyConfig& hk);
    std::wstring GetIniPath() const { return m_iniPath; }
    std::wstring GetSavePath() const { return m_savePath; }
    int GetWeaponCount() const { return m_weaponCount; }

private:
    ConfigManager();
    void InitDefaultItems();
    int ReadInt(const wchar_t* section, const wchar_t* key, int defaultVal, const std::wstring& path);
    std::wstring ReadString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultVal, const std::wstring& path);
    void LoadItemThresholds(const std::wstring& path);
    void SaveItemThresholds(const std::wstring& path);

    std::wstring m_iniPath;
    std::wstring m_savePath;
    HotkeyConfig m_toggleMenu;
    HotkeyConfig m_saveConfig;
    HotkeyConfig m_loadSnapshot;
    HotkeyConfig m_resetAll;
    HotkeyConfig m_reloadConfig;
    int m_defaultSale;
    int m_defaultBuy;
    int m_weaponCount;
    std::vector<ItemConfig> m_items;
};
