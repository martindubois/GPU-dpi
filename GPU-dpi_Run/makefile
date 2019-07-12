
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU-dpi_Run/makefile

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT) -I $(CUDA_FOLDER)/include

LIBRARIES = $(KMS_LIB_A)

OUTPUT = ../Binaries/GPU-dpi_Run

SOURCES =	GPU-dpi_Run.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -pthread -L ../Binaries $(BINARY_IMPORT) -L $(CUDA_FOLDER)/lib64 -L $(CUDA_FOLDER)/lib64/stubs -o $@ $^ -l GPU-dpi -lOpenNet -l cudart -l cuda -l nvrtc

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
# DO NOT DELETE

GPU-dpi_Run.o: ../Import/Includes/KmsBase.h ../Import/Includes/SafeAPI.h
GPU-dpi_Run.o: ../Import/Includes/WindowsToLinux.h
GPU-dpi_Run.o: ../Import/Includes/KmsLib/ThreadBase.h
GPU-dpi_Run.o: ../Import/Includes/KmsTool.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/Adapter.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/Processor.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/OpenNet.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/Status.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/StatisticsProvider.h
GPU-dpi_Run.o: ../Import/Includes/OpenNetK/Adapter_Types.h
GPU-dpi_Run.o: ../Import/Includes/OpenNet/System.h
GPU-dpi_Run.o: ../Includes/GPU_dpi/System.h ../Includes/GPU_dpi/Status.h
GPU-dpi_Run.o: ../Includes/GPU_dpi/GPU_dpi.h ../Includes/GPU_dpi/Types.h
GPU-dpi_Run.o: ../Common/Version.h
