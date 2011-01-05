@echo off
IF EXIST basename.bat ECHO Base en cours :
IF EXIST basename.bat TYPE basename.bat
DIR /B config-*.db3
SET /P BASE=[Select the database to activate]
IF NOT EXIST config-%BASE%.db3 GOTO err
IF EXIST basename.bat CALL basename.bat
ECHO COPY /Y config.db3 config-%BASE%.db3 > basename.bat
COPY /Y config-%BASE%.db3 config.db3
GOTO fin
:err
ECHO Erreur base inexistante
PAUSE
:fin