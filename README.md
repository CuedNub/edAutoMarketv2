# edAutoMarket v2.1 - Stronghold Crusader HD Auto Trade Mod

[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows_x86-lightgrey.svg)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**edAutoMarket v2.1** is an advanced *Auto Trading* (Auto Buy/Sell) modification for the game **Stronghold Crusader HD**. Set stock thresholds for goods in your granary and armory — if a resource exceeds the sell threshold it will be automatically sold, and if it falls below the buy threshold it will be automatically purchased. All navigation is done entirely with the keyboard, without any mouse clicks.

This mod is a major fork of **Ciptik's** original source code, focusing on keyboard-driven efficiency, a clean **config / preset separation**, full UI theme customization with an authentic **Stronghold Crusader HD color scheme**, and DirectDraw proxy stability for 100% compatibility with other popular modifications such as SHCPlayer.

---

![Screenshoot-edAutoMarketv2](https://github.com/CuedNub/edAutoMarketv2/blob/main/screenshot.jpg?raw=true)

---

## 🆕 What's New in v2.1

* 🎨 **Authentic Stronghold Crusader HD Color Theme** — Warm parchment brown, wooden panels, and Crusader gold trim baked in as default.
* 🔒 **Full Game Input Blocking** — When the menu is open, **all** keyboard and mouse input is intercepted so the game's built-in hotkeys (map scroll, unit selection, etc.) are frozen. No more accidental map panning while editing values.
* ⏩ **Hold-to-Scroll Navigation** — `↑` / `↓` arrow keys now support keyboard auto-repeat. Hold them down to smoothly scroll through the item list.
* 🔤 **Jump-to-Letter Navigation** — Press any letter key (A–Z) to jump to an item starting with that letter. Press the same letter again to cycle through the next match (e.g., `W` → Wheat → Wood → Wheat…).
* 📁 **Renamed Config Files** — `automarket.ini` → **`ddraw.ini`**, `automarketsave.ini` → **`ddrawsave.ini`** for cleaner file naming that matches the DLL.
* 🧹 **Clean Config Separation** — `ddraw.ini` now holds **only** settings (hotkeys, UI, colors, defaults). All item thresholds live exclusively in `ddrawsave.ini` as a preset file.
* 💬 **Self-Documented Config** — `ddraw.ini` is now auto-generated with inline comments explaining every single key.
* 🛡️ **Safer Reset** — `CTRL+R` (Reset All) only resets values in memory; your `ddrawsave.ini` preset file is **never** overwritten by the reset action.

---

## ✨ Core Features

* ⌨️ **Full Keyboard-Driven UI** — Complete navigation and number input using the keyboard, zero mouse clicks required.
* 🗂️ **Categorized Item List** — Items are neatly grouped into **Weapons (A–Z)** and **Resources (A–Z)** with clear display names.
* 📁 **Config + Preset File System:**
  * **`ddraw.ini`** — Main configuration file: hotkeys, UI theme, fonts, colors, defaults.
  * **`ddrawsave.ini`** — Preset file: only stores your saved sell/buy thresholds. Can be loaded/saved mid-game.
* 🎨 **Fully Customizable UI Theme** — Customize menu colors (RGB), font family, font sizes, menu width, row height, and screen position offsets via `ddraw.ini`.
* ⚡ **Adjustable Trade Frequency** — Control how fast the mod checks and executes market trades in milliseconds (`TradeFrequencyMs`).
* 🔄 **Silent Auto-Reset** — Thresholds are automatically reset to defaults when a match/skirmish ends (returning to the main menu), without any intrusive notifications.
* 🛡️ **DirectDraw Proxy Architecture (`ddraw.dll`)** — Uses a pure *DLL Proxy Wrapper* technique, making it **100% crash-free** and **fully compatible with SHCPlayer** (`dplayx.dll`).

---

## 📥 Download & Installation (No Compile Required)

For players who simply want to use this mod in-game without compiling the source code:

### 1. Download the Mod File
* Open the official GitHub release page: 👉 **[edAutoMarket Releases](https://github.com/CuedNub/edAutoMarketv2/releases)**
* In the **Assets** section of the latest version, download **`ddraw.zip`**.

### 2. Installation Steps
1. Open your *Stronghold Crusader HD* installation folder.  
   *(Default location example: `C:\Program Files (x86)\R.G. Mechanics\Stronghold Crusader HD\`)*
2. Extract `ddraw.zip` and copy/move the **`ddraw.dll`** file into the game's root directory (next to `Stronghold_Crusader.exe`).
3. Done! The mod is now installed.

> 💡 **Upgrading from v2.0?** Delete the old `automarket.ini` and `automarketsave.ini` files before running the new version. New config files will be regenerated automatically.

### 3. How to Use In-Game
1. Launch *Stronghold Crusader HD* as usual.
2. Enter a game mode (Skirmish / Campaign).
3. Press **`M`** on your keyboard to open the *Auto Market* menu.
4. Navigate the list:
   * Use **`↑ / ↓`** (hold to scroll fast) to select an item.
   * Press any letter **A–Z** to jump to an item starting with that letter (press again to cycle).
5. Press **`TAB`** or **`← / →`** to switch between the Sell/Buy columns.
6. Type the threshold value directly (0–9999), press **`Enter`** to confirm.
7. Press **`CTRL + SHIFT + S`** to save your preset to `ddrawsave.ini`.

---

## 📜 About the Original Source Code (Ciptik/crusader-automarket)

The base source code of this modification is derived from this GitHub repository:
👉 **[https://github.com/Ciptik/crusader-automarket](https://github.com/Ciptik/crusader-automarket)**

* **Ciptik's Original Functionality:** Hooks the game's DirectDraw functions to render the menu and uses *memory reading/writing* via MinHook to trigger market transactions without any mouse cursor movement. Originally used `dplayx.dll`.
* **What's New in edAutoMarket v2.x:** 
  1. Complete UI redesign from a mouse-button system into a *Keyboard-Driven Table*.
  2. Added a *Config + Preset System* (`ddraw.ini` & `ddrawsave.ini`).
  3. Added *Session-Aware* logic (Auto-Reset when a match ends).
  4. Full UI customization (colors, fonts, sizes, offsets, trade frequency).
  5. Migrated from `dplayx.dll` to `ddraw.dll` with *export function Wrappers* to support *multi-modding*.
  6. Full game input blocking, hold-to-scroll, jump-to-letter, and native Stronghold Crusader HD theme (v2.1).

---

## 🤝 Compatibility with SHCPlayer

This mod **DOES NOT REQUIRE** SHCPlayer to function (it runs 100% standalone). 
However, it has been specifically designed to be **100% COMPATIBLE and can run alongside SHCPlayer v1.8.3+** without triggering *Loader Lock* conflicts or crashes.

If you want additional features such as *Shift+Click to recruit 10 units*, replay speed controls, or minimap customization, you can download SHCPlayer:
👉 **SHCPlayer Download Link (ModDB):** [SHCPlayer v1.8.3 Download](https://www.moddb.com/mods/shcplayer/downloads/shcplayer)

> **💡 How to Use Both Mods Together:**
> * Keep the SHCPlayer mod file named **`dplayx.dll`** in your game folder.
> * Install the edAutoMarket mod file named **`ddraw.dll`** in the same game folder.
> * Both mods will run side-by-side seamlessly!

---

## ⌨️ Default Hotkey List

### Global Hotkeys (Always Active)

| Hotkey | Function / Description |
| :--- | :--- |
| `M` | Opens or closes the Auto Market menu in-game. |
| `CTRL + SHIFT + L` | **Load Preset:** Instantly reloads item thresholds from `ddrawsave.ini` without restarting the game ("Loaded!"). |
| `CTRL + L` | **Reload Config:** Re-reads `ddraw.ini` from disk and applies new hotkeys, colors, fonts, and layout instantly. |

### In-Menu Hotkeys (Only Active When Menu Is Open)

| Hotkey | Function / Description |
| :--- | :--- |
| `↑` / `↓` | Selects the item row up or down. **Hold to scroll continuously.** |
| `Home` / `End` | Jump to the first / last item in the list. |
| `PageUp` / `PageDown` | Jump 5 rows up / down. |
| `A – Z` | **Jump to Letter:** Jumps cursor to the first item starting with that letter. Press again to cycle through the next match. |
| `TAB` or `← / →` | Switches focus between the "Sell >" and "Buy <" columns. |
| `0 – 9` | Starts typing a sell/buy threshold value directly. |
| `Enter` | Confirms the number you just typed. |
| `Backspace` | Deletes the last digit typed. |
| `Delete` | Clears the entire input buffer. |
| `Esc` | Cancels typing or closes the menu. |
| `CTRL + SHIFT + S` | **Save:** Writes current values to `ddrawsave.ini` and updates `ddraw.ini`. Shows a "Saved!" notification and closes the menu. |
| `CTRL + R` (2x) | **Reset All:** Resets all thresholds in memory to default values. Press twice within 2 seconds to confirm. **Does NOT modify `ddrawsave.ini`.** |
| *(Automatic)* | When a match/skirmish ends (win/lose/quit), the mod will **automatically reset** all values to *defaults*. |

> 🔒 **Input Blocking:** While the menu is open, **all** keyboard and mouse events are blocked from reaching the game. Your map won't scroll and units won't get selected while you edit values.

---

## ⚙️ Configuration Files Explained

The mod uses **two separate files** — one for settings and one for saved presets:

### 📄 `ddraw.ini` — Configuration File

Contains hotkeys, UI colors, fonts, and default values. This file is **auto-generated with helpful inline comments** the first time you press `CTRL+SHIFT+S`. Press **`CTRL + L`** in-game to reload changes on the fly.

Sections included:
* `[Hotkeys]` — All keyboard shortcuts.
* `[Advanced]` — Trade frequency in milliseconds.
* `[UI]` — Menu dimensions, fonts, and colors (RGB).
* `[Defaults]` — The starting sell/buy values used when no preset is loaded.

### 📄 `ddrawsave.ini` — Preset File

Contains **only** your saved sell/buy thresholds per item, split into `[Weapons]` and `[Resources]` sections. This file is:
* **Written** only when you press `CTRL + SHIFT + S`.
* **Loaded** automatically on game start (if it exists), and manually via `CTRL + SHIFT + L`.
* **Never touched** by the Reset All (`CTRL + R`) action, so your preset is always safe.

---

## 🎨 Sample `ddraw.ini` (Auto-Generated)

```ini
; ============================================
; edAutoMarket v2.1 - Configuration File
; ============================================
; This file is ONLY for configuration (hotkeys, UI, colors).
; Item sell/buy values are stored in ddrawsave.ini.
; Press CTRL+L in-game to reload without restart.
; ============================================

[Hotkeys]
; Format: decimal Virtual-Key Code
; Reference: 77=M  76=L  82=R  83=S  65-90=A-Z  112-123=F1-F12
; _Ctrl and _Shift: 1 = required, 0 = not required

; Toggle Auto Market menu (default: M)
ToggleMenu=77
ToggleMenu_Ctrl=0
ToggleMenu_Shift=0

; Save configuration + preset snapshot (default: CTRL+SHIFT+S)
SaveConfig=83
SaveConfig_Ctrl=1
SaveConfig_Shift=1

; Load preset from ddrawsave.ini (default: CTRL+SHIFT+L)
LoadSnapshot=76
LoadSnapshot_Ctrl=1
LoadSnapshot_Shift=1

; Reset all thresholds to defaults, press twice to confirm (default: CTRL+R)
ResetAll=82
ResetAll_Ctrl=1
ResetAll_Shift=0

; Reload ddraw.ini without restarting the game (default: CTRL+L)
ReloadConfig=76
ReloadConfig_Ctrl=1
ReloadConfig_Shift=0

[Advanced]
; Trade check interval in milliseconds (default: 100)
; Lower = faster trading, but more CPU usage
TradeFrequencyMs=100

[UI]
MenuWidth=400
RowHeight=22
OffsetX=0
OffsetY=0

FontName=Consolas
FontSize=16
TitleSize=20

; --- UI Colors (format: R,G,B) ---
; Default theme: Stronghold Crusader HD (wood brown & gold)

BgColor=58,40,22
HeaderColor=92,62,30
RowColor=72,50,28
SelColor=140,95,40
EditColor=170,110,35
CatColor=105,72,35

; --- Text Colors (format: R,G,B) ---

TextColor=245,220,155
HighlightTextColor=255,245,200
EditTextColor=255,255,120
TitleColor=255,200,80

[Defaults]
; Default sell/buy thresholds used at game start
; (when ddrawsave.ini does not exist)
DefaultSale=500
DefaultBuy=0
```

### 📝 Key Code Reference (Decimal VK Codes)
* `77` = M  |  `76` = L  |  `82` = R  |  `83` = S
* `65` = A  ... through ... `90` = Z
* `112` = F1 |  `113` = F2 ... through ... `123` = F12
* `32` = Spacebar

---

## 🛠️ Build / Compile Requirements (For Developers)

If you want to modify the source code (`.cpp` / `.hpp`) and compile it yourself:

1. **Windows** OS (64-bit / 32-bit).
2. **MSYS2** (must use the **MSYS2 MINGW32** terminal). Download at: [msys2.org](https://www.msys2.org/).

---

## 💻 How to Build & Compile (MSYS2 Instructions)

**1. Open MSYS2 MINGW32**  
Run `mingw32.exe` (not MSYS2 MSYS or MINGW64).

**2. Install Toolchain (One-Time Only)**
```bash
pacman -S git mingw-w64-i686-gcc mingw-w64-i686-make mingw-w64-i686-cmake
```

**3. Clone the Repository**
```bash
git clone --recurse-submodules https://github.com/CuedNub/edAutoMarketv2.git
cd edAutoMarketv2
```

**4. Compile Process**
```bash
mkdir -p build
cd build
rm -rf *
cmake -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++ -Wl,--kill-at" ..
mingw32-make
```

**5. Compilation Output**  
The **`ddraw.dll`** file will be generated inside the `build/` folder. Copy this file to your Stronghold Crusader HD game folder to use it.

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for full details.

---
**Developed with ❤️ by CuedNub**
