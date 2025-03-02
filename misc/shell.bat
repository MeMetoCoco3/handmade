@echo off
where cl >nul 2>nul
if %errorlevel% neq 0 (
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)
