#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CreatePackages.sh
# Usage      ./CreatePackages.sh

echo Excuting  CreatePackages.sh  ...

# ===== Version =============================================================

KERNEL=$(uname -r)

PACKAGE_VERSION=0.0-0

VERSION=0.0

# ===== Initialisation ======================================================

PACKAGE_NAME=kms-gpu-dpi_$PACKAGE_VERSION

# ===== Execution ===========================================================

echo Creating  kms-gpu-dpi  ...

mkdir Packages
mkdir Packages/$PACKAGE_NAME
mkdir Packages/$PACKAGE_NAME/usr
mkdir Packages/$PACKAGE_NAME/usr/local
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/GPU_dpi
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/GPU_dpi

cp _DocUser/ReadMe.txt                    Packages/$PACKAGE_NAME
cp Binaries/GPU_dpi.so                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU_dpi_Extcap                Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU_dpi_Setup                 Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp DoxyFile_*.txt                         Packages/$PACKAGE_NAME_SDK/usr/local/GPU-dpi_$VERSION
cp GPU_dpi/_DocUser/ReadMe.txt            Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/GPU_dpi
cp Import/Binaries/OpenNet.so             Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Import/Binaries/OpenNet_Tool           Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Import/Includes/OpenNetK/ARP.h         Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Ethernet.h    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/IPv4.h        Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Kernel.h      Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Kernel_CUDA.h Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Types.h       Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Includes/GPU_dpi/*.h                   Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/GPU_dpi
cp Scripts/GPU_dpi_Extcap.sh              Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Scripts/GPU_dpi_Setup.sh               Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin

mkdir Packages/$PACKAGE_NAME/DEBIAN

cp Scripts/control     Packages/$PACKAGE_NAME/DEBIAN/control
cp Scripts/postinst.sh Packages/$PACKAGE_NAME/DEBIAN/postinst

dpkg-deb --build Packages/$PACKAGE_NAME

rm -r Packages/$PACKAGE_NAME

# ===== End =================================================================

echo OK
