#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Test.sh
# Usage      ./Test.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Executing  Test.sh  ...

# ===== Initialisation ======================================================

BINARIES=./Binaries

CUDA=/usr/local/cuda-10.0

# ===== Verification ========================================================

if [ ! -d $CUDA ]
then
    echo FATAL ERROR  $CUDA  does not exist
    echo Install CUDA Toolkit 10.0
    exit 1
fi

# ===== Execution ===========================================================

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BINARIES:./Import/Binaries:$CUDA/lib64:$CUDA/lib64/stubs
export LD_LIBRARY_PATH

$BINARIES/CodeGen_Test
if [ 0 != $? ] ; then
    echo ERROR  $BINARIES/CodeGen_Test  failed
    exit 2
fi

$BINARIES/GPU-dpi_Test
if [ 0 != $? ] ; then
    echo ERROR  $BINARIES/GPU-dpi_Test  failed
    exit 3
fi

# ===== End =================================================================

echo OK
