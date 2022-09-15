F1 Race
=======

![Screenshot from Windows 10](images/screenshot-windows.png)

Port of the game from MTK OS to SDL2 and Emscripten.

## Controls

* Arrows and 4, 6, 8, 2 on Keypad – Movement.
* Space, Return and 5 on Keypad – Fly.

## Web Demo

* [F1 Race](https://baat.exlmoto.ru/f1/).

## Build for Windows

Install [MSYS2](https://www.msys2.org/) first and run "MSYS2 MINGW64" shell.

```sh
$ pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer
mingw32-make build-windows
mingw32-make build-windows-static # Alternative, maybe broken over time.
```

## Build for Linux

```sh
$ sudo apt install build-essential libsdl2-dev libsdl2-mixer-dev
$ make build-linux
```

## Build for Web

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) first.

```sh
$ source /opt/emsdk/emsdk_env.sh
$ make build-web
```

## TODO

* Implement more detailed Game Over screen with scores when car crashes.

## Other Information

Thanks to [@nehochupechatat](https://github.com/nehochupechatat) for source.
