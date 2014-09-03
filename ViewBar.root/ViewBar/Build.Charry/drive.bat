@echo off
@if not "%os%"=="Windows_NT" GOTO 95OS

:NTOS
rem @call \\szav01\vplogon\vplogon.bat
@net use v: \\fileserver\ex_folder$ /persistent:no
@net use x: \\fileserver\agloco$ /persistent:no
GOTO EOF


:95OS
rem @call \\szav01\vplogon\vplogon.bat
@net use v: \\fileserver\ex_folder$
@net use x: \\fileserver\agloco$  



:EOF
@EXIT