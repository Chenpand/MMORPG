@echo off
set XGAME_HOME=%cd%

echo %XGAME_HOME%

echo Start ChallengeServer
cd %XGAME_HOME%\ChallengeServer
start "ChallengeServer" /B "..\Bin\ChallengeServer.exe"

ping null -n 1 -w 5000 > null

echo Start WSceneServer
cd %XGAME_HOME%\WSceneServer1
start "WSceneServer1" /B "..\Bin\SceneServer.exe" "--DEBUG"

pause