@echo off

rem Author     KMS - Martin Dubois, ing.
rem Copyright  (C) 2019 KMS. All rights reserved.
rem Product    GPU-dpi
rem File       Import.cmd
rem Usage      .\Import.cmd

echo Executing  Import.cmd  ...

rem ===== Configuration =====================================================

set EXPORT_FOLDER=K:\Export

rem ===== Dependencies ======================================================

set KMS_BASE="%EXPORT_FOLDER%\KmsBase\3.0.13_Windows"

set OPEN_NET="%EXPORT_FOLDER%\OpenNet\1.0.11_Windows_Signed"

rem ===== Constants =========================================================

set DST_FOLDER="%CD%\Import"

rem ===== Verification ======================================================

if not exist %KMS_BASE% (
	echo FATAL ERROR  %KMS_BASE%  does not exist
	pause
	exit /B 1
)

if not exist %OPEN_NET% (
	echo FATAL ERROR  %OPEN_NET%  does not exist
	pause
	exit /B 2
)

rem  ===== Execution ========================================================

if not exist %DST_FOLDER% mkdir %DST_FOLDER%

pushd %KMS_BASE%
    call Import.cmd %DST_FOLDER%
popd

if ERRORLEVEL 1 (
	echo ERROR  call Import.cmd %DST_FOLDER%  failed - %ERRORLEVEL%
	pause
	exit /B 3
)

pushd %OPEN_NET%
    call Import.cmd %DST_FOLDER%
popd

if ERRORLEVEL 1 (
	echo ERROR  call Import.cmd %DST_FOLDER%  failed - %ERRORLEVEL%
	pause
	exit /B 4
)

copy Import\Binaries\Debug_64\OpenNet.dll x64\Debug
if ERRORLEVEL 1 (
	echo ERROR  copy Import\Binaries\Debug_64\OpenNet.dll x64\Debug  failed - %ERRORLEVEL%
	pause
	exit /B 5
)

copy Import\Binaries\Debug_64\OpenNet_Tool.exe x64\Debug
if ERRORLEVEL 1 (
	echo ERROR  copy Import\Binaries\Debug_64\OpenNet_Tool.exe x64\Debug  failed - %ERRORLEVEL%
	pause
	exit /B 6
)

copy Import\Binaries\Release_64\OpenNet.dll x64\Release
if ERRORLEVEL 1 (
	echo ERROR  copy Import\Binaries\Release_64\OpenNet.dll x64\Release  failed - %ERRORLEVEL%
	pause
	exit /B 7
)

copy Import\Binaries\Release_64\OpenNet_Tool.exe x64\Release
if ERRORLEVEL 1 (
	echo ERROR  copy Import\Binaries\Release_64\OpenNet_Tool.exe x64\Release  failed - %ERRORLEVEL%
	pause
	exit /B 8
)

rem  ===== End ==============================================================

echo  OK
