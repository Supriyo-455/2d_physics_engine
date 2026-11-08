@echo off
cd /d %~dp0..
raddbg --user:.\misc\game.rdbg --project:.\project.rdbg .\build\Game.exe