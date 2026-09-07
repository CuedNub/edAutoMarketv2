#!/bin/bash
# Masuk ke folder build
mkdir -p build
cd build

# Konfigurasi CMake jika belum
if [ ! -f "Makefile" ]; then
    cmake -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++" ..
fi

# Compile
mingw32-make

# Jika compile sukses, langsung copy ke folder game
if [ $? -eq 0 ]; then
    echo "----------------------------------------"
    echo "Compile SUKSES! Menyalin ke folder game..."
    cp dplayx.dll "/c/Program Files (x86)/R.G. Mechanics/Stronghold Crusader HD/"
    echo "dplayx.dll berhasil dipasang ke Game!"
    echo "----------------------------------------"
else
    echo "Compile GAGAL! Periksa error di atas."
fi
