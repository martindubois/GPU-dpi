#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2018-2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CopyToTestComputer.sh
# Usage      ./CopyToTestComputer.sh

echo Excuting  CopyToTestComputer.sh  ...

# ===== Configuration =======================================================

TEST_COMPUTER=192.168.0.197

# ===== Initialisation ======================================================

DST_FOLDER=~/GPU-dpi

RESULT=0

# ===== Execution ===========================================================

scp Binaries/GPU_dpi.so $TEST_COMPUTER:$DST_FOLDER/Binaries
if [ 0 != $? ] ; then
    echo ERROR  scp Binaries/GPU_dpi.so $TEST_COMPUTER:$DST_FOLDER/Binaries  failed - $?
    RESULT=1
fi

# ===== End =================================================================

if [ 0 = $RESULT ] ; then
    echo OK
fi

exit $RESULT
