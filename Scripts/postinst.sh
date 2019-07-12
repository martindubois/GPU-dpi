#!/bin/sh

# Auhtor     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Scripts/postinst.sh

# CODE REVIEW  2019-07-12  KMS - Martin Dubois, ing.

echo  Executing  postinst.sh  ...

# ===== Configuration =======================================================

GPU_DPI_INSTALL=/usr/local/GPU-dpi_1.0

# ===== Execution ===========================================================

depmod
if [ 0 != $? ] ; then
    echo ERROR  depmod  failed
    exit 1
fi

$GPU_DPI_INSTALL/bin/GPU-dpi_Setup.sh install
if [ 0 != $? ] ; then
    echo ERROR  $GPU_DPI_INSTALL/bin/GPU-dpi_Setup.sh install  failed
    exit 2
fi

# ===== End =================================================================

echo OK
