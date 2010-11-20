@REM This script have to be run in the directory of the SQLite database file
@echo off
echo Installation of Common/Admin interface data...
echo .exit | sqlite3 -init ..\..\..\..\..\..\..\data\interface\common\s3-admin\deb\opt\rcs\common-datas3-interface-admin\install.sql config.db3
pause
