#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU_dpi
# File       Export.sh
# Usage      ./Export.sh {Major.Minor.Build_Type}

echo Executing  Export.sh $1  ...

# ===== Initialisation ======================================================

DST_FOLDER=~/Export/GPU_dpi/$1_Linux

# ===== Execution ===========================================================

mkdir $DST_FOLDER
if [ 0 != $? ] ; then
    echo ERROR  mkdir $DST_FOLDER  failed
    exit 1
fi

mkdir $DST_FOLDER/Binaries
mkdir $DST_FOLDER/Includes
mkdir $DST_FOLDER/Includes/GPU_dpi
mkdir $DST_FOLDER/Libraries
mkdir $DST_FOLDER/GPU_dpi
mkdir $DST_FOLDER/GPU_dpi_Extcap
mkdir $DST_FOLDER/GPU_dpi_Setup
mkdir $DST_FOLDER/Packages
mkdir $DST_FOLDER/Scripts
mkdir %DST_FOLDER/Tests

cp _DocUser/ReadMe.txt                $DST_FOLDER
cp Binaries/GPU_dpi_Test              $DST_FOLDER/Tests
cp Binaries/GPU_dpi.so                $DST_FOLDER/Binaries
cp Binaries/GPU_dpi_Extcap            $DST_FOLDER/Binaries
cp Binaries/GPU_dpi_Setup             $DST_FOLDER/Binaries
cp DoxyFile_*.txt                     $DST_FOLDER
cp Includes/GPU_dpi/*.h               $DST_FOLDER/Includes/GPU_dpi
cp GPU_dpi/_DocUser/ReadMe.txt        $DST_FOLDER/GPU_dpi
cp GPU_dpi_Extcap/_DocUser/ReadMe.txt $DST_FOLDER/GPU_dpi_Extcap
cp GPU_dpi_Setup/_DocUser/ReadMe.txt  $DST_FOLDER/GPU_dpi_Setup
cp Packages/*.deb                     $DST_FOLDER/Packages
cp Scripts/GPU_dpi_Extcap.sh          $DST_FOLDER/Binaries
cp Scripts/GPU_dpi_Setup.sh           $DST_FOLDER/Binaries

# ===== End =================================================================

echo OK
