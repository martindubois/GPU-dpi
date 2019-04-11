#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Clean.sh
# Usage      ./Clean.sh

echo Excuting  Clean.sh  ...

# ===== Execution ===========================================================

cd GPU_dpi
./Clean.sh
cd ..

cd GPU_dpi_Extcap
./Clean.sh
cd ..

cd GP_dpi_Setup
./Clean.h
cd ..

cd GPU_dpi_Test
./Clean.sh
cd ..

# ===== End =================================================================

echo OK
exit 0
