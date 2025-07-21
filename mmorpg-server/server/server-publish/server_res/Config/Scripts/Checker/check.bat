@echo off

set TaskPath=..\Task
set NpcPath=..\Npc

for %%a in ("%TaskPath%\*.lua","%NpcPath%\*.lua") do (
	.\luac52.exe "%%a"
)

pause