windres wingui.rc -O coff -o build\wingui.res
g++ -std=c++17 -mwindows -fdiagnostics-color=always -o "build\wingui-debug.exe" -g "wingui.cpp" "build\wingui.res"