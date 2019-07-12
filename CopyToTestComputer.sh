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

scp Binaries/libGPU-dpi.so $TEST_COMPUTER:$DST_FOLDER/Binaries
if [ 0 != $? ] ; then
    echo ERROR  scp Binaries/libGPU-dpi.so $TEST_COMPUTER:$DST_FOLDER/Binaries  failed
    RESULT=1
fi

scp Binaries/GPU-dpi_Test $TEST_COMPUTER:$DST_FOLDER/Binaries
if [ 0 != $? ] ; then
    echo ERROR  scp Binaries/GPU-dpi_Test $TEST_COMPUTER:$DST_FOLDER/Binaries  failed
    RESULT=2
fi

scp GPU-dpi_Run/Tests/Test02.txt $TEST_COMPUTER:$DST_FOLDER/GPU-dpi_Run/Tests
if [ 0 != $? ] ; then
    echo ERROR  scp GPU-dpi_Run/Tests/Test00.txt $TEST_COMPUTER:$DST_FOLDER/GPU-dpi_Run/Tests  failed
    RESULT=3
fi

scp GPU-dpi_TG/Tests/*.txt $TEST_COMPUTER:$DST_FOLDER/GPU-dpi_TG/Tests
if [ 0 != $? ] ; then
    echo ERROR  scp GPU-dpi_TG/Tests/*.txt $TEST_COMPUTER:$DST_FOLDER/GPU-dpi_TG/Tests  failed
    RESULT=4
fi

scp Packages/kms-gpu-dpi_1.0-0.deb $TEST_COMPUTER:$DST_FOLDER/Packages
if [ 0 != $? ] ; then
    echo ERROR  scp Packages/kms-gpu-dpi_1.0-0.deb $TEST_COMPUTER:$DST_FOLDER/Packages  failed
    RESULT=5
fi

# ===== End =================================================================

if [ 0 = $RESULT ] ; then
    echo OK
fi

exit $RESULT
