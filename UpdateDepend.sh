#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       UpdateDepend.sh
# Usage      ./UpdateDepend.sh

echo Excuting  UpdateDepend.sh  ...

# ===== Initialisation ======================================================

RESULT=0

# ===== Execution ===========================================================

cd GPU_dpi
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi - make depend  failed - $?
    RESULT=1
fi
cd ..

cd GPU_dpi_Test
make depend > /dev/null 2> /dev/null
if [ 0 != $? ] ; then
    echo ERROR  GPU_dpi_Test - make depend  failed - $?
    RESULT=2
fi
cd ..

# ===== End =================================================================

if [ 0 = $RESULT ] ; then
    echo OK
fi

exit $RESULT
