# edAutoMarket v2.0 - Stronghold Crusader HD Auto Trade Mod

[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows_x86-lightgrey.svg)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**edAutoMarket v2.0** is an advanced *Auto Trading* (Auto Buy/Sell) modification for the game **Stronghold Crusader HD**. This mod allows you to set stock thresholds for goods in your granary/armory; if a resource stock exceeds the sell threshold, it will be automatically sold. If it falls below the buy threshold, it will be automatically purchased.

This mod is a major fork from **Ciptik's** original source code, focusing on keyboard-driven navigation efficiency, a dual profile (*Snapshot*) system, cleanly categorized items, and DLL proxy stability for full compatibility with other popular modifications.

---
![Screenshoot-edAutoMarketv2](https://github.com/CuedNub/edAutoMarketv2/blob/main/screenshot.jpg?raw=true)

## ✨ Key Features (v2.0)

* ⌨️ **Full Keyboard-Driven UI:** Complete navigation and number input using the keyboard, requiring zero mouse clicks.
* 🗂️ **Categorized Item List:** Items are neatly grouped into **Weapons (A-Z)** and **Resources (A-Z)** with clear display names.
* 📁 **Dual Config File System:**
  * `automarket.ini`: Stores main hotkey settings and default values.
  * `automarketsave.ini`: Stores instant threshold *snapshot* profiles that can be loaded/saved mid-game.
* 🔄 **Silent Auto-Reset:** Thresholds are automatically reset to defaults when a match/skirmish ends (returning to the main menu) without any intrusive notifications.
* 🛡️ **DirectDraw Proxy Architecture (`ddraw.dll`):** Uses a pure *DLL Proxy Wrapper* technique, making it **100% crash-free** and **fully compatible with SHCPlayer** (`dplayx.dll`).

---

## 📥 Download & Installation (No Compile Required)

For players who simply want to use this mod in-game without compiling the source code:

### 1. Download the Mod File
* Open the official GitHub release page: 👉 **[edAutoMarket v2.0 Releases](https://github.com/CuedNub/edAutoMarketv2/releases)**
* In the **Assets** section of the latest version, download **`ddraw.zip`**.

### 2. Installation Steps
1. Open your *Stronghold Crusader HD* installation folder.  
   *(Default location example: `C:\Program Files (x86)\R.G. Mechanics\Stronghold Crusader HD\`)*
2. Extract `ddraw.zip` and copy/move the **`ddraw.dll`** file into the game's root directory (next to `Stronghold_Crusader.exe`).
3. Done! The mod is now installed.

### 3. How to Use In-Game
1. Launch *Stronghold Crusader HD* as usual.
2. Enter a game mode (Skirmish / Campaign).
3. Press **`M`** on your keyboard to open the *Auto Market* menu.
4. Use the arrow keys **`↑ / ↓`** to select an item, press **`TAB`** to switch between the Sell/Buy columns, then type the threshold value directly.
5. Press **`CTRL + SHIFT + S`** to save your settings to the profile file (`automarketsave.ini`).

---

## 📜 About the Original Source Code (Ciptik/crusader-automarket)

The base source code of this modification is derived from this GitHub repository:
👉 **[https://github.com/Ciptik/crusader-automarket](https://github.com/Ciptik/crusader-automarket)**

* **Ciptik's Original Functionality:** Hooks the game's DirectDraw functions to render the menu and uses *memory reading/writing* via MinHook to trigger market transactions without any mouse cursor movement. Originally used `dplayx.dll`.
* **What's New in edAutoMarket v2.0:** 
  1. Complete UI redesign from a mouse-button system into a *Keyboard-Driven Table*.
  2. Added a *Dual Config System* (`automarket.ini` & `automarketsave.ini`).
  3. Added *Session-Aware* logic (Auto-Reset when a match ends).
  4. Migrated from `dplayx.dll` to `ddraw.dll` with *export function Wrappers* to support *multi-modding*.

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

| Hotkey | Function / Description |
| :--- | :--- |
| `M` | Opens or closes the Auto Market menu in-game. |
| `↑` / `↓` | (In menu) Selects the item row up or down. |
| `TAB` | (In menu) Switches focus between the "Sell >" and "Buy <" columns. |
| `0 - 9` | (In menu) Types the sell/buy threshold value directly. |
| `Enter` | (In menu) Confirms the number you just typed. |
| `Esc` | (In menu) Cancels typing or closes the menu. |
| `CTRL + SHIFT + S` | **Save Snapshot:** Saves the current sell/buy configuration to `automarketsave.ini`. The menu will close and a "Saved!" notification appears. |
| `CTRL + SHIFT + L` | **Load Snapshot:** Instantly reloads values from `automarketsave.ini` without needing to restart the game ("Loaded!"). |
| `CTRL + R` (2x) | **Reset All:** Returns all values to their *defaults*. Press twice in a row to confirm. |
| `CTRL + L` | **Reload Config:** Re-reads the `automarket.ini` file from disk. |
| *(Automatic)* | When a match/skirmish ends (win/lose/quit), the mod will **automatically reset** all values to *defaults*. |

---

## ⚙️ How to Change Hotkeys

All *hotkeys* can be customized without needing to recompile. Simply edit the **`automarket.ini`** file (located in your game folder) using Notepad.

```ini
[Hotkeys]
; Format: Hex Virtual-Key Code (0x..)
; Modifier: 1 = Active, 0 = Inactive

SaveConfig=0x53        ; 0x53 = 'S' key
SaveConfig_Ctrl=1      ; Requires CTRL
SaveConfig_Shift=1     ; Requires SHIFT
```

### 📝 Virtual-Key Codes Reference (Hex)
* `0x4D` = M  |  `0x4C` = L  |  `0x52` = R  |  `0x53` = S
* `0x41` = A  ... through ... `0x5A` = Z
* `0x70` = F1 |  `0x71` = F2 ... through ... `0x7B` = F12
* `0x20` = Spacebar

*(Change the code as needed, then save the `.ini` file. In-game, press **`CTRL + L`** to instantly apply the new hotkey settings).*

---

## 🛠️ Build / Compile Requirements (For Developers)

If you want to modify the source code (`.cpp` / `.hpp`) and compile it yourself:

1. **Windows** OS (64-bit / 32-bit).
2. **MSYS2** (Must use the **MSYS2 MINGW32** terminal). Download at: [msys2.org](https://www.msys2.org/).

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
