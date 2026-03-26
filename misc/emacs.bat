@echo off

pushd .\code\

etags *.cpp *.h

popd

start runemacs.exe -q -l misc\config.el