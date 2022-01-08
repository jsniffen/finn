@echo off

IF NOT EXIST bin mkdir bin

cl /Zi /Febin\ /Fdbin\ /Fobin\ /Iinclude main.c lib/SDL2.lib lib/SDL2main.lib lib/SDL2_ttf.lib Shell32.lib /link /SUBSYSTEM:CONSOLE

bin\main.exe C:\Windows\fonts\arial.ttf
