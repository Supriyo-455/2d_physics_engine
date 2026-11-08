@echo off
setlocal

:: =============================================================================
:: PROJECT PATHS & DEFINES
:: =============================================================================
set EXE_NAME=Game
set DLL_NAME=game
set code_path=..\code\
set third_party_path=..\third_party\
set source_assets=..\assets\

:: =============================================================================
:: COMPILER FLAGS
:: =============================================================================
:: -std:c++20 : Target C++20 standard
:: -nologo    : Suppress compiler banner
:: -Od        : Disable optimizations for fast builds and clean debugging
:: -Gm-       : Disable minimal rebuild
:: -GR-       : Disable RTTI
:: -EHa-      : Disable C++ exceptions
:: -W4 -WX    : High warnings as errors
:: -Zi -FC    : Full debug info + full paths for editor error jumping
set common_compiler=-std:c++20 -nologo -Od -Gm- -GR- -EHa- -W4 -WX -Zi -FC
set common_compiler=%common_compiler% -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4127 -wd4996
set common_compiler=%common_compiler% -I%third_party_path%include -I%code_path%

set defines=-DENGINE_DEBUG=1

:: =============================================================================
:: LIBRARIES & LINKER FLAGS
:: =============================================================================
set win32_libs=user32.lib gdi32.lib winmm.lib shell32.lib
set sdl_libs=SDL2.lib SDL2_ttf.lib SDL2_image.lib opengl32.lib
set common_link=-opt:ref -incremental:no -DEBUG:FULL

if not exist .\build mkdir .\build
pushd .\build

:: Copy dependencies and assets
if exist %third_party_path%dll (
    xcopy "%third_party_path%dll\*.dll" "." /q /v /y > NUL 2> NUL
)
if exist "%source_assets%" (
    xcopy "%source_assets%" ".\assets\" /e /s /y > NUL 2> NUL
)

:: =============================================================================
:: 1. COMPILE GAME DLL (Live Reloadable Logic)
:: =============================================================================
:: Create lock file so the running game doesn't load a half-linked DLL
echo WAITING FOR PDB > lock.tmp

set DLL_PDB_NAME=game_%RANDOM%.pdb
echo [BUILD] Compiling %DLL_NAME%.dll...
cl %common_compiler% %defines% %code_path%game.cpp ^
   -LD /link %common_link% /PDB:%DLL_PDB_NAME% /EXPORT:GameUpdateAndRender ^
   /OUT:%DLL_NAME%.dll

:: Remove lock file once link is fully finished
del lock.tmp > NUL 2> NUL

:: =============================================================================
:: 2. COMPILE PLATFORM HOST EXECUTABLE (Game.exe)
:: =============================================================================
:: Only compile Game.exe if it doesn't exist, or if explicitly requested (or pass "all")
if "%1"=="game" goto finish

set EXE_PDB_NAME=GAME_%RANDOM%.pdb
echo [BUILD] Compiling %EXE_NAME%.exe...
cl %common_compiler% %defines% %code_path%sdl_main.cpp ^
   %sdl_libs% %win32_libs% ^
   /link %common_link% -subsystem:console /LIBPATH:%third_party_path%lib ^
   /PDB:%EXE_PDB_NAME% /OUT:%EXE_NAME%.exe

:finish
popd
echo [BUILD] Complete.