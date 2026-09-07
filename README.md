Berikut adalah isi file **`README.md`** yang sudah disempurnakan secara profesional, rapi, dan siap langsung Anda *copy-paste* ke Notepad++:

***

# edAutoMarket v2.0 - Stronghold Crusader HD Auto Trade Mod

[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows_x86-lightgrey.svg)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**edAutoMarket v2.0** adalah modifikasi *Auto Trading* (Jual/Beli Otomatis) tingkat lanjut untuk game **Stronghold Crusader HD**. Mod ini memungkinkan Anda menetapkan batas (*threshold*) jumlah stok barang di gudang/gudang senjata; jika jumlah barang melebihi batas jual, mod akan otomatis menjualnya. Jika kurang dari batas beli, mod otomatis membelinya.

Mod ini merupakan pengembangan besar-besaran (*Fork*) dari *source code* asli milik **Ciptik** dengan fokus pada efisiensi navigasi *keyboard*, sistem profil ganda (*Snapshot*), kategorisasi item yang rapi, dan stabilitas proxy DLL agar kompatibel penuh dengan modifikasi populer lainnya.

---

## ✨ Fitur Utama (v2.0)

* ⌨️ **Full Keyboard-Driven UI:** Seluruh navigasi dan pengisian angka menggunakan *keyboard* tanpa perlu interaksi klik *mouse*.
* 🗂️ **Daftar Item Terkategori:** Barang dikelompokkan dengan rapi berdasarkan **Weapons (A-Z)** dan **Resources (A-Z)** menggunakan nama tampilan yang jelas.
* 📁 **Sistem Konfigurasi File Ganda:**
  * `automarket.ini`: Menyimpan pengaturan utama *hotkey* dan nilai *default*.
  * `automarketsave.ini`: Menyimpan profil *snapshot* threshold yang bisa di-load/save secara instan di tengah permainan.
* 🔄 **Silent Auto-Reset:** Threshold otomatis ter-reset kembali ke *default* saat pertandingan/skirmish berakhir (kembali ke menu utama) tanpa menampilkan notifikasi yang mengganggu.
* 🛡️ **DirectDraw Proxy Architecture (`ddraw.dll`):** Menggunakan teknik *DLL Proxy Wrapper* murni sehingga **100% aman dari crash** dan **kompatibel penuh dengan SHCPlayer** (`dplayx.dll`).

---

## 📜 Tentang Source Code Asli (Ciptik/crusader-automarket)

Source code dasar dari modifikasi ini diambil dari repositori GitHub:
👉 **[https://github.com/Ciptik/crusader-automarket](https://github.com/Ciptik/crusader-automarket)**

* **Fungsi Asli Ciptik:** Membajak fungsi DirectDraw game untuk menggambar menu dan menggunakan *memory reading/writing* via MinHook untuk memicu transaksi pasar tanpa pergerakan kursor mouse. Menggunakan file `dplayx.dll`.
* **Perubahan pada edAutoMarket v2.0:** 
  1. Redesign total UI dari sistem tombol mouse menjadi *Keyboard-Driven Table*.
  2. Penambahan *Dual Config System* (`automarket.ini` & `automarketsave.ini`).
  3. Penambahan logika *Session-Aware* (Auto-Reset saat match selesai).
  4. Migrasi dari `dplayx.dll` ke `ddraw.dll` dengan *export function Wrapper* untuk mendukung *multi-modding*.

---

## 🤝 Kompatibilitas dengan SHCPlayer

Mod ini **TIDAK MEMBUTUHKAN** SHCPlayer untuk berjalan (bisa berfungsi murni mandiri). 
Namun, mod ini telah dirancang khusus agar **100% KOMPATIBEL dan dapat berjalan bersamaan dengan SHCPlayer v1.8.3+** tanpa konflik *Loader Lock* atau crash.

Jika Anda menginginkan fitur tambahan seperti *Shift+Click rekrut 10 unit*, pengatur kecepatan *replay*, atau kustomisasi *minimap*, Anda bisa mengunduh SHCPlayer:
👉 **Link Download SHCPlayer (ModDB):** [SHCPlayer v1.8.3 Download](https://www.moddb.com/mods/shcplayer/downloads/shcplayer)

> **💡 Cara Penggunaan Bersamaan:**
> * Biarkan file mod SHCPlayer bernama **`dplayx.dll`** di folder game Anda.
> * Pasang file mod edAutoMarket bernama **`ddraw.dll`** di folder game yang sama.
> * Kedua mod akan berjalan berdampingan secara sempurna!

---

## 📦 Cara Instalasi (Untuk Pemain)

1. Unduh file **`ddraw.dll`** rilis terbaru.
2. Salin/Pindahkan file `ddraw.dll` ke folder utama game Stronghold Crusader HD Anda (contoh: `C:\Program Files (x86)\R.G. Mechanics\Stronghold Crusader HD\`).
3. Jalankan game seperti biasa.
4. Masuk ke pertempuran (Skirmish/Campaign) dan tekan tombol **`M`** untuk membuka menu mod.

---

## ⌨️ Daftar Hotkey Default

| Hotkey | Fungsi / Keterangan |
| :--- | :--- |
| `M` | Membuka atau menutup menu Auto Market di dalam game. |
| `↑` / `↓` | (Di dalam menu) Memilih baris item ke atas / ke bawah. |
| `TAB` | (Di dalam menu) Berpindah antara kolom "Sell >" dan "Buy <". |
| `0 - 9` | (Di dalam menu) Mengetik angka batas jual/beli langsung. |
| `Enter` | (Di dalam menu) Mengonfirmasi angka yang baru diketik. |
| `Esc` | (Di dalam menu) Membatalkan pengetikan atau menutup menu. |
| `CTRL + SHIFT + S` | **Save Snapshot:** Menyimpan konfigurasi jual/beli saat ini ke `automarketsave.ini`. Menu akan tertutup dan muncul notifikasi "Saved!". |
| `CTRL + SHIFT + L` | **Load Snapshot:** Memuat kembali angka dari `automarketsave.ini` secara instan tanpa perlu merestart game ("Loaded!"). |
| `CTRL + R` (2x) | **Reset All:** Mengembalikan semua angka ke nilai *default*. Tekan dua kali berturut-turut untuk konfirmasi. |
| `CTRL + L` | **Reload Config:** Membaca ulang file `automarket.ini` dari disk. |
| *(Otomatis)* | Saat *match/skirmish* berakhir (menang/kalah/keluar), mod akan **otomatis me-reset** semua nilai ke *default*. |

---

## ⚙️ Cara Mengubah Hotkey

Semua *hotkey* dapat disesuaikan tanpa perlu melakukan *compile* ulang. Edit file **`automarket.ini`** yang berada di folder game Anda menggunakan Notepad.

```ini
[Hotkeys]
; Format: Kode Virtual-Key Hex (0x..)
; Modifier: 1 = Aktif, 0 = Non-aktif

SaveConfig=0x53        ; 0x53 = Tombol 'S'
SaveConfig_Ctrl=1      ; Menggunakan CTRL
SaveConfig_Shift=1     ; Menggunakan SHIFT
```

### 📝 Referensi Kode Tombol (Virtual-Key Codes Hex)
* `0x4D` = M  |  `0x4C` = L  |  `0x52` = R  |  `0x53` = S
* `0x41` = A  ... sampai ... `0x5A` = Z
* `0x70` = F1 |  `0x71` = F2 ... sampai ... `0x7B` = F12
* `0x20` = Spacebar

*(Ubah kode sesuai kebutuhan, lalu simpan file `.ini`. Di dalam game, tekan **`CTRL + L`** untuk memperbarui tombol secara langsung).*

---

## 🛠️ Persyaratan Build / Compile (Untuk Developer)

Jika Anda ingin mengubah kode sumber (`.cpp` / `.hpp`) dan mengompilasinya sendiri:

1. OS **Windows** (64-bit / 32-bit).
2. **MSYS2** (Wajib menggunakan terminal **MSYS2 MINGW32**). Download di: [msys2.org](https://www.msys2.org/).

---

## 💻 Cara Build & Compile (Instruksi MSYS2)

**1. Buka MSYS2 MINGW32**
Jalankan `mingw32.exe` (bukan MSYS2 MSYS atau MINGW64).

**2. Instalasi Toolchain (Hanya 1x)**
```bash
pacman -S git mingw-w64-i686-gcc mingw-w64-i686-make mingw-w64-i686-cmake
```

**3. Clone Repository**
```bash
git clone --recurse-submodules https://github.com/CuedNub/edAutoMarketv2.git
cd edAutoMarketv2
```

**4. Proses Kompilasi**
```bash
mkdir -p build
cd build
rm -rf *
cmake -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++ -Wl,--kill-at" ..
mingw32-make
```

**5. Hasil Kompilasi**
File **`ddraw.dll`** akan terbentuk di dalam folder `build/`. Salin file tersebut ke folder game Stronghold Crusader HD Anda.

---

## 📄 Lisensi

Proyek ini dilisensikan di bawah **MIT License** - lihat file [LICENSE](LICENSE) untuk detail selengkapnya.

---
**Developed with ❤️ by CuedNub**