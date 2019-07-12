
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU_dpi_Setup/makefile

# CODE REVIEW  2016-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT) -I $(CUDA_FOLDER)/include

LIBRARIES = $(KMS_LIB_A)

OUTPUT = ../Binaries/GPU-dpi_Setup

SOURCES =	GPU_dpi_Setup.cpp \
            OSDep_Linux.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -L ../Binaries $(BINARY_IMPORT) -L $(CUDA_FOLDER)/lib64 -L $(CUDA_FOLDER)/lib64/stubs -o $@ $^ -l GPU-dpi -l OpenNet -l cudart -l cuda -l nvrtc

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

GPU_dpi_Setup.o: Component.h ../Import/Includes/KmsBase.h
GPU_dpi_Setup.o: ../Import/Includes/SafeAPI.h
GPU_dpi_Setup.o: ../Import/Includes/WindowsToLinux.h
GPU_dpi_Setup.o: ../Import/Includes/KmsTool.h ../Includes/GPU_dpi/SetupTool.h
GPU_dpi_Setup.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
GPU_dpi_Setup.o: ../Common/Version.h OSDep.h
OSDep_Linux.o: Component.h ../Import/Includes/KmsBase.h
OSDep_Linux.o: ../Import/Includes/SafeAPI.h
OSDep_Linux.o: ../Import/Includes/WindowsToLinux.h
