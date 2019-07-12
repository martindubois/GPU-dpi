
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU_dpi_Extcap/makefile

# CODE REVIEW  2019-11-26  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT) -I $(CUDA_FOLDER)/include

LIBRARIES = $(KMS_LIB_A)

OUTPUT = ../Binaries/GPU-dpi_Extcap

SOURCES =	GPU_dpi_Extcap.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -pthread -L ../Binaries $(BINARY_IMPORT) -L $(CUDA_FOLDER)/lib64 -L $(CUDA_FOLDER)/lib64/stubs -o $@ $^ -l GPU-dpi -l OpenNet -l cudart -l cuda -l nvrtc

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

GPU_dpi_Extcap.o: Component.h ../Import/Includes/KmsBase.h
GPU_dpi_Extcap.o: ../Import/Includes/SafeAPI.h
GPU_dpi_Extcap.o: ../Import/Includes/WindowsToLinux.h
GPU_dpi_Extcap.o: ../Import/Includes/KmsLib/Exception.h
GPU_dpi_Extcap.o: ../Import/Includes/KmsLib/ThreadBase.h
GPU_dpi_Extcap.o: ../Includes/GPU_dpi/System.h ../Includes/GPU_dpi/Status.h
GPU_dpi_Extcap.o: ../Includes/GPU_dpi/GPU_dpi.h ../Includes/GPU_dpi/Types.h
