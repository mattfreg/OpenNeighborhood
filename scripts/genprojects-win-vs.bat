@echo off
pushd ..\
call tools\premake\bin\premake5.exe vs2019
popd
PAUSE
