@echo off
if exist icons.h del icons.h
for %%f in (*.png) do call :one "%%f"
rem call :one execute_12.png
del temp_header.tmp
goto :eof
:one
magick %1 -negate "%~n1.mono"
FileToArray progmem "%~n1.mono" temp_header.tmp %~n1
del "%~n1.mono"
type temp_header.tmp >> icons.h
echo. >> icons.h
goto :eof