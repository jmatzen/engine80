setlocal

PATH=C:\depot\engine80\vcpkg\downloads\tools\cmake-3.27.1-windows\cmake-3.27.1-windows-i386\bin;%PATH%
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:\depot\engine80\vcpkg\scripts\buildsystems\vcpkg.cmake
