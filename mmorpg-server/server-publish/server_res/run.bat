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

echo Start SceneServer1
cd %XGAME_HOME%\SceneServer1
start "SceneServer" /B "..\Bin\SceneServer.exe" "--DEBUG"

rem echo Start SceneServer2
rem cd %XGAME_HOME%\SceneServer2
rem start "SceneServer" /B "..\Bin\SceneServer.exe" "--DEBUG"

ping null -n 1 -w 5000 > null

echo Start GateServer
cd %XGAME_HOME%\GateServer1
start "GateServer" /B "..\Bin\GateServer.exe"

rem echo Start GateServer
rem cd %XGAME_HOME%\GateServer2
rem start "GateServer" /B "..\Bin\GateServer.exe"

pause