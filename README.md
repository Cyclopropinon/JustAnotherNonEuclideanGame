# JustAnotherNonEuclideanGame
JustAnotherNonEuclideanGame (yes, very creative name, I know) is a small coding adventure into gamedev of mine based on a [linux fork](https://github.com/Srinivasa314/NonEuclidean) of [NonEuclidean](https://github.com/HackerPoet/NonEuclidean).

## NonEuclidean
A NonEuclidean rendering engine for Win32 and SDL2, written in C++ OpenGL.
To see what this code is about, check out this video:
https://youtu.be/kEB11PQ9Eo8

## Source Code Dependencies
Win32 - Add glew-2.1.0 to the main directory 

SDL - Install glew, glut and SDL libraries

(packets `libglew-dev freeglut3 freeglut3-dev libsdl2-dev` on ubuntu)

## Building
Win32 - Open NonEuclidean.sln

SDL :
```sh
cd NonEuclidean
make all
./NonEuclidean
```

### Please note
i only develop on linux, so no guarantee it will work/compile on other OSs. Fixes are welcome!

## Controls
* **Mouse** - Look around
* **AWSD** - Movement
* **1 - 8** - Switch between different demo rooms
* **Alt + Enter** - Toggle Fullscreen
* **Esc** - Exit demo

## TODO
* **HELP WANTED!** Rendering a string somehow doesn't work, and i haven't been able to fix it (see Engine::RenderString in Engine.cpp)
* understand the code lol
* Logfile
* Coordinate display
* fix memory leak when switching levels
* add settings file
* more levels
* level editor
* fps monitor

## Done
* upgrade to modern C++ versions (C++23)

