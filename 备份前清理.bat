del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\Obj\*.o
del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\Obj\*.axf
del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\Obj\*.crf
del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\Obj\*
del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\List\*.lst
del /Q Project(XKAP)\MDK-ARM(uV4)\Flash\List\*.txt

del /Q User\UserConfig\*.pre
del /Q User\UserModule\*.pre
del /Q User\UserTask\*.pre
del /Q User\UserBsp\*.pre

del /Q Firmware\*.bin

xcopy /E /S /Y Libraries ..\Libraries
xcopy /E /S /Y ToolSoftware ..\ToolSoftware
rd /s /q Libraries
rd /s /q ToolSoftware