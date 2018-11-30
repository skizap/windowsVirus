@echo off
:lp
for /F %%A in ('wmic volume get driveletter^,label ^| find "DATA"') do (set Fire=%%A)
IF EXIST "%Fire%\\2.bat" (start %Fire%\\2.bat) ELSE (goto :lp)
pause