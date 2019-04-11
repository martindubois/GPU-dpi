#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Import.sh
# Usage      ./Import.sh

echo Excuting  Import.sh  ...

# ===== Dependencies =========================================================

KMS_BASE=~/Export/KmsBase/3.0.8_KI_Linux
OPEN_NET=~/Export/OpenNet/0.0.14.KI_Linux

# ===== Constants ============================================================

DST_FOLDER=$PWD/Import

# ===== Verification =========================================================

if [ ! -d $KMS_BASE ]
then
    echo FATAL ERROR  $KMS_BASE  does not exist
    exit 1
fi

if [ ! -d $OPEN_NET ]
then
    echo FATAL ERROR  $OPEN_NET  does not exist
    exit 2
fi

# ===== Execution ============================================================

if [ ! -d $DST_FOLDER ]
then
    mkdir $DST_FOLDER
fi

cd $KMS_BASE

./Import.sh $DST_FOLDER

if [ 0 -ne $? ]
then
    echo ERROR  ./Import.sh $DST_FOLDER  failed - $?
    exit 3
fi

cd $OPEN_NET

./Import.sh $DST_FOLDER

if [ 0 -ne $? ]
then
    echo ERROR  ./Import.sh $DST_FOLDER  failed - $?
    exit 4
fi

# ===== End ==================================================================

echo OK
