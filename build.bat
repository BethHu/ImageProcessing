@echo off
cd /d "F:\rsdiplib\code\myAlg"
"C:\Program Files\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"
msbuild myAlg.sln /p:Configuration=Debug /p:Platform=Win32
pause
