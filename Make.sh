#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Make.sh
# Usage      ./Make.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Excuting  Make.sh  ...

# ===== Execution ===========================================================

echo Building CodeGen ...
cd CodeGen
make
if [ 0 != $? ] ; then
    echo ERROR  CodeGen - make  failed
    exit 1
fi
cd ..

echo Building CodeGen_Test ...
cd CodeGen_Test
make
if [ 0 != $? ] ; then
    echo ERROR  CodeGen_Test - make  failed
    exit 2
fi
cd ..

echo Building GPU_dpi ...
cd GPU_dpi
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi - make  failed
    exit 3
fi
cd ..

echo Building GPU_dpi_Extcap ...
cd GPU_dpi_Extcap
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Extcap - make  failed
    exit 4
fi
cd ..

echo Building GPU_dpi_Setup ...
cd GPU_dpi_Setup
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Setup - make  failed
    exit 5
fi
cd ..

echo Building GPU_dpi_Test ...
cd GPU_dpi_Test
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Test - make  failed
    exit 6
fi
cd ..

echo Building GPU-dpi_Run ...
cd GPU-dpi_Run
make
if [ 0 != $? ] ; then
    echo ERROR  GPU-dpi_Run - make  failed
    exit 7
fi
cd ..

echo Building GPU-dpi_TG ...
cd GPU-dpi_TG
make
if [ 0 != $? ] ; then
    echo ERROR  GPU-dpi_TG - make  failed
    exit 8
fi
cd ..

# ===== End =================================================================

echo OK
exit 0
