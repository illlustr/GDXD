# GDXD

an alternative to typing `executable.exe -v -d` in the terminal or script for GDX / Godot in windows.

## Usage

This executable automatically searches for a program with a similar name in the same folder. To distinguish this executable from the target program, use a `.` as a separator in the filename.

Example :
- yourexecutable.justToSeparate.exe
- gdx.debug.exe

## compiling

### RC

````
windres GDXD.rc -o GDXD.o
````

### Binary

````
g++ -o GDXD.exe GDXD.cpp GDXD.o -Os -s
````
