#!/bin/sh

# Auhtor     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Scripts/prerm.sh

# CODE REVIEW  2019-07-12  KMS - Martin Dubois, ing.

echo  Executing  prerm.sh  ...

# ===== Configuration =======================================================

GPU_DPI_INSTALL=/usr/local/GPU-dpi_1.0

# ===== Execution ===========================================================

$GPU_DPI_INSTALL/bin/GPU-dpi_Setup.sh uninstall
if [ 0 != $? ] ; then
    echo ERROR  $GPU_DPI_INSTALL/bin/GPU-dpi_Setup.sh uninstall  failed
    exit 1
fi

# ===== End =================================================================

echo OK
