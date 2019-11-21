rem First run eg: "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
rem or run this from the command window after using the windows menu:
rem     "Start" -> "Visual Studio 2019" -> "x64 Native Tools Command Prompt for VS2019"
cl /c /nologo /EHsc realpath.c
cl /c /nologo /EHsc resolvr.cpp
link /out:resolvr.exe *.obj