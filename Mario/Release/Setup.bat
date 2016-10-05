@echo off
copy *.* %windir%\System32
reg import Mario.reg
echo Done, please reboot the machine to complete the installation 