#!/bin/bash
mkdir -p build
cd build

if [ ! -f "Makefile" ]; then
    cmake -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_SHARED_LINKER_FLAGS="-static-libgcc -static-libstdc++ -Wl,--kill-at" ..
fi

mingw32-make

if [ $? -eq 0 ]; then
    echo "----------------------------------------"
    echo "Compile SUKSES! Menyalin ddraw.dll ke folder game..."
    cp ddraw.dll "/c/Program Files (x86)/R.G. Mechanics/Stronghold Crusader HD/"
    echo "ddraw.dll berhasil dipasang ke Game!"
    echo "----------------------------------------"
else
    echo "Compile GAGAL! Periksa error di atas."
fi
