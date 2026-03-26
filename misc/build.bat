@echo off

:: ================================
:: PROJECT DEFINES
:: ================================
set PDB_FILE_NAME=GAME_%RANDOM%.pdb
set EXE_NAME=Game
set code_path=..\code\
set third_party_path=..\third_party\
set source_assets=..\assets\

:: ================================
:: COMPILER FLAGS
:: ================================
set compiler=-std:c++20 -nologo -Od -Gm- -GR- -EHa- -W4 -WX

:: IGNORE WARNINGS
set compiler=%compiler% -wd4201 -wd4100 -wd4189 -wd4505 -wd4456

:: INCLUDE PATHS
set compiler=%compiler% -I%third_party_path%include

:: ================================
:: DEBUG & DEFINES
:: ================================
set debug=-FC -Zi
set defines=-DENGINE_DEBUG=1

:: ================================
:: LIBRARIES
:: ================================
set win32_libs=user32.lib gdi32.lib winmm.lib shell32.lib
set sdl_libs=SDL2.lib SDL2_ttf.lib SDL2_image.lib opengl32.lib

:: ================================
:: LINKER
:: ================================
set link=-opt:ref -incremental:no -subsystem:console -DEBUG:FULL

:: ================================
:: BUILD DIR
:: ================================
if not exist .\build mkdir .\build

pushd .\build

if exist %third_party_path%dll (
	xcopy "%third_party_path%dll\*.dll" "." /q /v /y
)

if exist "%source_assets%" (
	xcopy "%source_assets%" ".\assets\" /e /s /y
)

del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL
del *.rdi > NUL 2> NUL

:: ================================
:: BUILD EXECUTABLE
:: ================================

cl %compiler% %defines% %debug% %code_path%game.cpp ^
   /I%third_party_path%include %sdl_libs% %win32_libs% ^
   /link %link% /LIBPATH:%third_party_path%lib ^
   /PDB:%PDB_FILE_NAME% /OUT:%EXE_NAME%.exe

popd