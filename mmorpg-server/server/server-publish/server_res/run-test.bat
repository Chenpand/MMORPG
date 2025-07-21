@echo off
set XGAME_HOME=%cd%

echo %XGAME_HOME%

echo Start AdminServer
cd %XGAME_HOME%\AdminServer
start "AdminServer" /B "..\Bin\AdminServer.exe"

ping null -n 1 -w 5000 > null

echo Start RecordServer
cd %XGAME_HOME%\RecordServer
start "RecordServer" /B "..\Bin\RecordServer.exe"

ping null -n 1 -w 5000 > null

echo Start WorldServer
cd %XGAME_HOME%\WorldServer
start "WorldServer" /B "..\Bin\WorldServer.exe"

ping null -n 1 -w 5000 > null

echo Start GateServer
cd %XGAME_HOME%\GateServer1
start "GateServer" /B "..\Bin\GateServer.exe"

pause