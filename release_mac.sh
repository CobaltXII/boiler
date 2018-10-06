clang++ -o "$1.app" "$1.cpp" -std=c++11 -lSDL2 -Ofast
"./$1.app"