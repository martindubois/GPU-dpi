@echo off

rem Author     KMS - Martin Dubois, ing.
rem Copyright  (C) 2019 KMS. All rights reserved.
rem Product    GPU-dpi
rem File       Build.cmd

echo Executing  Build.cmd  ...

rem ===== Initialization / Initialisation ===================================

set CERT_SHA=2D45F19469373612C5E626A8FCD4450759792859

set INNO_COMPIL32="C:\Program Files (x86)\Inno Setup 5\Compil32.exe"
set KMS_VERSION="C:\Software\KmsTools\KmsVersion.exe"
set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\MSBuild\15.0\Bin\MSBuild.exe"
set OPTIONS="GPU-dpi.sln" /target:rebuild

set SIGNTOOL_EXE="C:\Program Files (x86)\Windows Kits\10\Tools\bin\i386\signtool.exe"

rem ===== Verification / Verification =======================================

if not exist %INNO_COMPIL32% (
	echo FATAL ERROR  %INNO_COMPIL32%  does not exist
	echo Install Inno 5.6.1
	pause
	exit /B 1
)

if not exist %KMS_VERSION% (
    echo FATAL ERROR  %KMS_VERSION%  does not exist
	echo Install KmsTools version 2.4.2 or higher
	pause
	exit /B 2
)

if not exist %MSBUILD% (
	echo ERREUR FATAL  %MSBUILD%  does not exist
    echo Install Visual Studio 2017 Professional
	pause
	exit /B 3
)

if not exist %SIGNTOOL_EXE% (
	echo FATAL ERROR  %SIGNTOOL_EXE%  does not exist
	echo Install the WDK
	pause
	exit /B 4
)

rem ===== Execution / Execution =============================================

%MSBUILD% %OPTIONS% /property:Configuration=Debug /property:Platform=x64
if ERRORLEVEL 1 (
	echo ERROR  %MSBUILD% %OPTIONS% /property:Configuration=Debug /property:Platform=x64  failed - %ERRORLEVEL%
	pause
	exit /B 5
)

%MSBUILD% %OPTIONS% /property:Configuration=Release /property:Platform=x64
if ERRORLEVEL 1 (
	echo ERROR  %MSBUILD% %OPTIONS% /property:Configuration=Release /property:Platform=x64  failed - %ERRORLEVEL%
	pause
	exit /B 6
)

call Test.cmd
if ERRORLEVEL 1 (
    echo ERROR  Test.cmd  failed - %ERRORLEVEL%
	pause
	exit /B 7
)

%KMS_VERSION% Common\Version.h Export.cmd.txt GPU_dpi.iss
if ERRORLEVEL 1 (
	echo ERROR  %KMS_VERSION% Common\Version.h Export.cmd.txt GPU_dpi.iss  failed - %ERRORLEVEL%
	pause
	exit /B 8
)

%INNO_COMPIL32% /cc GPU_dpi.iss
if ERRORLEVEL 1 (
	echo ERROR  %INNO_COMPIL32% /cc GPU_dpi.iss  failed - %ERRORLEVEL%
	pause
	exit /B 9
)

%SIGNTOOL_EXE% sign /fd sha256 /sha1 %CERT_SHA% /td sha256 /tr http://timestamp.digicert.com Installer/GPU-dpi_*.exe
if ERRORLEVEL 1 (
	echo ERROR  %SIGNTOOL_EXE% sign /fd sha256 /sha1 %CERT_SHA% /td sha256 /tr http://timestamp.digicert.com Installer/GPU-dpi_*.exe  failed - %ERRORLEVEL%
	pause
	exit /B 10
)

%KMS_VERSION% -S Common\Version.h Export.cmd
if ERRORLEVEL 1 (
    echo ERROR  %KMS_VERSION% -S Common\Version.h Export.cmd  failed - %ERRORLEVEL%
	pause
	exit /B 11
)

rem ===== End ===============================================================

echo OK
