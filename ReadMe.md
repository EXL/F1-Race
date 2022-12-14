F1 Race
=======

![Screenshot from Windows 10](images/screenshot-windows.png)

A port of the simple race game from MTK OS to SDL2 and Emscripten.

## Controls

* Arrows and 4, 6, 8, 2 on Keypad – Movement.
* Space, Return and 5 on Keypad – Fly.
* Tab, N and 0 on Keypad – Switch MIDI.
* M and 7 on Keypad – Mute sound.

## Web Demo

* [F1 Race](https://lab.exlmoto.ru/f1/) page.

## Download

Ready-to-run game builds are available in the "[Releases](https://github.com/EXL/F1-Race/releases)" section.

## Run Tips

Linux, Ubuntu 22.04 LTS: Install the SDL2 and SDL2_mixer libraries before starting the game.

```sh
$ sudo apt install libsdl2-2.0-0 libsdl2-mixer-2.0-0
$ ./F1-Race
```

Web: Add a MIME type for WASM to serve files properly.\
More information about this here: [Hosting a WebAssembly App](https://platform.uno/docs/articles/how-to-host-a-webassembly-app.html).

Nginx: Add this to the `http` section of the configuration file:

```nginx
types {
    application/wasm wasm;
}
```

Apache: Add this to the `apache2.conf` or `httpd.conf` configuration files:

```apache
AddType application/wasm .wasm
```

## Build for Windows

Install [MSYS2](https://www.msys2.org/) first and run "MSYS2 MINGW64" shell.

```sh
$ pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-gcc
$ pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer
$ mingw32-make build-windows
$ mingw32-make build-windows-static # Alternative, maybe broken over time.
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

## Other Information

Thanks to [@nehochupechatat](https://github.com/nehochupechatat) for source and [@OldPhonePreservation](https://twitter.com/oldphonepreserv) for original MTK MIDIs.
