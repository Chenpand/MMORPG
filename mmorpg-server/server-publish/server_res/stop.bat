@echo off

echo Stop GateServer
taskkill /f /t /im GateServer.exe

echo Stop SceneServer1
taskkill /f /t /im SceneServer.exe

echo Stop WorldServer
taskkill /f /t /im WorldServer.exe

echo Stop RecordServer
taskkill /f /t /im RecordServer.exe

echo Stop AdminServer
taskkill /f /t /im AdminServer.exe

pause