@echo off

:: ================================
:: PROJECT DEFINES
:: ================================
set EXE_NAME=Tests
set code_path=..\code\
set third_party_path=..\third_party\
set source_assets=..\assets\

:: ================================
:: COMPILER FLAGS
:: ================================
set compiler=-std:c++20 -nologo -Od -Gm- -GR- -EHa- -W4 -WX

:: IGNORE WARNINGS
set compiler=%compiler% -wd4201 -wd4100 -wd4189 -wd4505 -wd4456 -wd4127

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

del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL
del *.rdi > NUL 2> NUL

:: ================================
:: BUILD EXECUTABLE
:: ================================

cl %compiler% %defines% %debug% %code_path%all_tests.cpp ^
   /I%third_party_path%include %sdl_libs% %win32_libs% ^
   /link %link% /LIBPATH:%third_party_path%lib ^
   /OUT:%EXE_NAME%.exe

popd

.\build\%EXE_NAME%.exe