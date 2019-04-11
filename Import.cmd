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

set KMS_BASE="%EXPORT_FOLDER%\KmsBase\3.0.9_KI_Windows"
set OPEN_NET="%EXPORT_FOLDER%\OpenNet\0.0.15_KI_Windows"

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

rem  ===== End ==============================================================

echo  OK
