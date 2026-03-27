@echo off

pushd code

findstr /s /i /n "TODO" *.* | findstr /v "todo.bat"

popd