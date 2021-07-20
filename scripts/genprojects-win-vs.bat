@echo off
pushd ..\
call vendor\premake\bin\premake5.exe --file=PremakeConfig\main.lua vs2019
popd
PAUSE
