@echo off
pushd ..\
call tools\premake\bin\premake5.exe gmake2
popd
PAUSE
