#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Scripts/GPU-dpi_Setup.sh
# Usage      See GPU-dpi_Setup

# CODE REVIEW  2017-07-12  KMS - Martin Dubois, ing.

echo  Executing  GPU-dpi_Setup.sh $*  ...

# ===== Configuration =======================================================

GPU_DPI_INSTALL=/usr/local/GPU-dpi_1.0

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GPU_DPI_INSTALL/bin:/usr/local/cuda-10.0/lib64

export LD_LIBRARY_PATH

# ===== Execution ===========================================================

$GPU_DPI_INSTALL/bin/GPU-dpi_Setup $*
if [ 0 != $? ] ; then
    echo ERROR  $GPU_DPI_INSTALL/bin/GPU-dpi_Setup $*  failed
    exit 1
fi

# ===== End =================================================================

echo OK
