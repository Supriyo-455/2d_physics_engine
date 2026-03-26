@echo off

REM Please change the visual studio directory path to the installed location on your system

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=D:\projects\physics_engine;%path%