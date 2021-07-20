@echo off
pushd ..\
call vendor\premake\bin\premake5.exe --file=PremakeConfig\main.lua gmake2
popd
PAUSE
