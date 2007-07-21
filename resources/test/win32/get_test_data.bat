@REM This script have to be run in the directory of the SQLite database file
@echo off
copy /Y config.db3 config-old.db3
echo .exit | sqlite3 -init ..\..\..\..\..\..\resources\test\test_transport_network.sql config.db3
