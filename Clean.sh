#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Clean.sh
# Usage      ./Clean.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Excuting  Clean.sh  ...

# ===== Execution ===========================================================

cd CodeGen
./Clean.sh
cd ..

cd CodeGen_Test
./Clean.sh
cd ..

cd GPU_dpi
./Clean.sh
cd ..

cd GPU_dpi_Extcap
./Clean.sh
cd ..

cd GPU_dpi_Setup
./Clean.sh
cd ..

cd GPU_dpi_Test
./Clean.sh
cd ..

cd GPU-dpi_Run
./Clean.sh
cd ..

cd GPU-dpi_TG
./Clean.sh
cd ..

# ===== End =================================================================

echo OK
exit 0
