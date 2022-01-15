@echo off

REM this is a comment
IF NOT EXIST bin mkdir bin

cl /Zi /Febin\ /Fdbin\ /Fobin\ /Iinclude main.c lib/SDL2.lib lib/SDL2main.lib lib/SDL2_ttf.lib Shell32.lib /link /SUBSYSTEM:CONSOLE
