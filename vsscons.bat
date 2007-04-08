
REM for vc8 call "C:\Program Files\Microsoft Platform SDK for Windows XP SP2\SetEnv.Bat"
REM for vc8 call vcvars32.bat

echo %PATH%

SET PATH=%PATH%;C:\Python24
SET PATH=%PATH%;C:\Python25
SET PATH=%PATH%;C:\Program Files\Python24
SET PATH=%PATH%;C:\Program Files\doxygen\bin
SET PATH=%PATH%;C:\Program Files\Graphviz\Graphviz\bin
SET PATH=%PATH%;C:\Program Files\texmf\miktex\bin
SET PATH=%PATH%;%LIBS_REPO_HOME%\lib

cd
cd ..
cd ..
cd ..
cd
ECHO --------------------------
ECHO vsscons %1 %2 %3 %4 %5 %6 %7 %8 %9
ECHO --------------------------



IF %1 == clean GOTO CLEAN

IF %1 == build GOTO BUILD

IF %1 == rebuild GOTO REBUILD

GOTO END



:CLEAN
rem CALL scons -c %2 %3 %4 %5 %6 %7 %8 %9
CALL ant %2 %3 %4 %5 %6 %7 %8 %9 clean
GOTO END

:BUILD
echo scons %2 %3 %4 %5 %6 %7 %8 %9
rem CALL scons -j %NUMBER_OF_PROCESSORS% %2 %3 %4 %5 %6 %7 %8 %9
CALL ant %2 %3 %4 %5 %6 %7 %8 %9 build
GOTO END


:REBUILD
CALL ant %2 %3 %4 %5 %6 %7 %8 %9 clean
rem CALL scons -c %2 %3 %4 %5 %6 %7 %8 %9
GOTO BUILD



:END

