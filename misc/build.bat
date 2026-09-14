@echo off

set PDB_FILE_NAME=GAME_%RANDOM%.pdb
set EXE_NAME=Game
set code_path=..\code\
set third_party_path=..\third_party\
set source_assets=..\assets\

:: COMPILER FLAGS
:: -std:c++20 : Target the C++20 standard
:: -nologo    : Suppress the compiler startup banner
:: -Od        : Disable optimization for faster debug builds
:: -Gm-       : Disable minimal rebuild (deprecated in modern MSVC)
:: -GR-       : Disable Run-Time Type Info (RTTI)
:: -EHa-      : Disable C++ exception handling
:: -W4        : Enable high-level warnings
:: -WX        : Treat all warnings as errors
set compiler=-std:c++20 -nologo -Od -Gm- -GR- -EHa- -W4 -WX

:: SUPPRESSED WARNINGS
:: 4201: Nameless struct/union        | 4100: Unused parameter 
:: 4189: Unused local variable        | 4505: Unreferenced local function 
:: 4456: Variable shadowing           | 4127: Constant conditional
set compiler=%compiler% -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4127

set compiler=%compiler% -I%third_party_path%include

:: DEBUG & DEFINES
:: -FC : Output full absolute paths in diagnostics (crucial for 4coder jump-to-error)
:: -Zi : Generate complete debugging information (.pdb)
set debug=-FC -Zi
set defines=-DENGINE_DEBUG=1

:: LIBRARIES
set win32_libs=user32.lib gdi32.lib winmm.lib shell32.lib
set sdl_libs=SDL2.lib SDL2_ttf.lib SDL2_image.lib opengl32.lib

:: LINKER FLAGS
:: -opt:ref           : Strip unreferenced functions and data
:: -incremental:no    : Force full links for cleaner builds
:: -subsystem:console : Build as a console application
:: -DEBUG:FULL        : Emit full debug symbols for the PDB
set link=-opt:ref -incremental:no -subsystem:console -DEBUG:FULL

if not exist .\build mkdir .\build
pushd .\build

:: Copy shared libraries and assets silently (/q), preserving structure (/e /s), overwriting automatically (/y)
if exist %third_party_path%dll (
    xcopy "%third_party_path%dll\*.dll" "." /q /v /y
)
if exist "%source_assets%" (
    xcopy "%source_assets%" ".\assets\" /e /s /y
)

:: Clean previous build artifacts silently
del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL
del *.rdi > NUL 2> NUL

:: BUILD EXECUTABLE
cl %compiler% %defines% %debug% %code_path%game.cpp ^
   /I%third_party_path%include %sdl_libs% %win32_libs% ^
   /link %link% /LIBPATH:%third_party_path%lib ^
   /PDB:%PDB_FILE_NAME% /OUT:%EXE_NAME%.exe

popd