# This Makefile was created by EXL: 14-Sep-2022
# Edited: 15-Sep-2022 (add windows support using MSYS2)

all: build-linux

build-linux:
	$(CC) -O2 F1-Race.c -o F1-Race -lSDL2 -lSDL2_mixer
	strip -s F1-Race

build-windows:
	windres -i windows/F1-Race.rc -o F1-Race_res.o --include-dir=.
	$(CC) -O2 F1-Race.c -o F1-Race.exe F1-Race_res.o `sdl2-config --libs` -lSDL2_mixer
	strip -s F1-Race.exe

build-windows-static:
	windres -i windows/F1-Race.rc -o F1-Race_res.o --include-dir=.
	$(CC) -static -static-libgcc -O2 F1-Race.c -o F1-Race.exe F1-Race_res.o \
		`sdl2-config --static-libs` -lSDL2_mixer -lwinmm -lmpg123 -lopusfile -logg -lopus -lshlwapi -lssp
	strip -s F1-Race.exe

build-web:
	emcc -02 --use-preload-plugins --preload-file assets F1-Race.c -s USE_SDL=2 -s USE_SDL_MIXER=2 -o F1-Race.html
	emstrip -s F1-Race.wasm

clean:
	-rm -f F1-Race
	-rm -f F1-Race.o
	-rm -f F1-Race.obj
	-rm -f F1-Race_res.o
	-rm -f F1-Race.exe
	-rm -f F1-Race.data
	-rm -f F1-Race.html
	-rm -f F1-Race.wasm
	-rm -f F1-Race.js
