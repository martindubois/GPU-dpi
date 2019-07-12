#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CodeGen/Clean.sh
# Usage      ./Clean.sh

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

echo Excuting  CodeGen/Clean.sh  ...

# ===== Execution ===========================================================

rm *.o 2> /dev/null

rm ../Libraries/CodeGen.a 2> /dev/null

# ===== End =================================================================

echo OK
