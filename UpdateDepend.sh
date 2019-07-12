#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       UpdateDepend.sh
# Usage      ./UpdateDepend.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Excuting  UpdateDepend.sh  ...

# ===== Initialisation ======================================================

RESULT=0

# ===== Execution ===========================================================

cd CodeGen
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  CodeGen - make depend  failed
    RESULT=1
fi
cd ..

cd CodeGen_Test
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  CodeGen_Test - make depend  failed
    RESULT=2
fi
cd ..

cd GPU_dpi
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi - make depend  failed
    RESULT=3
fi
cd ..

cd GPU_dpi_Extcap
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Extcap - make depend  failed
    RESULT=4
fi
cd ..

cd GPU_dpi_Setup
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Setup - make depend  failed
    RESULT=5
fi
cd ..

cd GPU_dpi_Test
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Test - make depend  failed
    RESULT=6
fi
cd ..

cd GPU-dpi_Run
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU-dpi_Run - make depend  failed
    RESULT=7
fi
cd ..

cd GPU-dpi_TG
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU-dpi_TG - make depend  failed
    RESULT=8
fi
cd ..

# ===== End =================================================================

if [ 0 = $RESULT ] ; then
    echo OK
fi

exit $RESULT
