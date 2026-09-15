#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct HotkeyConfig {
    UINT vkCode;
    bool ctrl;
    bool shift;
    bool alt;
};

struct ItemConfig {
    std::string name;
    std::wstring displayName;
    int saleThreshold;
    int buyThreshold;
    bool isWeapon;
};

struct UIConfig {
    int menuWidth;
    int rowHeight;
    int offsetX;
    int offsetY;
    std::wstring fontName;
    int fontSize;
    int titleSize;

    COLORREF bgColor;
    COLORREF headerColor;
    COLORREF rowColor;
    COLORREF selColor;
    COLORREF editColor;
    COLORREF catColor;

    COLORREF textColor;
    COLORREF selTextColor;
    COLORREF editTextColor;
    COLORREF titleColor;
};

class ConfigManager {
public:
    static ConfigManager& Instance();

    void Load(const std::wstring& iniPath);
    void Save(const std::wstring& iniPath);
    void ResetAll();
    void SaveSnapshot();
    void LoadSnapshot();

    std::wstring GetActivePresetName() const;
    void SetActivePreset(int idx);
    void SaveActivePreset();
    void AddPreset(const std::wstring& newName);
    void DeleteActivePreset();
    const std::vector<std::wstring>& GetPresetList() const { return m_presets; }
    int GetActivePresetIndex() const { return m_activePresetIdx; }
    void LoadPresetByName(const std::wstring& name);

    HotkeyConfig GetToggleMenu() const { return m_toggleMenu; }
    HotkeyConfig GetSaveConfig() const { return m_saveConfig; }
    HotkeyConfig GetLoadSnapshot() const { return m_loadSnapshot; }
    HotkeyConfig GetResetAll() const { return m_resetAll; }
    HotkeyConfig GetReloadConfig() const { return m_reloadConfig; }
    HotkeyConfig GetHelp() const { return m_help; }

    const std::vector<ItemConfig>& GetItems() const { return m_items; }
    void SetItemSale(int index, int value);
    void SetItemBuy(int index, int value);

    const UIConfig& GetUI() const { return m_ui; }
    int GetTradeFrequency() const { return m_tradeFrequency; }
    int GetDefaultSale() const { return m_defaultSale; }
    int GetDefaultBuy() const { return m_defaultBuy; }
    int GetWeaponCount() const { return m_weaponCount; }
    std::wstring GetIniPath() const { return m_iniPath; }

    static bool CheckHotkey(const HotkeyConfig& hk);
    static std::wstring HotkeyToString(const HotkeyConfig& hk);

private:
    ConfigManager();
    void InitDefaultItems();
    int ReadInt(const wchar_t* section, const wchar_t* key, int defaultVal, const std::wstring& path);
    std::wstring ReadString(const wchar_t* section, const wchar_t* key, const wchar_t* defaultVal, const std::wstring& path);
    COLORREF ReadColor(const wchar_t* section, const wchar_t* key, COLORREF defaultColor, const std::wstring& path);
    void WriteColor(const wchar_t* section, const wchar_t* key, COLORREF color, const std::wstring& path);
    
    void LoadPresetThresholds(const std::wstring& path, const std::wstring& presetName);
    void SavePresetThresholds(const std::wstring& path, const std::wstring& presetName);
    void LoadPresets();
    void SavePresetList();

    std::wstring m_iniPath;
    std::wstring m_savePath;
    
    HotkeyConfig m_toggleMenu, m_saveConfig, m_loadSnapshot, m_resetAll, m_reloadConfig, m_help;
    UIConfig m_ui;
    
    int m_tradeFrequency;
    int m_defaultSale, m_defaultBuy;
    int m_weaponCount;
    std::vector<ItemConfig> m_items;

    std::vector<std::wstring> m_presets;
    int m_activePresetIdx;
};
