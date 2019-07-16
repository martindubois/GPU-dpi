#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Build.sh
# Usage      ./Build.sh {Major.Minor.Build_Type}

# CODE REVIEW  2019-07-16  KMS - Martin Dubois, ing.

echo Excuting  Build.sh $1  ...

# ===== Execution ===========================================================

./Clean.sh

./Make.sh
if [ 0 != $? ] ; then
    echo ERROR  ./Make.sh  failed
    exit 1
fi

./Test.sh
if [ 0 != $? ] ; then
    echo ERROR  ./Test.sh  failed
    exit 2
fi

./CreatePackages.sh
if [ 0 != $? ] ; then
    echo ERROR  ./Test.sh  failed
    exit 3
fi

./Export.sh $1
if [ 0 != $? ] ; then
    echo ERROR  ./Export.sh $1  failed
    exit 4
fi

# ===== End =================================================================

echo OK
exit 0
