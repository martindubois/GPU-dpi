#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU_dpi/Clean.sh
# Usage      ./Clean.sh

echo Excuting  GPU_dpi/Clean.sh  ...

# ===== Execution ===========================================================

rm *.o 2> /dev/null

rm ../Binaries/GPU_dpi.so 2> /dev/null

# ===== End =================================================================

echo OK
