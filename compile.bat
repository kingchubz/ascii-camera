set cur_dir=%~dp0

mkdir "%cur_dir%obj"
g++ -Wall -g  -c "%cur_dir%escapi.cpp" -o "%cur_dir%obj\escapi.o"

mkdir "%cur_dir%bin"
g++ -Wall -g  -c "%cur_dir%main.cpp" -o "%cur_dir%obj\main.o"

g++  -o "%cur_dir%bin\ASCIIcamera.exe" "%cur_dir%obj\escapi.o" "%cur_dir%obj\main.o" -O2 -static-libstdc++ -static-libgcc -lgdi32 -luser32 -lkernel32 -lcomctl32
copy "%cur_dir%escapi.dll" "%cur_dir%bin\escapi.dll"

PAUSE