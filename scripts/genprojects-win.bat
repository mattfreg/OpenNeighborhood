@echo off
SET rootDir=%~dp0..
call %rootDir%\tools\premake\bin\premake5.exe --file=%rootDir%\premake5.lua gmake2
