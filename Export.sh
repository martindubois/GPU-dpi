#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       Export.sh
# Usage      ./Export.sh {Major.Minor.Build_Type}

# TODO  Export
#       Normal (Cleanup) - Use KmsCopy

echo Executing  Export.sh $1  ...

# ===== Initialisation ======================================================

DST_FOLDER=~/Export/GPU-dpi/$1_Linux_$(uname -r)

# ===== Execution ===========================================================

mkdir $DST_FOLDER
if [ 0 != $? ] ; then
    echo ERROR  mkdir $DST_FOLDER  failed
    exit 1
fi

mkdir $DST_FOLDER/Libraries
mkdir $DST_FOLDER/Packages
mkdir $DST_FOLDER/Scripts
mkdir $DST_FOLDER/Tests

cp Binaries/CodeGen_Test              $DST_FOLDER/Tests
cp Binaries/GPU-dpi_Test              $DST_FOLDER/Tests
cp DoxyFile_*.txt                     $DST_FOLDER
cp Libraries/CodeGen.a                $DST_FOLDER/Libraries
cp Packages/*.deb                     $DST_FOLDER/Packages

# ----- File also in the package --------------------------------------------

mkdir $DST_FOLDER/Binaries
mkdir $DST_FOLDER/Includes
mkdir $DST_FOLDER/Includes/GPU_dpi

cp _DocUser/GPU-dpi.ReadMe.txt                               $DST_FOLDER
cp Binaries/libGPU-dpi.so                                    $DST_FOLDER/Binaries
cp Binaries/GPU-dpi_Extcap                                   $DST_FOLDER/Binaries
cp Binaries/GPU-dpi_Setup                                    $DST_FOLDER/Binaries
cp Binaries/GPU-dpi_Run                                      $DST_FOLDER/Binaries
cp Binaries/GPU-dpi_TG                                       $DST_FOLDER/Binaries
cp GPU_dpi/_DocUser/GPU-dpi.GPU-dpi.ReadMe.txt               $DST_FOLDER
cp GPU_dpi_Extcap/_DocUser/GPU-dpi.GPU-dpi_Extcap.ReadMe.txt $DST_FOLDER
cp GPU_dpi_Run/_DocUser/GPU-dpi.GPU-dpi_Run.ReadMe.txt       $DST_FOLDER
cp GPU_dpi_Setup/_DocUser/GPU-dpi.GPU-dpi_Setup.ReadMe.txt   $DST_FOLDER
cp GPU_dpi_TG/_DocUser/GPU-dpi.GPU-dpi_TG.ReadMe.txt         $DST_FOLDER
cp Includes/GPU_dpi/*.h                                      $DST_FOLDER/Includes/GPU_dpi
cp Scripts/GPU-dpi_Extcap.sh                                 $DST_FOLDER/Binaries
cp Scripts/GPU-dpi_Setup.sh                                  $DST_FOLDER/Binaries

# ===== End =================================================================

echo OK
