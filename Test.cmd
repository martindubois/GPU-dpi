@echo off

rem Author     KMS - Martin Dubois, ing.
rem Copyright  (C) 2019 KMS. All rights reserved.
rem Product    GPU-dpi
rem File       Test.cmd

echo Executing  Test.cmd  ...

rem ===== Initialization ====================================================

set DEBUG=x64\Debug
set RELEASE=x64\Release

rem ===== Verification ======================================================

if not exist "%DEBUG%" (
    echo FATAL ERROR  "%DEBUG%"  does not exist
    echo Compile the product
    pause
    exit /B 1
)

if not exist "%RELEASE%" (
    echo FATAL ERROR  "%RELEASE%"  does not exist
    echo Compile the product
    pause
    exit /B 2
)

rem ===== Execution =========================================================

%DEBUG%\CodeGen_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %DEBUG%\CodeGen_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 3
)

%DEBUG%\GPU_dpi_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %DEBUG%\GPU_dpi_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 4
)

%RELEASE%\CodeGen_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %RELEASE%\CodeGen_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 5
)

%RELEASE%\GPU_dpi_Test.exe
if ERRORLEVEL 1 (
    echo ERROR  %RELEASE%\GPU_dpi_Test.exe  failed - %ERRORLEVEL%
    pause
    exit /B 5
)

rem ===== End ===============================================================

echo OK
