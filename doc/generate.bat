del /Q ..\build\html\*.*
del /Q ..\build\latex\*.*
doxygen Doxyfile.doxyfile
copy /Y latex\Makefile ..\build\latex
cd ..\build\latex
nmake
