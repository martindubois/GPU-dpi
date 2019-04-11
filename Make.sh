#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Make.sh
# Usage      ./Make.sh

echo Excuting  Make.sh  ...

# ===== Execution ===========================================================

echo Building GPU_dpi ...
cd GPU_dpi
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi - make  failed
    exit 1
fi
cd ..

echo Building GPU_dpi_Extcap ...
cd GPU_dpi_Extcap
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Extcap - make  failed
    exit 2
fi
cd ..

echo Building GPU_dpi_Setup ...
cd GPU_dpi_Setup
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Setup - make  failed
    exit 3
fi
cd ..

echo Building GPU_dpi_Test ...
cd GPU_dpi_Test
make
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Test - make  failed
    exit 4
fi
cd ..

# ===== End =================================================================

echo OK
exit 0
