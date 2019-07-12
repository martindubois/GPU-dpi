
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU_dpi_Test/makefile

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT) -I $(CUDA_FOLDER)/include

LIBRARIES = $(KMS_LIB_A)

OUTPUT = ../Binaries/GPU-dpi_Test

SOURCES = Filter.cpp       \
          GPU_dpi_Test.cpp \
          SetupTool.cpp    \
          System.cpp       \
          TestCase.cpp

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

Filter.o: Component.h ../Import/Includes/KmsBase.h
Filter.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Filter.o: ../Import/Includes/KmsTest.h ../Includes/GPU_dpi/System.h
Filter.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
Filter.o: ../Includes/GPU_dpi/Types.h TestCase.h
GPU_dpi_Test.o: Component.h ../Import/Includes/KmsBase.h
GPU_dpi_Test.o: ../Import/Includes/SafeAPI.h
GPU_dpi_Test.o: ../Import/Includes/WindowsToLinux.h
GPU_dpi_Test.o: ../Import/Includes/KmsTest.h
SetupTool.o: Component.h ../Import/Includes/KmsBase.h
SetupTool.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
SetupTool.o: ../Import/Includes/KmsTest.h ../Includes/GPU_dpi/SetupTool.h
SetupTool.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
System.o: Component.h ../Import/Includes/KmsBase.h
System.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
System.o: ../Import/Includes/KmsTest.h ../Includes/GPU_dpi/System.h
System.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
System.o: ../Includes/GPU_dpi/Types.h
TestCase.o: Component.h ../Import/Includes/KmsBase.h
TestCase.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
TestCase.o: ../Import/Includes/KmsTest.h
TestCase.o: ../Import/Includes/KmsLib/ThreadBase.h TestCase.h
