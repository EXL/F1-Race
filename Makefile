# This Makefile was created and edited by EXL, 14-Sep-2022

all: native emscripten

native:
	$(CC) -O2 F1-Race.c -o F1-Race -lSDL2 -lSDL2_mixer
	strip -s F1-Race

emscripten:
	emcc --use-preload-plugins --preload-file assets F1-Race.c -s USE_SDL=2 -s USE_SDL_MIXER=2 -o F1-Race.html

clean:
	-rm -f F1-Race
	-rm -f F1-Race.exe
	-rm -f F1-Race.data
	-rm -f F1-Race.html
	-rm -f F1-Race.wasm
	-rm -f F1-Race.js
