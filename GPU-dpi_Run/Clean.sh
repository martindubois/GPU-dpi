#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU-dpi_Run/Clean.sh
# Usage      ./Clean.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Excuting  GPU-dpi_Run/Clean.sh  ...

# ===== Execution ===========================================================

rm *.o 2> /dev/null

rm ../Binaries/GPU-dpi_Run.so 2> /dev/null

# ===== End =================================================================

echo OK
