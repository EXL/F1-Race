# This Makefile was created by EXL: 14-Sep-2022
# Edited: 15-Sep-2022 (add windows support using MSYS2)

all: linux

linux:
	$(CC) -O2 F1-Race.c -o F1-Race -lSDL2 -lSDL2_mixer
	strip -s F1-Race

windows:
	$(CC) -O2 F1-Race.c -o F1-Race.exe `sdl2-config --libs` -lSDL2_mixer
	strip -s F1-Race.exe

windows-static:
	$(CC) -static -static-libgcc -O2 F1-Race.c -o F1-Race.exe `sdl2-config --static-libs` -lSDL2_mixer -lwinmm -lmpg123 -lopusfile -logg -lopus -lshlwapi -lssp
	strip -s F1-Race.exe

web:
	emcc --use-preload-plugins --preload-file assets F1-Race.c -s USE_SDL=2 -s USE_SDL_MIXER=2 -o F1-Race.html

clean:
	-rm -f F1-Race
	-rm -f F1-Race.exe
	-rm -f F1-Race.data
	-rm -f F1-Race.html
	-rm -f F1-Race.wasm
	-rm -f F1-Race.js
