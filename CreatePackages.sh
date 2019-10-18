#!/bin/sh

# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CreatePackages.sh
# Usage      ./CreatePackages.sh

# CODE REVIEW  2019-07-26  KMS - Martin Dubois, ing.

echo Excuting  CreatePackages.sh  ...

# ===== Version =============================================================

KERNEL=$(uname -r)

PACKAGE_VERSION=1.0-4

VERSION=1.0

# ===== Initialisation ======================================================

PACKAGE_NAME=kms-gpu-dpi_$PACKAGE_VERSION

# ===== Execution ===========================================================

rm Packages/*.deb

echo Creating  kms-gpu-dpi  ...

mkdir Packages
mkdir Packages/$PACKAGE_NAME
mkdir Packages/$PACKAGE_NAME/lib
mkdir Packages/$PACKAGE_NAME/lib/modules
mkdir Packages/$PACKAGE_NAME/lib/modules/$KERNEL
mkdir Packages/$PACKAGE_NAME/lib/modules/$KERNEL/kernel
mkdir Packages/$PACKAGE_NAME/lib/modules/$KERNEL/kernel/drivers
mkdir Packages/$PACKAGE_NAME/lib/modules/$KERNEL/kernel/drivers/pci
mkdir Packages/$PACKAGE_NAME/usr
mkdir Packages/$PACKAGE_NAME/usr/local
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/GPU_dpi
mkdir Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK

cp _DocUser/GPU-dpi.ReadMe.txt                               Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Binaries/libGPU-dpi.so                                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU-dpi_Extcap                                   Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU-dpi_Run                                      Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU-dpi_Setup                                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Binaries/GPU-dpi_TG                                       Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp DoxyFile_*.txt                                            Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp GPU_dpi/_DocUser/GPU-dpi.GPU-dpi.ReadMe.txt               Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp GPU_dpi_Extcap/_DocUser/GPU-dpi.GPU-dpi_Extcap.ReadMe.txt Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp GPU_dpi_Run/_DocUser/GPU-dpi.GPU-dpi_Run.ReadMe.txt       Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp GPU_dpi_Setup/_DocUser/GPU-dpi.GPU-dpi_Setup.ReadMe.txt   Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp GPU_dpi_TG/_DocUser/GPU-dpi.GPU-dpi_TG.ReadMe.txt         Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/Binaries/libOpenNet.so                             Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Import/Binaries/OpenNet_Tool                              Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Import/Includes/OpenNetK/ARP.h                            Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/ByteOrder.h                      Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Ethernet.h                       Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/IPv4.h                           Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/IPv6.h                           Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Kernel.h                         Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Kernel_CUDA.h                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/RegEx.h                          Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/TCP.h                            Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/Types.h                          Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/Includes/OpenNetK/UDP.h                            Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/OpenNetK
cp Import/KmsBase.KmsLib.ReadMe.txt                          Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/KmsBase.ReadMe.txt                                 Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/Modules/ONK_Pro1000.ko                             Packages/$PACKAGE_NAME/lib/modules/$KERNEL/kernel/drivers/pci
cp Import/OpenNet.ONK_Hardware.ReadMe.txt                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/OpenNet.ONK_Lib.ReadMe.txt                         Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/OpenNet.OpenNet.ReadMe.txt                         Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/OpenNet.OpenNet_Tool.ReadMe.txt                    Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Import/OpenNet.ReadMe.txt                                 Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION
cp Includes/GPU_dpi/*.h                                      Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/inc/GPU_dpi
cp Scripts/GPU-dpi_Extcap.sh                                 Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin
cp Scripts/GPU-dpi_Setup.sh                                  Packages/$PACKAGE_NAME/usr/local/GPU-dpi_$VERSION/bin

mkdir Packages/$PACKAGE_NAME/DEBIAN

cp Scripts/control     Packages/$PACKAGE_NAME/DEBIAN/control
cp Scripts/postinst.sh Packages/$PACKAGE_NAME/DEBIAN/postinst
cp Scripts/prerm.sh    Packages/$PACKAGE_NAME/DEBIAN/prerm

dpkg-deb --build Packages/$PACKAGE_NAME

rm -r Packages/$PACKAGE_NAME

# ===== End =================================================================

echo OK
