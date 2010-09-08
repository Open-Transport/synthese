@REM This script have to be run in the directory of the SQLite database file
@echo off
copy /Y config.db3 config-old.db3
echo BEGIN TRANSACTION; > transaction.sql
type c:\windows\temp\synthese\synthese3.sql >> transaction.sql
echo END TRANSACTION; >> transaction.sql
echo .exit | sqlite3 -init transaction.sql config.db3
del transaction.sql
pause
