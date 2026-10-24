@echo off

REM Please change the visual studio directory path to the installed location on your system

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set path=C:\Users\pauls\Documents\dev\2d_physics_engine;%path%