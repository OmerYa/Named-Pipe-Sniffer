@echo off
copy EasyHook32.dll %windir%\System32
copy Mario.dll %windir%\System32
reg import Mario.reg
echo Done, please reboot the machine to complete the installation 
pause